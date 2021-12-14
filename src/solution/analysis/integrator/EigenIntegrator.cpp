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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/EigenIntegrator.cpp,v $
                                                                        
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::EigenIntegrator.
// EigenIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis.
//
// This class is inheritanted from the base class of Integrator which was
// created by fmk (Frank).


#include "solution/analysis/integrator/EigenIntegrator.h"
#include "solution/SolutionStrategy.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>

//! @brief Constructor.
XC::EigenIntegrator::EigenIntegrator(SolutionStrategy *owr)
  :Integrator(owr,EigenINTEGRATOR_TAGS_Eigen) {}

//! @brief Asks the element being passed as parameter
//! to build its mass(flagK= false) or stiffness matrix (flagK= true).
int XC::EigenIntegrator::formEleTangent(FE_Element *theEle)
  {
    if(flagK == 0)
      return formEleTangK(theEle);
    else
      return formEleTangM(theEle);
  }

int XC::EigenIntegrator::formNodTangent(DOF_Group *theDof)
  { return this->formNodTangM(theDof); }

int XC::EigenIntegrator::formEleResidual(FE_Element *theEle)
  { return 0; }

int XC::EigenIntegrator::formNodUnbalance(DOF_Group *theDof)
  { return 0; }

int XC::EigenIntegrator::newStep(void)
  { return 0; }

int XC::EigenIntegrator::getLastResponse(Vector &result, const XC::ID &id)
  { return 0; }

//! @brief Stiffness matrix assembly.
int XC::EigenIntegrator::formK(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    EigenSOE *theSOE= getEigenSOEPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no AnalysisModel or EigenSOE has been set\n";
	return -1;
      }
   
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations

    // loop through the FE_Elements getting them to form the tangent
    // FE_EleIter &theEles1= mdl->getFEs();
    FE_Element *elePtr= nullptr;

    flagK= 0; //stiffness matrix (see formEleTangent)
    theSOE->zeroA();

    //while((elePtr = theEles1()) != 0) 
    //  elePtr->formTangent(this);
   
    // loop through the FE_Elements getting them to add the tangent    
    int result = 0;
    FE_EleIter &theEles2= mdl->getFEs();    
    while((elePtr = theEles2()) != nullptr)
      {
        if(theSOE->addA(elePtr->getTangent(this), elePtr->getID()) < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; WARNING - failed in addA for ID "
		      << elePtr->getID();	    
	    result = -2;
	    break;
	  }
      }
    return result;    
  }

//! @brief Mass matrix assembly.
int XC::EigenIntegrator::formM(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    EigenSOE *theSOE= getEigenSOEPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << "WARNING EigenIntegrator::formM -";
	std::cerr << " no AnalysisModel or EigenSOE has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations


    flagK = 1; //mass matrix (see formEleTangent)
    theSOE->zeroM();

    // while((elePtr = theEles1()) != 0) 
    //     elePtr->formTangent(this);
   
    int result = 0;
    // loop through the FE_Elements getting them to form the tangent
    // FE_EleIter &theEles1 = mdl->getFEs();
    FE_Element *elePtr= nullptr;
    FE_EleIter &theEles2 = mdl->getFEs();    
    while((elePtr = theEles2()) != 0)
      {     
	if(theSOE->addM(elePtr->getTangent(this), elePtr->getID()) < 0)
          {
	    std::cerr << "WARNING EigenIntegrator::formM -";
	    std::cerr << " failed in addM for ID " << elePtr->getID();	    
	    result = -2;
	  }
      }

    DOF_Group *dofGroupPtr= nullptr;
    DOF_GrpIter &theDofs= mdl->getDOFGroups();    
    while((dofGroupPtr = theDofs()) != 0)
      {
	//   	dofGroupPtr->formTangent(this);
	if(theSOE->addM(dofGroupPtr->getTangent(this),dofGroupPtr->getID()) < 0)
          {
	    std::cerr << "WARNING EigenIntegrator::formM -";
	    std::cerr << " failed in addM for ID " << dofGroupPtr->getID();	    
	    result = -3;
	  }
      }
    return result;    
  }

//! @brief Asks the element being passed as parameter
//! to build its tangent stiffness matrix.
int XC::EigenIntegrator::formEleTangK(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addKtToTang(1.0);
    return 0;
  }

//! @brief Asks the element being passed as parameter
//! to build its mass matrix.
int XC::EigenIntegrator::formEleTangM(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addMtoTang(1.0);
    return 0;
  }

int XC::EigenIntegrator::formNodTangM(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang(1.0);
    return 0;
  }

int XC::EigenIntegrator::update(const XC::Vector &deltaU)
  { return 0; }

//! @brief Returns a pointer to the system of equations de eigenvalues.
XC::EigenSOE *XC::EigenIntegrator::getEigenSOEPtr(void)
  {
    SolutionStrategy  *sm= getSolutionStrategy();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
const XC::EigenSOE *XC::EigenIntegrator::getEigenSOEPtr(void) const
  {
    const SolutionStrategy  *sm= getSolutionStrategy();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

int XC::EigenIntegrator::sendSelf(Communicator &comm)
  { return 0; }

int XC::EigenIntegrator::recvSelf(const Communicator &comm)
  { return 0; }

