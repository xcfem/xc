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

//! Construct a LagrangeSFreedom\_FE element to enforce the constraint.
//!
//! Construct a LagrangeSFreedom\_FE element to enforce the constraint
//! specified by the SFreedom\_Constraint \p theSP using a value for
//! \f$\alpha\f$ of \p alpha (which, if none is specified, defaults to
//! \f$1.0\f$). The FE\_Element class constructor is called with 
//! the integers \f$2\f$ and \f$2\f$. A Matrix and a Vector object of order \f$2\f$
//! are created to return the tangent and residual contributions, with the
//! tangent entries (0,1) and (1,0) set at \f$\alpha\f$. A link to the Node in the Domain
//! corresponding to the SFreedom\_Constraint is also set. A warning message is
//! printed and program terminates if there is not enough memory or no
//! Node associated with the SFreedom\_Constraint exists in the Domain, or
//! DOF\_Group is associated with the Node.
XC::LagrangeSFreedom_FE::LagrangeSFreedom_FE(int tag, Domain &theDomain, SFreedom_Constraint &TheSP,DOF_Group &theGroup, double Alpha)
  :SFreedom_FE(tag,2,TheSP,Alpha), Lagrange_FE(theGroup)
  {
    // zero the Matrix and Vector
    resid.Zero();
    tang.Zero();

    const int nodeTag= theSP->getNodeTag();
    theNode= theDomain.getNode(nodeTag);    
    if(!theNode)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING node: " << nodeTag
	          << " not found." << std::endl;
	exit(-1);
      }

    // set the tangent
    tang(0,1)= alpha;
    tang(1,0)= alpha;
    
    // set the myDOF_Groups tags indicating the attached id's of the
    // DOF_Group objects
    DOF_Group *theNodesDOFs= theNode->getDOF_GroupPtr();
    if(!theNodesDOFs)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING no DOF_Group found for constrained node: "
		  << nodeTag << std::endl;
	exit(-1);
      }    

    myDOF_Groups(0) = theNodesDOFs->getTag();
    myDOF_Groups(1) = getLagrangeDOFGroup()->getTag();
  }

//! @brief Method to set the correSPonding index of the ID to value.
//!
//! Causes the LagrangeSFreedom_FE to determine the mapping between it's equation
//! numbers and the degrees-of-freedom. From the Node object link, created
//! in the constructor, the DOF_group corresponding to the Node
//! associated with the constraint is determined. From this {\em
//! DOF\_Group} object the mapping for the constrained degree of freedom
//! is determined and the myID(0) in the base class is set. The myID(1) is
//! determined from the Lagrange DOF\_Group \p theGroup passed in the
//! constructor. Returns \f$0\f$ if 
//! successful. Prints a warning message and returns a negative number if
//! an error occurs: \f$-2\f$ if the
//! Node has no associated DOF\_Group, \f$-3\f$ if the constrained DOF
//! specified is invalid for this Node and \f$-4\f$ if the ID in the
//! DOF\_Group is too small for the Node. 
int XC::LagrangeSFreedom_FE::setID(void)
  {
    int result = 0;

    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the DOF_Group
    // associated with the constrained node
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(!theNodesDOFs)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING no DOF_Group found for constrained node: "
		  << theNode->getTag() << std::endl;
	return -1;
      }

    const int restrainedDOF = theSP->getDOF_Number();
    const ID &theNodesID = theNodesDOFs->getID();
    
    if(restrainedDOF < 0 || restrainedDOF >= theNodesID.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING restrained DOF: " << restrainedDOF
	          << " is invalid.\n";
	return -2;
      }
    
    myID(0) = theNodesID(restrainedDOF);
    myID(1) = (getLagrangeDOFGroup()->getID())(0);
    
    return result;
  }

//! brief Returns the tangent Matrix created in the constructor.
const XC::Matrix &XC::LagrangeSFreedom_FE::getTangent(Integrator *theIntegrator)
  { return tang; }

//! @brief Sets the FE_Elements contribution to the residual:
//!
//! Sets the FE_Elements contribution to the residual:
//! \[ \left\{ \begin{array}{c} 0 \alpha(u_s - u_t) \end{array} \right\} \]
//! where \f$U_s\f$ is the specified value of the
//! constraint and \f$U_t\f$ the current trial displacement at the node
//! corresponding to constrained degree-of-freedom. Prints a warning
//! message and sets this contribution to \f$0\f$ if the specified constrained
//! degree-of-freedom is invalid. Returns this residual Vector.
const XC::Vector &XC::LagrangeSFreedom_FE::getResidual(Integrator *theNewIntegrator)
  {
    const double constraint = theSP->getValue();
    const int constrainedDOF = theSP->getDOF_Number();
    const Vector &nodeDisp = theNode->getTrialDisp();

    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << " constrained DOF " << constrainedDOF
		  << " outside range.\n";
	resid(1)= 0;
      }
    
    resid(1)= alpha *(constraint - nodeDisp(constrainedDOF));
    return resid;
  }



//! @brief  Sets the FE_Elements contribution to the residual:
//! 
//! Sets the FE_Elements contribution to the residual:
//! \[ \left\{ \begin{array}{c} 0 \alpha(u_s - u_t) \end{array} \right\} \]
//! where \f$U_s\f$ is the specified value of the
//! constraint and \f$U_t\f$ the current trial displacement in \p disp
//! corresponding to constrained degree-of-freedom. Prints a warning
//! message and sets this contribution to \f$0\f$ if the specified constrained
//! degree-of-freedom is invalid. 
const XC::Vector &XC::LagrangeSFreedom_FE::getTangForce(const XC::Vector &disp, double fact)
  {
    const double constraint = theSP->getValue();
    const int constrainedID = myID(1);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << " constrained ID " << constrainedID
		  << " outside disp.\n";
	resid(1)= constraint*alpha;
	return resid;
      }
    resid(1)= disp(constrainedID);
    return resid;    
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getK_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getC_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING not yet implemented\n";
    resid.Zero(); //Added by LCPT.
    return resid;
  }

