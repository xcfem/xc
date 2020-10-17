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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/03/06 20:32:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TransientIntegrator.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/TransientIntegrator.C
// 
// Written: fmk 
// Created: Tue Sept 17 15:54:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::TransientIntegrator.
// TransientIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for tncrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) TransientIntegrator.C, revA"

#include <solution/analysis/integrator/TransientIntegrator.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>


//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
XC::TransientIntegrator::TransientIntegrator(SolutionStrategy *owr,int clasTag)
  : IncrementalIntegrator(owr,clasTag) {}

//! @brief Builds tangent stiffness matrix.
//!
//! Invoked to form the structure tangent matrix. The method is rewritten
//! for this class to include inertia effects from the nodes. The method
//! iterates over both the FE\_Elements and DOF\_Groups invoking methods
//! to form their contributions to the \f$A\f$ matrix of the LinearSOE and
//! then adding these contributions to the \f$A\f$ matrix. The method performs
//! the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theSysOfEqn.zeroA();
//! DOF\_EleIter \&theDofs = theAnalysisModel.getDOFs();
//! while((dofGroupPtr = theDofs()) \f$\neq\f$ 0) \+
//! dofGroupPtr-\f$>\f$formTangent(theIntegrator);
//! theSOE.addA(dofGroupPtr-\f$>\f$getTangent(this),
//! dofGroupPtr-\f$>\f$getID()) \-
//! FE\_EleIter \&theEles = theAnalysisModel.getFEs();
//! while((elePtr = theEles()) \f$\neq\f$ 0) \+
//! theSOE.addA(elePtr-\f$>\f$getTangent(this),
//! elePtr-\f$>\f$getID(), \f$1.0\f$)
//! \end{tabbing}
//! \noindent Returns \f$0\f$ if successful, otherwise a \f$-1\f$ if an error
//! occurred while trying to add the stiffness. The two loops are introduced
//! for the FE\_Elements, to allow for efficient parallel programming when the
//! FE\_Elements are associated with a ShadowSubdomain. 
int XC::TransientIntegrator::formTangent(int statFlag)
  {
    int result = 0;
    statusFlag = statFlag;

    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theLinSOE == 0 || theModel == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING no XC::LinearSOE or XC::AnalysisModel"
	          << " has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations
    
    theLinSOE->zeroA();

    // loop through the DOF_Groups and add the unbalance
    DOF_GrpIter &theDOFGroups = theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
	if(theLinSOE->addA(dofGroupPtr->getTangent(this),dofGroupPtr->getID()) <0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to addA: dof\n";
	    result = -1;
	  }
      }    

    // loop through the FE_Elements getting them to add the tangent    
    FE_EleIter &theEles2 = theModel->getFEs();    
    FE_Element *elePtr;    
    while((elePtr = theEles2()) != 0)
      {
	if(theLinSOE->addA(elePtr->getTangent(this),elePtr->getID()) < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to addA: ele\n";
	    result = -2;
	  }
      }
    return result;
  }

//! @brief Assembles the unbalanced vector of the element
//! being passed as parameter.
//!
//! Called upon by the FE\_Element \p theEle to determine it's
//! contribution to the rhs of the equation. The following are invoked
//! before \f$0\f$ is returned.
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theEle-\f$>\f$zeroResidual()
//! theEle-\f$>\f$addRIncInertiaToResid()
//! \end{tabbing}
int XC::TransientIntegrator::formEleResidual(FE_Element *theEle)
  {
    theEle->zeroResidual();
    theEle->addRIncInertiaToResidual();
    return 0;
  }

//! @brief Builds the unbalanced load vector of the node being passed
//! as parameter.
//!
//! Called upon by the DOF\_Group \p theDof to determine it's
//! contribution to the rhs of the equation. The following are invoked
//! before \f$0\f$ is returned.
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theDof-\f$>\f$zeroUnbalance()
//! theDof-\f$>\f$addPIncInertiaToUnbalance()
//! \end{tabbing}
int XC::TransientIntegrator::formNodUnbalance(DOF_Group *theDof)
  {
    theDof->zeroUnbalance();
    theDof->addPIncInertiaToUnbalance();
    return 0;
  }

