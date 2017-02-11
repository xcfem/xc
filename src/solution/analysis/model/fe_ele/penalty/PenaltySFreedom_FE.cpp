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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::PenaltySFreedom_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.h>
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
#include <domain/constraints/SFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::PenaltySFreedom_FE::PenaltySFreedom_FE(int tag, Domain &theDomain, SFreedom_Constraint &TheSP, double Alpha)
  :SFreedom_FE(tag, 1,TheSP,Alpha)
  {
    // get a pointer to the XC::Node
    theNode = theDomain.getNode(theSP->getNodeTag());
    if(!theNode)
      {
	std::cerr << "Error en PenaltySFreedom_FE::PenaltySFreedom_FE() - no se encontró el nodo de tag: ";
	std::cerr << theSP->getNodeTag() << " en el domain.\n";
	exit(-1);
      }

    // set the XC::DOF_Group tags
    DOF_Group *dofGrpPtr = theNode->getDOF_GroupPtr();
    if(dofGrpPtr != 0) 
      myDOF_Groups(0) = dofGrpPtr->getTag();	    
    
    // set the tangent
    tang(0,0) = alpha;
  }


// void setID(int index, int value);
//	Method to set the corresponding index of the XC::ID to value.
int XC::PenaltySFreedom_FE::setID(void)
  {
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::PenaltySFreedom_FE::setID(void) - no XC::DOF_Group with XC::Node\n";
	return -2;
      }    
    myDOF_Groups(0) = theNodesDOFs->getTag();
    
    int restrainedDOF = theSP->getDOF_Number();
    if(restrainedDOF < 0 || restrainedDOF >= theNode->getNumberDOF())
      {
	std::cerr << "¡OJO! PenaltySFreedom_FE::setID(); - el nodo: "
                  << theNode->getTag() << " no tiene grado de libertad de índice: "
	          << restrainedDOF << std::endl;
	return -3;
      }    	
    const XC::ID &theNodesID = theNodesDOFs->getID();
    if(restrainedDOF >= theNodesID.Size())
      {
	std::cerr << "WARNING XC::PenaltySFreedom_FE::setID(void) - ";
	std::cerr << " Nodes XC::DOF_Group too small\n";
	return -4;
      }    		
    
    myID(0) = theNodesID(restrainedDOF);

    return 0;
  }


const XC::Matrix &XC::PenaltySFreedom_FE::getTangent(Integrator *theNewIntegrator)
  { return tang; }


const XC::Vector &XC::PenaltySFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    const double constraint = theSP->getValue();
    int constrainedDOF = theSP->getDOF_Number();
    const Vector &nodeDisp = theNode->getTrialDisp();
	
    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << "WARNING XC::PenaltySFreedom_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedDOF << " outside disp\n";
	resid(0) = 0;
      }

    //    resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    // is replace with the following to remove possible problems with
    // subtracting very small numbers

    resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    return resid;
  }


const XC::Vector &XC::PenaltySFreedom_FE::getTangForce(const XC::Vector &disp, double fact)
  {
    //double constraint = theSP->getValue();
    int constrainedID = myID(0);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << "WARNING PenaltySFreedom_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedID << " outside disp\n";
	resid(0) = 0.0;
	return resid;
      }
    resid(0)= alpha * disp(constrainedID);
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getK_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySFreedom_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getC_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySFreedom_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySFreedom_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }




