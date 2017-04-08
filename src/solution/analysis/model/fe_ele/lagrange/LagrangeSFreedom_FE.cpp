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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.cpp,v $
                                                                        
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Purpose: This file contains the code for iSPlementing the methods
// of the LagrangeSFreedom_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::LagrangeSFreedom_FE::LagrangeSFreedom_FE(int tag, Domain &theDomain, SFreedom_Constraint &TheSP,DOF_Group &theGroup, double Alpha)
  :SFreedom_FE(tag,2,TheSP,Alpha), Lagrange_FE(theGroup)
  {
    // zero the XC::Matrix and XC::Vector
    resid.Zero();
    tang.Zero();

    theNode= theDomain.getNode(theSP->getNodeTag());    
    if(theNode == 0)
      {
	std::cerr << "WARNING XC::LagrangeSFreedom_FE::LagrangeSFreedom_FE()";
	std::cerr << "- no asscoiated XC::Node\n";
	exit(-1);
      }

    // set the tangent
    tang(0,1)= alpha;
    tang(1,0)= alpha;
    
    // set the myDOF_Groups tags indicating the attached id's of the
    // DOF_Group objects
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::LagrangeSFreedom_FE::LagrangeSFreedom_FE()";
	std::cerr << " - no XC::DOF_Group with Constrained XC::Node\n";
	exit(-1);	
      }    

    myDOF_Groups(0) = theNodesDOFs->getTag();
    myDOF_Groups(1) = getLagrangeDOFGroup()->getTag();
  }

//! @brief Method to set the correSPonding index of the XC::ID to value.
int XC::LagrangeSFreedom_FE::setID(void)
  {
    int result = 0;

    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::LagrangeSFreedom_FE::setID(void)";
	std::cerr << " - no XC::DOF_Group with Constrained XC::Node\n";
	return -1;
      }

    int restrainedDOF = theSP->getDOF_Number();
    const XC::ID &theNodesID = theNodesDOFs->getID();
    
    if(restrainedDOF < 0 || restrainedDOF >= theNodesID.Size())
      {
	std::cerr << "WARNING XC::LagrangeSFreedom_FE::setID(void)";
	std::cerr << " - restrained DOF invalid\n";
	return -2;
      }
    
    myID(0) = theNodesID(restrainedDOF);
    myID(1) = (getLagrangeDOFGroup()->getID())(0);
    
    return result;
  }

const XC::Matrix &XC::LagrangeSFreedom_FE::getTangent(Integrator *theIntegrator)
  { return tang; }

const XC::Vector &XC::LagrangeSFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    double constraint = theSP->getValue();
    int constrainedDOF = theSP->getDOF_Number();
    const XC::Vector &nodeDisp = theNode->getTrialDisp();

    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << "LagrangeSFreedom_FE::formResidual() -";
	std::cerr << " constrained DOF " << constrainedDOF << " ouside range\n";
	resid(1)= 0;
      }
    
    resid(1)= alpha *(constraint - nodeDisp(constrainedDOF));
    return resid;
  }




const XC::Vector &XC::LagrangeSFreedom_FE::getTangForce(const XC::Vector &disp, double fact)
  {
    double constraint = theSP->getValue();
    int constrainedID = myID(1);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << "WARNING XC::LagrangeSFreedom_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedID << " outside disp\n";
	resid(1)= constraint*alpha;
	return resid;
      }
    resid(1)= disp(constrainedID);
    return resid;    
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getK_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSFreedom_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getC_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSFreedom_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSFreedom_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

