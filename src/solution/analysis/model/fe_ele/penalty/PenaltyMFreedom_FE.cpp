//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::PenaltyMFreedom_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

//! @brief Constructor.
//!
//! To construct a PenaltyMP\_FE element to enforce the constraint
//! specified by the MP\_Constraint \p theMFreedom using a default value for
//! \f$\alpha\f$ of \f$alpha\f$. The FE\_Element class constructor is called
//! with the integers \f$2\f$ and the size of the \p retainedID plus the size of
//! the \p constrainedID at the MP\_Constraint \p theMFreedom. A Matrix
//! and a Vector object are created for adding the contributions to the
//! tangent and the residual. The residual is zeroed. A Matrix is created
//! to store the \f$C\f$ Matrix. If the MP\_Constraint is not time varying,
//! the components of this Matrix are determined, then the contribution
//! to the tangent \f$\alpha C^TC\f$ is determined and finally the \f$C\f$
//! matrix is destroyed. Links are set to the retained and constrained nodes.
//! A warning message is printed and the program is terminated if
//! either not enough memory is available for the Matrices and Vector or the
//! constrained and retained Nodes do not exist in the Domain.
XC::PenaltyMFreedom_FE::PenaltyMFreedom_FE(int tag, Domain &theDomain, 
			   MFreedom_Constraint &TheMP, double Alpha)
:MFreedom_FE(tag, 2,(TheMP.getConstrainedDOFs()).Size()+
	    (TheMP.getRetainedDOFs()).Size(), TheMP,Alpha)
  {
    
    const ID &id1= theMFreedom->getConstrainedDOFs();
    int size= id1.Size();
    const ID &id2= theMFreedom->getRetainedDOFs();    
    size += id2.Size();

    tang= Matrix(size,size);
    resid= Vector(size);
    C= Matrix(id1.Size(),size);

    theRetainedNode = theDomain.getNode(theMFreedom->getNodeRetained());    
    theConstrainedNode = theDomain.getNode(theMFreedom->getNodeConstrained());

    if(theRetainedNode == nullptr || theConstrainedNode == nullptr)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL - Constrained or Retained"
		  << " Node does not exist in Domain\n"
		  << theMFreedom->getNodeRetained() << " "
		  << theMFreedom->getNodeConstrained() << std::endl;
	exit(-1);
      }


    // set up the dof groups tags
    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(1)= determineRetainedNodeDofGrpPtr()->getTag();
    
    if(theMFreedom->isTimeVarying() == false)
      this->determineTangent();
  }

//! @brief Method to set the correMPonding index of the ID to value.
//!
//! Causes the PenaltyMP\_FE to determine the mapping between it's equation
//! numbers and the degrees-of-freedom. This information is obtained by
//! using the mapping information at the DOF\_Group objects associated with
//! the constrained and retained nodes to determine the mappings between
//! the degrees-of-freedom identified in the \p constrainedID and the
//! \p retainedID at the MP\_Constraint \p theMFreedom. Returns \f$0\f$ if
//! successful. Prints a warning message and returns a negative number if
//! an error occurs: \f$-2\f$ if the
//! Node has no associated DOF\_Group, \f$-3\f$ if the constrained DOF
//! specified is invalid for this Node (sets corresponding ID component to
//! \f$-1\f$ so nothing is added to the tangent) and \f$-4\f$ if the ID in the
//! DOF\_Group is too small for the Node (again setting corresponding ID
//! component to \f$-1\f$). 
int XC::PenaltyMFreedom_FE::setID(void)
  {
    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int size1= determineConstrainedDOFsIDs(0);
    
    // then determine the IDs for the retained dof's
    int result= determineRetainedDOFsIDs(size1);
    return result;
  }

//! If the MP\_Constraint is time-varying, from the MP\_Constraint
//! \p theMFreedom it obtains the current \f$C_{cr}\f$ matrix; it then forms the
//! \f$C\f$ matrix and finally it sets the tangent matrix to be \f$\alpha
//! C^TC\f$. Returns the tangent matrix.
const XC::Matrix &XC::PenaltyMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMFreedom->isTimeVarying() == true)
	this->determineTangent();
    return tang;
  }

//! @brief Returns the residual, a \f$0\f$ Vector.
const XC::Vector &XC::PenaltyMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    // zero residual, CD = 0
    return resid;
  }

//! CURRENTLY just returns the \f$0\f$ residual. THIS WILL NEED TO CHANGE FOR
//! ELE-BY-ELE SOLVERS. 
const XC::Vector &XC::PenaltyMFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; WARNING - not yet implemented."
              << std::endl;
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; WARNING - not yet implemented."
              << std::endl;
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; WARNING - not yet implemented."
              << std::endl;
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMFreedom_FE::getM_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; WARNING - not yet implemented."
              << std::endl;
    resid.Zero(); //Added by LCPT.
    return resid;
  }

void XC::PenaltyMFreedom_FE::determineTangent(void)
  {
    // first determine [C] = [-I [Ccr]]
    C.Zero();
    const Matrix &constraint = theMFreedom->getConstraint();
    const int noRows = constraint.noRows();
    const int noCols = constraint.noCols();
    
    for(int j=0; j<noRows; j++)
      C(j,j) = -1.0;
    
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
	C(i,j+noRows) = constraint(i,j);
    

    // now form the tangent: [K] = alpha * [C]^t[C]
    // *(tang) = C^C;
    // *(tang) *= alpha;

    // THIS IS A WORKAROUND UNTIL WE GET addMatrixTransposeProduct() IN
    // THE Matrix CLASS OR UNROLL THIS COMPUTATION
    const int rows= C.noRows();
    const int cols= C.noCols();
    Matrix CT(cols,rows);
    const Matrix &Cref= C;
    // Fill in the transpose of C
    for(int k = 0; k < cols; k++)
      for(int l = 0; l < rows; l++)
	CT(k,l) = Cref(l,k);
    // Compute alpha*(C^C)
    tang.addMatrixProduct(0.0, CT, Cref, alpha);
  }


