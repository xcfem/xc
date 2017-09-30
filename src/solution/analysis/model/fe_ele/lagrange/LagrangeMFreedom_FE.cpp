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

XC::LagrangeMFreedom_FE::LagrangeMFreedom_FE(int tag, Domain &theDomain, MFreedom_Constraint &TheMP, DOF_Group &theGroup, double Alpha)
  :MFreedom_FE(tag, 3,(TheMP.getConstrainedDOFs().Size()+TheMP.getRetainedDOFs().Size()+TheMP.getRetainedDOFs().Size()),TheMP,Alpha), Lagrange_FE(theGroup)
  {
    const Matrix &constraint = theMP->getConstraint();
    const int noRows = constraint.noRows();
    const int noCols = constraint.noCols();
    const int size = 2*noRows+noCols;
    tang= Matrix(size,size);
    resid= Vector(size);
    tang.Zero();	
    resid.Zero();

    theRetainedNode= theDomain.getNode(theMP->getNodeRetained());    
    theConstrainedNode= theDomain.getNode(theMP->getNodeConstrained());

    if(theRetainedNode == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no associated retained node\n";
	exit(-1);
      }
    
    if(theConstrainedNode == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no asscoiated constrained node\n";
	exit(-1);
      }
    
    if(theMP->isTimeVarying() == false)
      { this->determineTangent(); }
    

    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(1)= determineRetainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(2) = getLagrangeDOFGroup()->getTag();
  }

//! @brief  method to set the corresponding index of the ID to value.
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

const XC::Matrix &XC::LagrangeMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMP->isTimeVarying() == true)
      this->determineTangent();
    return tang;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  { return resid; }



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
    const Matrix &constraint = theMP->getConstraint();
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





