//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::LagrangeMFreedom_FE class interface.

#include <solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.h>
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
#include "utility/utils/misc_utils/colormod.h"

// Note 1:
//    changed by M.Petracca: 2024. should be the constrained dof size.
//    now it's the same because all MP constraints in opensees are one-to-one.
//    but in the future we may implement generic constraints of the form:
//    CDOF = a1*RDOF1 + a2*RDOF2 + ... + an*RDOFn + beta

//!To construct a LagrangeMFreedom\_FE element to enforce the constraint
//! specified by the MFreedom\_Constraint \p theMFreedom using a default value
//! for \f$\alpha\f$ of \f$alpha\f$. The FE\_Element class constructor is called
//! with the integers \f$3\f$ and the two times the size of the \p retainedID
//! plus the size of the \p constrainedID at the MFreedom\_Constraint {\em
//! theMFreedom} plus . A Matrix and a Vector object are created for adding the
//! contributions to the tangent and the residual. The residual is
//! zeroed. If the MFreedom\_Constraint is not time varying, then the
//! contribution to the tangent is determined. Links are set to the retained
//! and constrained nodes. The DOF\_Group tag ID is set using the tag of the
//! constrained Nodes DOF\_Group, the tag of the retained Node Dof\_group and
//! the tag of the LagrangeDOF\_Group, \p theGroup. A warning message is
//! printed and the program is terminated if either not enough memory is
//! available for the Matrices and Vector or the constrained and retained Nodes
//! of their DOF\_Groups do not exist.
XC::LagrangeMFreedom_FE::LagrangeMFreedom_FE(int tag, Domain &theDomain, MFreedom_Constraint &TheMFreedom, DOF_Group &theGroup, double Alpha)
  :MFreedom_FE(tag, 3,(TheMFreedom.getConstrainedDOFs().Size()+
		       TheMFreedom.getRetainedDOFs().Size()+
		       TheMFreedom.getConstrainedDOFs().Size()), // *see note 1
	       TheMFreedom,Alpha), Lagrange_FE(theGroup)
  {
    const Matrix &constraint = theMFreedom->getConstraint();
    const int noRows = constraint.noRows();
    const int noCols = constraint.noCols();
    const int size = 2*noRows+noCols;
    tang= Matrix(size,size);
    resid= Vector(size);
    tang.Zero();	
    resid.Zero();

    this->theRetainedNode= theDomain.getNode(theMFreedom->getNodeRetained());
    if(theRetainedNode == nullptr)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no associated retained node."
	          << Color::def << std::endl;
	exit(-1);
      }
    
    this->theConstrainedNode= theDomain.getNode(theMFreedom->getNodeConstrained());
    if(theConstrainedNode == nullptr)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; no associated constrained node."
	          << Color::def << std::endl;
	exit(-1);
      }
    
    if(theMFreedom->isTimeVarying() == false)
      { this->determineTangent(); }

    const DOF_Group *theConstrainedNodesDOFs= this->determineConstrainedNodeDofGrpPtr();
    if(this->getLagrangeDOFGroup()->getID().Size() != theConstrainedNodesDOFs->getID().Size())
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - ConstrainedDOFs size != Lagrange size"
		  << Color::def << std::endl;
        exit(-1);
      }
    
    const DOF_Group *theRetainedNodesDOFs= this->determineRetainedNodeDofGrpPtr();

    
    myDOF_Groups(0)= theConstrainedNodesDOFs->getTag();
    myDOF_Groups(1)= theRetainedNodesDOFs->getTag();
    myDOF_Groups(2)= getLagrangeDOFGroup()->getTag();
  }

//! @brief  method to set the corresponding index of the ID to value.
//!
//! Causes the LagrangeMFreedom\_FE to determine the mapping between it's equation
//! numbers and the degrees-of-freedom. This information is obtained by
//! using the mapping information at the DOF\_Group objects associated with
//! the constrained and retained nodes and the LagrangeDOF\_Group, {\em
//! theGroup}. Returns \f$0\f$ if
//! successful. Prints a warning message and returns a negative number if
//! an error occurs: \f$-2\f$ if the
//! Node has no associated DOF\_Group, \f$-3\f$ if the constrained DOF
//! specified is invalid for this Node (sets corresponding ID component to
//! \f$-1\f$ so nothing is added to the tangent) and \f$-4\f$ if the ID in the
//! DOF\_Group is too small for the Node (again setting corresponding ID
//! component to \f$-1\f$). 
int XC::LagrangeMFreedom_FE::setID(void)
  {
    int offset = 0;

    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int offset1= determineConstrainedDOFsIDs(0);
    if(offset1>=0) offset= offset1;
 
    // then determine the IDs for the retained dof's
    const int offset2= determineRetainedDOFsIDs(offset1);
    if(offset2>=0) offset= offset2;
    
    // finally set the ID corresponding to the ID's at the LagrangeDOF_Group
    const ID &theGroupsID = getLagrangeDOFGroup()->getID();
    int size3 = theGroupsID.Size();
    for(int k=0; k<size3; k++) 
      myID(k+offset) = theGroupsID(k);
    
    return offset;
  }

//! If the MFreedom\_Constraint is time-varying, from the MFreedom\_Constraint
//! \p theMFreedom it obtains the current \f$C_{cr}\f$ matrix; it then adds the
//! contribution to the tangent matrix. Returns this tangent Matrix.
const XC::Matrix &XC::LagrangeMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMFreedom->isTimeVarying() == true)
      this->determineTangent();
    return tang;
  }

//! @brief Returns the residual vector.
const XC::Vector &XC::LagrangeMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    // get the solution vector [Uc Ur lambda]
    static Vector UU;
    const ID &id1= theMFreedom->getConstrainedDOFs();
    const int id1Sz= id1.Size();
    const ID &id2= theMFreedom->getRetainedDOFs();
    const int id2Sz= id2.Size();
    const DOF_Group *theDofGroup= this->getLagrangeDOFGroup();
    const ID &id3= theDofGroup->getID(); 
    const int size= id1Sz + id2Sz + id3.Size();
    UU.resize(size);
    // Constrained DOFs.
    this->assemble_constrained_DOF_displacements(UU);

    // Retained DOFs.
    this->assemble_retained_DOF_displacements(UU, id1Sz);

    // Lagrange DOFs.
    const int id3Sz= id3.Size();    
    const Vector &lambda = theDofGroup->getTrialDisp();
    for(int i = 0; i < id3Sz; ++i)
      {
        UU(i + id1Sz + id2Sz) = lambda(i);
      }

    /*
    R = -C*U + G
       .R = generalized residual vector
       .C = constraint matrix
       .U = generalized solution vector (displacement, lagrange multipliers)
       .G = constrain values for non-homogeneous MP constraints (not available now)
    | Ru |    | 0  A | | u |   | 0 |
    |    | = -|      |*|   | + |   |
    | Rl |    | A  0 | | l |   | g |
    */

    // compute residual
    const Matrix &KK= getTangent(theNewIntegrator);
    resid.addMatrixVector(0.0, KK, UU, -1.0);

    // done
    return resid;
  }

//! @brief CURRENTLY just returns the \f$0\f$ residual. THIS WILL NEED
//! TO CHANGE FOR ELE-BY-ELE SOLVERS. 
const XC::Vector &XC::LagrangeMFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return  resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getKi_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

void XC::LagrangeMFreedom_FE::determineTangent(void)
  {
    const Matrix &constraint = theMFreedom->getConstraint();
    const int noRows= constraint.noRows();
    const int noCols= constraint.noCols();
    const int n= noRows+noCols;
    
    tang.Zero();    

    for(int j=0; j<noRows; j++)
      {
	tang(n+j, j)= -alpha;
	tang(j, n+j)= -alpha;	
      }
    
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
        {
	  const double val = constraint(i,j) * alpha;
	  tang(n+i, j+noRows) = val;
	  tang(noRows+j, n+i) = val;
	}

  }





