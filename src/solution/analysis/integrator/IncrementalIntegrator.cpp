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
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/IncrementalIntegrator.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::IncrementalIntegrator.
//
// What: "@(#) IncrementalIntegrator.C, revA"

#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "solution/AnalysisAggregation.h"
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
XC::IncrementalIntegrator::IncrementalIntegrator(AnalysisAggregation *owr,int clasTag)
  : Integrator(owr,clasTag), statusFlag(CURRENT_TANGENT) {}


//! @brief Builds tangent stiffness matrix.
//!
//! Invoked to form the structure tangent matrix. The method first loops
//! over all the FE\_Elements in the AnalysisModel telling them to form
//! their tangent and then it loops over the FE\_Elements again adding the
//! tangent to the LinearSOE objects A matrix. It performs the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! FE\_EleIter \&theEles = theAnalysisModel.getFEs();
//! theSOE.zeroA();
//! while((elePtr = theEles1()) \f$\neq\f$ 0) \+
//! if (theSOE.addA(elePtr-\f$>\f$getTangent(this),
//! elePtr-\f$>\f$getID(), \f$1.0\f$) \f$<\f$ 0) \+
//! return \f$-1\f$; \-\- 
//! \end{tabbing}
//! \noindent Returns \f$0\f$ if successful, otherwise an error
//! message is printed an a \f$-1\f$ is returned if setLinks() has not
//! been called, or \f$-2\f$ if failure to add an FE\_Elements tangent to the
//! LinearSOE. The two loops are introduced to allow for efficient
//! parallel programming. THIS MAY CHANGE TO REDUCE MEMORY DEMANDS.  
int XC::IncrementalIntegrator::formTangent(int statFlag)
  {
    int result = 0;
    statusFlag = statFlag;
    AnalysisModel *mdl= getAnalysisModelPtr();
    LinearSOE *theSOE= getLinearSOEPtr();

    if(!mdl || !theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING no AnalysisModel or LinearSOE have been set\n";
	return -1;
      }

    theSOE->zeroA(); //Zeroes the matrix elements.
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations - CHANGE

    // loop through the FE_Elements adding their contributions to the tangent
    FE_Element *elePtr;
    FE_EleIter &theEles2= mdl->getFEs();    
    while((elePtr = theEles2()) != 0)  
      if(theSOE->addA(elePtr->getTangent(this),elePtr->getID()) < 0)
        {
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; WARNING failed in addA for ID "
		    << elePtr->getID();	    
	  result = -3;
	}
    return result;
  }

//! @brief Builds the unbalanced load vector (right hand side of the equation).
//!
//! Invoked to form the unbalance. The method fist zeros out the \f$B\f$
//! vector of the LinearSOE object and then invokes formElementResidual()
//! and formNodalUnbalance() on itself.
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theSOE.zeroB();
//! this-\f$>\f$fromElementResidual();
//! this-\f$>\f$formNodalUnbalance()
//! \end{tabbing}
//!
//! \noindent If an error occurs in either of these two methods or if {\em
//! setLinks()} has not been called, an error message is printed and a
//! negative number is returned. Returns \f$0\f$ if successful. 
int XC::IncrementalIntegrator::formUnbalance(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    if((!mdl) || (!theSOE))
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING no AnalysisModel or LinearSOE has been set\n";
	return -1;
      }
    
    theSOE->zeroB();
    
    if(formElementResidual() < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: this->formElementResidual failed\n";
	return -1;
      }
    
    if(formNodalUnbalance() < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: this->formNodalUnbalance failed\n";
	return -2;
      }
    return 0;
  }
  
//! @brief Returns the response on the DOFs being passed as parameter.
//!
//! Returns in \p result values for the last solution to the system of
//! equation object whose location in the solution vector is given by {\em
//! id}. For a location specified by a negative integer in \p id 0.0 will be
//! returned in \p result. Returns a \f$0\f$ if successful, a warning
//! message and a negative number is returned if an error occurs. \f$-1\f$ if
//! setSize() has not been called and a \f$-2\f$ if location in \p id is
//! greater than \f$order-1\f$ of \f$b\f$ vector.
//!
//! @param id: dofs identifiers on which the response is requested.
//! @param result: respones values.
int XC::IncrementalIntegrator::getLastResponse(Vector &result, const XC::ID &id)
  {
    LinearSOE *theSOE= getLinearSOEPtr();
    if(!theSOE)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING no LineaerSOE object not set.\n";	
	return -1;
      }

    int res = 0; 
    int size = theSOE->getNumEqn() -1;
    const Vector &X = theSOE->getX();
    for(int i=0; i<id.Size(); i++)
      {
	int loc = id(i);
	if(loc < 0 )
	  result(i) = 0.0;
	else if(loc <= size) //DOF exists.
          { result(i) = X(loc);	}
	else
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING location " << loc
		      << "in ID outside bounds ";
	    std::cerr << size << "\n";	
	    res = -2;
	  }
      }	    
    return res;
  }

//! @brief ??
int XC::IncrementalIntegrator::newStep(double deltaT)
  { return 0; }


int XC::IncrementalIntegrator::initialize(void)
  { return 0; }

//! @brief Invoked by the SolutionAlgorithm to inform the Integrator
//! that current state of domain is on solution path.
//! 
//! Invoked by the SolutionAlgorithm to inform the Integrator that current
//! state of domain is on solution path. Returns the result of invoking
//! commitDomain() on the AnalysisModel object associated with the
//! Integrator.
int XC::IncrementalIntegrator::commit(void) 
  {
    int retval= -1;
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING AnalysisModel object not set.\n";	
      }
    else
      retval= commitModel();
    return retval;
  }


int XC::IncrementalIntegrator::revertToLastStep(void) 
  { return 0; }   

int XC::IncrementalIntegrator::revertToStart()
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "ERROR: method not yet implemented "
	      << " for the chosen type of integrator. " << std::endl;
    return 0;
  }

//! @brief Returns a pointer to the linear system of equations.
//!
//! A member function which returns a pointer to the LinearSOE
//! associated with the IncrementalIntegrator object, i.e. \p theSOE.
XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Returns a pointer to the linear system of equations.
//!
//! A const member function which returns a const pointer to the LinearSOE
//! associated with the IncrementalIntegrator object, i.e. \p theSOE.
const XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Builds the nodal unbalanced load vector.
//!
//! The method first loops over all the DOF\_Group objects telling them to
//! form their unbalance and then adds this Vector to the \f$b\f$ vector of
//! the LinearSOE object, i.e. it performs the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! DOF\_EleIter \&theDofs = theAnalysisModel.getDOFs();
//! theSOE.zeroB();
//! while((dofGroupPtr = theDofs()) \f$\neq\f$ 0) \+
//! theSOE.addB(dofGroupPtr-\f$>\f$getUnbalance(theIntegrator),
//! dofGroupPtr-\f$>\f$getID())
//! \end{tabbing}
//! \noindent Returns \f$0\f$ if successful, otherwise a  negative number is
//! returned and a warning message is printed if an error occurred. Note,
//! no test is made to ensure setLinks() has been invoked.
int XC::IncrementalIntegrator::formNodalUnbalance(void)
  {
    // loop through the DOF_Groups and add the unbalance
    LinearSOE *theSOE= getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    DOF_GrpIter &theDOFGroups= mdl->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    int res = 0;

    while((dofGroupPtr = theDOFGroups()) != nullptr)
      { 
        //      std::cerr << getClassName() << "::" << __FUNCTION__
	//                << "NODPTR: " << dofGroupPtr->getUnbalance(this);
	if(theSOE->addB(dofGroupPtr->getUnbalance(this),dofGroupPtr->getID()) <0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING failed in addB for ID: "
		      << dofGroupPtr->getID();
	    res = -2;
	  }
      }
    return res;
  }

//! @brief Builds the unbalanced load vector of the elements.
//! 
//! Invoked to form residual vector (the C vector in theSOE). The method
//! iterates twice over the FE\_elements in the AnalysisModel, the first
//! time telling the FE\_Elements top form their residual and the second
//! time to add this residual to the LinearSOE objects \f$b\f$ vector, i.e. it
//! performs the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! FE\_EleIter \&theEles = theAnalysisModel.getFEs();
//! while((elePtr = theEles()) \f$\neq\f$ 0) \{ \+
//! theSOE.addA(elePtr-\f$>\f$getResidual(this), elePtr-\f$>\f$getID()) \+ 
//! \end{tabbing}
//! \noindent Returns \f$0\f$ if successful, otherwise a warning message is
//! printed and a negative number is returned if an error occurs. Note, no
//! test is made to ensure setLinks() has been invoked.
int XC::IncrementalIntegrator::formElementResidual(void)
  {
    // loop through the FE_Elements and add the residual
    FE_Element *elePtr;

    int res = 0;    

    LinearSOE *theSOE= getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    FE_EleIter &theEles2 = mdl->getFEs();
    while((elePtr= theEles2()) != nullptr)
      {
	if(theSOE->addB(elePtr->getResidual(this),elePtr->getID()) <0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING failed in addB for ID: "
		      << elePtr->getID();
	    res = -2;
	  }
      }
    return res;	    
  }

