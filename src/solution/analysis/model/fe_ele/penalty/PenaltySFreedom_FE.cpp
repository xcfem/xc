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

//! @brief Constructor.
//!
//! To construct a PenaltySFreedom\_FE element to enforce the constraint
//! specified by the SFreedom\_Constraint \p theSP using a value for
//! \f$\alpha\f$ of \p alpha (which, if none is specified, defaults to
//! \f$1.0e8\f$). The FE\_Element class constructor is called with 
//! the integers \f$1\f$ and \f$1\f$. A Matrix and a Vector object of
//! order \f$1\f$ are created to return the tangent and residual contributions,
//! with the tangent entry being set at \f$\alpha\f$. A link to the Node in the
//! Domain corresponding to the SFreedom\_Constraint is also set. A warning
//! message is printed and program terminates if there is not enough memory or
//! no node associated with the SFreedom\_Constraint exists in the Domain.
XC::PenaltySFreedom_FE::PenaltySFreedom_FE(int tag, Domain &theDomain, SFreedom_Constraint &TheSP, double Alpha)
  :SFreedom_FE(tag, 1,TheSP,Alpha)
  {
    // get a pointer to the node
    theNode = theDomain.getNode(theSP->getNodeTag());
    if(!theNode)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; node identified by: "
	          << theSP->getNodeTag() << " not found.\n";
	exit(-1);
      }

    // set the DOF_Group tags
    DOF_Group *dofGrpPtr = theNode->getDOF_GroupPtr();
    if(dofGrpPtr) 
      myDOF_Groups(0) = dofGrpPtr->getTag();	    
    
    // set the tangent
    tang(0,0) = alpha;
  }


//! @brief Method to set the corresponding index of the ID.
//!
//! Causes the PenaltySFreedom\_FE to determine the mapping between it's
//! equation numbers and the degrees-of-freedom. From the Node object link,
//! created! in the constructor, the DOF\_group corresponding to the Node
//! associated with the constraint is determined. From this {\em
//! DOF\_Group} object the mapping for the constrained degree of freedom
//! is determined and the ID in the base class is set. Returns \f$0\f$ if
//! successful. Prints a warning message and returns a negative number if
//! an error occurs: \f$-2\f$ if the
//! Node has no associated DOF\_Group, \f$-3\f$ if the constrained DOF
//! specified is invalid for this Node and \f$-4\f$ if the ID in the
//! DOF\_Group is too small for the Node. 
int XC::PenaltySFreedom_FE::setID(void)
  {
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(!theNodesDOFs)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - no DOF_Group associated with node.\n";
	return -2;
      }    
    myDOF_Groups(0) = theNodesDOFs->getTag();
    
    int restrainedDOF = theSP->getDOF_Number();
    if(restrainedDOF < 0 || restrainedDOF >= theNode->getNumberDOF())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ¡WARNING! - node: "
                  << theNode->getTag() << " has not the DOF: "
	          << restrainedDOF << std::endl;
	return -3;
      }    	
    const ID &theNodesID = theNodesDOFs->getID();
    if(restrainedDOF >= theNodesID.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - nodes DOF_Group too small.\n";
	return -4;
      }    		
    
    myID(0) = theNodesID(restrainedDOF);

    return 0;
  }

//! @brief Returns the tangent Matrix created in the constructor.
const XC::Matrix &XC::PenaltySFreedom_FE::getTangent(Integrator *theNewIntegrator)
  { return tang; }

//! Sets the FE\_Elements contribution to the residual to be
//! \f$\alpha * (U_s - U_t)\f$, where \f$U_s\f$ is the specified value of the
//! constraint and \f$U_t\f$ the current trial displacement at the node
//! corresponding to constrained degree-of-freedom. Prints a warning
//! message and sets this contribution to \f$0\f$ if the specified constrained
//! degree-of-freedom is invalid. Returns this residual Vector set.
const XC::Vector &XC::PenaltySFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    const double constraint = theSP->getValue();
    const int constrainedDOF = theSP->getDOF_Number();
    const Vector &nodeDisp = theNode->getTrialDisp();
	
    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING; - constrained DOF "
		  << constrainedDOF << " outside disp.\n";
	resid(0) = 0;
      }

    // resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    // is replace with the following to remove possible problems with
    // subtracting very small numbers

    resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    return resid;
  }

//! Sets the FE\_Elements contribution to the residual to be
//! \f$\alpha * (U\_s - disp\_t)\f$, where \f$U\_s\f$ is the specified value
//! of the constraint and \f$disp\_t\f$ the value in \p disp
//! corresponding to constrained degree-of-freedom. Prints a warning
//! message and sets this contribution to \f$0\f$ if the mapping, determined in
//! setID(), for the the specified constrained degree-of-freedom lies 
//! outside \p disp.  
const XC::Vector &XC::PenaltySFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    //double constraint = theSP->getValue();
    int constrainedID= myID(0);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - constrained DOF "
		  << constrainedID << " outside disp.\n";
	resid(0) = 0.0;
      }
    else
      resid(0)= alpha * disp(constrainedID);
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySFreedom_FE::getM_Force(const Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING - not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }




