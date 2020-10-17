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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DirectIntegrationAnalysis.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of the
// DirectIntegrationAnalysis class.
//
// What: "@(#) DirectIntegrationAnalysis.C, revA"


#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>
#include "solution/analysis/ModelWrapper.h"
#include "solution/SolutionStrategy.h"
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/domain/Domain.h>

// AddingSensitivity:BEGIN //////////////////////////////////
#ifdef _RELIABILITY
#include <reliability/FEsensitivity/SensitivityAlgorithm.h>
#endif
// AddingSensitivity:END ////////////////////////////////////

//! @brief Constructor.
//! @param solution_strategy: solution strategy to which this analysis belongs.
XC::DirectIntegrationAnalysis::DirectIntegrationAnalysis(SolutionStrategy *solution_strategy)
  :TransientAnalysis(solution_strategy), domainStamp(0)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
	theSensitivityAlgorithm= nullptr;
#endif
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief Destructor.
//! Does nothing. clearAll() must be invoked if the destructor on
//! the objects in the aggregation need to be invoked.
XC::DirectIntegrationAnalysis::~DirectIntegrationAnalysis(void)
  {
    // we don't invoke the destructors in case user switching
    // from a static to a direct integration analysis 
    // clearAll() must be invoked if user wishes to invoke destructor
  }

//! @brief Clears all object members (constraint handler, analysis model,...).
//!
//! Will invoke the destructor on all the objects in the aggregation. NOTE
//! this means they must have been constructed using new(),
//! otherwise a segmentation fault can occur.
void XC::DirectIntegrationAnalysis::clearAll(void)
  {
// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
    delete theSensitivityAlgorithm;
#endif
// AddingSensitivity:END //////////////////////////////////////
  }    

int XC::DirectIntegrationAnalysis::initialize(void)
  {
    assert(solution_strategy);
    Domain *the_Domain = solution_strategy->getDomainPtr();

    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;	
        if(this->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged() failed\n";
	    return -1;
          }	
      }
    if(solution_strategy->getTransientIntegratorPtr()->initialize() < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; integrator initialize() failed\n";
	return -2;
      }
    else
      solution_strategy->getTransientIntegratorPtr()->commit();
    return 0;
  }

//! @brief Performs the analysis.
//!
//! @param numSteps: number of steps in the analysis.
//! @param dT: time increment.
//!
//! Invoked to perform a transient analysis on the FE\_Model. The method
//! checks to see if the domain has changed before it performs the
//! analysis.
//! The type of analysis performed, depends on the type of the
//! objects in the analysis aggregation. If any of the methods invoked
//! returns a negative number, an error message is printed, {\em
//! revertToLastCommit()} is invoked on the Domain, and a negative number
//! is immediately returned. Returns a \f$0\f$ if the algorithm is successful.
int XC::DirectIntegrationAnalysis::analyze(int numSteps, double dT)
  {
    int result= 0;
    assert(solution_strategy);
    CommandEntity *old= solution_strategy->Owner();
    solution_strategy->set_owner(this);
    Domain *the_Domain = solution_strategy->getDomainPtr();

    for(int i=0; i<numSteps; i++)
      {
        if(newStepDomain(solution_strategy->getModelWrapperPtr()->getAnalysisModelPtr(),dT) < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the AnalysisModel failed"
		      << " at time "
		      << the_Domain->getTimeTracker().getCurrentTime()
		      << std::endl;
	    the_Domain->revertToLastCommit();
	    return -2;
          }

        // check if domain has undergone change
        int stamp = the_Domain->hasDomainChanged();
        if(stamp != domainStamp)
          {
	    domainStamp = stamp;	
	    if(this->domainChanged() < 0)
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; domainChanged() failed\n";
	        return -1;
              }	
          }

        if(solution_strategy->getTransientIntegratorPtr()->newStep(dT) < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the XC::Integrator failed at time "
		      << the_Domain->getTimeTracker().getCurrentTime()
		      << std::endl;
	    the_Domain->revertToLastCommit();
	    return -2;
          }

        result = solution_strategy->getEquiSolutionAlgorithmPtr()->solveCurrentStep();
        if(result < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Algorithm failed at time "
		      << the_Domain->getTimeTracker().getCurrentTime()
		      << std::endl;
	    the_Domain->revertToLastCommit();	    
	    solution_strategy->getTransientIntegratorPtr()->revertToLastStep();
	    return -3;
          }    

// AddingSensitivity:BEGIN ////////////////////////////////////
#ifdef _RELIABILITY
        if(theSensitivityAlgorithm != 0)
          {
	    result = theSensitivityAlgorithm->computeSensitivities();
	    if(result < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; the SensitivityAlgorithm failed"
			  << " at iteration: " << i
			  << " with domain at load factor "
			  << the_Domain->getTimeTracker().getCurrentTime()
			  << std::endl;
	        the_Domain->revertToLastCommit();	    
	        solution_strategy->getTransientIntegratorPtr()->revertToLastStep();
	        return -5;
	      }
          }
#endif
// AddingSensitivity:END //////////////////////////////////////
      
        result= solution_strategy->getTransientIntegratorPtr()->commit();
        if(result < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Integrator failed to commit at time "
		      << the_Domain->getTimeTracker().getCurrentTime()
		      << std::endl;
	    the_Domain->revertToLastCommit();	    
	    solution_strategy->getTransientIntegratorPtr()->revertToLastStep();
	    return -4;
          }
      }    
    solution_strategy->set_owner(old);
    return result;
  }

//! @brief Execute the changes following a change in the domain.
//!
//! This is a method invoked by a domain which indicates to the analysis
//! that the domain has changed. The method invokes the following:
//! - It invokes clearAll() on \p theModel which causes the
//!   AnalysisModel to clear out its list of FE\_Elements and DOF\_Groups,
//!   and clearAll() on \p theHandler.
//! - It then invokes handle() on \p theHandler. This causes
//!   the constraint handler to recreate the appropriate FE\_Element and
//!   DOF\_Groups to perform the analysis subject to the boundary conditions
//!   in the modified domain.
//! - It then invokes number() on \p theNumberer. This causes
//!   the DOF numberer to assign equation numbers to the individual
//!   dof's. Once the equation numbers have been set the numberer then
//!   invokes setID() on all the FE\_Elements in the model. Finally
//!   the numberer invokes setNumEqn() on the model.
//! - It then invokes domainChanged() on \p theIntegrator and
//!   theAlgorithm to inform these objects that changes have occurred
//!   in the model.
//! - It invokes {\em setSize(theModel.getDOFGraph())} on {\em
//!   theSOE} which causes the system of equation to determine its size
//!   based on the connectivity of the dofs in the analysis model. 
//! - Finally it invokes domainChanged() on \p theIntegrator and theAlgorithm. 
//!   Returns \f$0\f$ if successful. At any stage above, if an error occurs the
//!   method is stopped, a warning message is printed and a negative number
//!   is returned. 
int XC::DirectIntegrationAnalysis::domainChanged(void)
  {
    assert(solution_strategy);
    Domain *the_Domain = solution_strategy->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    domainStamp = stamp;
   
    solution_strategy->getModelWrapperPtr()->getAnalysisModelPtr()->clearAll();    
    solution_strategy->getModelWrapperPtr()->getConstraintHandlerPtr()->clearAll();
    
    // now we invoke handle() on the constraint handler which
    // causes the creation of XC::FE_Element and XC::DOF_Group objects
    // and their addition to the XC::AnalysisModel.

    solution_strategy->getModelWrapperPtr()->getConstraintHandlerPtr()->handle();
    // we now invoke number() on the numberer which causes
    // equation numbers to be assigned to all the DOFs in the
    // AnalysisModel.


    solution_strategy->getModelWrapperPtr()->getDOF_NumbererPtr()->numberDOF();

    solution_strategy->getModelWrapperPtr()->getConstraintHandlerPtr()->doneNumberingDOF();

    // we invoke setGraph() on the XC::LinearSOE which
    // causes that object to determine its size

    solution_strategy->getLinearSOEPtr()->setSize(solution_strategy->getModelWrapperPtr()->getAnalysisModelPtr()->getDOFGraph());

    // we invoke domainChange() on the integrator and algorithm
    solution_strategy->getTransientIntegratorPtr()->domainChanged();
    solution_strategy->getEquiSolutionAlgorithmPtr()->domainChanged();


    return 0;
  }    

// AddingSensitivity:BEGIN //////////////////////////////
#ifdef _RELIABILITY
int XC::DirectIntegrationAnalysis::setSensitivityAlgorithm(SensitivityAlgorithm *passedSensitivityAlgorithm)
  {
    int result = 0;

    // invoke the destructor on the old one
    if(theSensitivityAlgorithm)
      { delete theSensitivityAlgorithm; }

    theSensitivityAlgorithm = passedSensitivityAlgorithm;
    return 0;
  }
#endif
// AddingSensitivity:END ///////////////////////////////

//! @brief Sets the renumerador to use in the analysis.
int XC::DirectIntegrationAnalysis::setNumberer(DOF_Numberer &theNewNumberer) 
  {
    int result= TransientAnalysis::setNumberer(theNewNumberer);

    // invoke domainChanged() either indirectly or directly
    Domain *the_Domain = solution_strategy->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    domainStamp = stamp;
    result = this->domainChanged();    
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; setNumberer() failed";
        return -1;
      }	
    return 0;
  }


//! @brief Sets the solutio algorithm to use in the analysis.
//!
//! To change the algorithm between analysis. It first invokes the
//! destructor on the old SolutionAlgorithm object associated with the
//! analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newAlgorithm and sets the
//! links for this object by invoking setLinks(). Checks then to
//! see if the domain has changed, if true it invokes {\em
//! domainChanged()}, otherwise it invokes domainChanged() on the
//! new SolutionAlgorithm. Returns \f$0\f$ if successful, a warning message
//! and a negative number if not.
int XC::DirectIntegrationAnalysis::setAlgorithm(EquiSolnAlgo &theNewAlgorithm) 
  {
    // invoke the destructor on the old one
    TransientAnalysis::setAlgorithm(theNewAlgorithm);

    // invoke domainChanged() either indirectly or directly
    assert(solution_strategy);
    Domain *the_Domain = solution_strategy->getDomainPtr();
    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;	    
	if(this->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged failed";
	    return -1;
	  }	
      }
    else
      {
	if(solution_strategy->getEquiSolutionAlgorithmPtr()->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; algorithm::domainChanged() failed";
	    return -2;
	  }
      }
    return 0;
  }

//! @brief Sets the integrator to use in the analysis.
//!
//! To change the integration scheme between analysis. It first invokes the
//! destructor on the old Integrator object associated with the
//! analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newAlgorithm and sets the
//! links for this object by invoking setLinks(). It also invokes
//! setLinks() on the ConstraintHandler and SolutionAlgorithm
//! objects. Checks then to see if the domain has changed, if true it
//! invokes domainChanged(), otherwise it invokes {\em
//! domainChanged()} on the new Integrator. Returns \f$0\f$ if
//! successful, a warning message and a negative number if not.
int XC::DirectIntegrationAnalysis::setIntegrator(TransientIntegrator &theNewIntegrator)
  {
    // set the links needed by the other objects in the aggregation
    TransientAnalysis::setIntegrator(theNewIntegrator);

    // invoke domainChanged() either indirectly or directly
    int stamp = solution_strategy->getDomainPtr()->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;	    
	if(this->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged failed";
	    return -1;
          }	
      }
    else
      {
        if(solution_strategy->getTransientIntegratorPtr()->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; Integrator::domainChanged failed";
	    return -1;
          }	
      }
    return 0;
  }

//! @brief Sets the linear system of equations to use in the analysis.
//!
//! To change the linear system of equation object between analysis. It
//! first invokes the destructor on the old LinearSOE object associated
//! with the analysis. It then sets the SolutionAlgorithm 
//! associated with the analysis to be \p newSOE.
//! links for this object by invoking setLinks(). It then invokes
//! setLinks() on the ConstraintHandler and SolutionAlgorithm
//! objects. Checks then to see if the domain has changed, if true it
//! invokes domainChanged(), otherwise it invokes {\em
//! setSize()} on the new LinearSOE. Returns \f$0\f$ if successful, a warning
//! message and a negative number if not.
int XC::DirectIntegrationAnalysis::setLinearSOE(LinearSOE &theNewSOE)
  {
    // invoke the destructor on the old one
    TransientAnalysis::setLinearSOE(theNewSOE);

    // set the links needed by the other objects in the aggregation

    // set the size either indirectly or directly
    assert(solution_strategy);
    Domain *the_Domain= solution_strategy->getDomainPtr();
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;	    
        if(this->domainChanged() < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged failed";
	    return -1;
          }	
      }
    else
      {
        Graph &theGraph = solution_strategy->getModelWrapperPtr()->getAnalysisModelPtr()->getDOFGraph();
        if(solution_strategy->getLinearSOEPtr()->setSize(theGraph) < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; LinearSOE::setSize() failed";
	    return -2;	
          }
      }
    return 0;
  }

//! @brief Sets the convergence test to use in the analysis.
int XC::DirectIntegrationAnalysis::setConvergenceTest(ConvergenceTest &theNewTest)
  {
    if(solution_strategy)
      solution_strategy->setConvergenceTest(theNewTest);
    return 0;
  }

//! @brief Comprueba si el domain ha cambiado.
int XC::DirectIntegrationAnalysis::checkDomainChange(void)
  {
    assert(solution_strategy);
    Domain *the_Domain = solution_strategy->getDomainPtr();

    // check if domain has undergone change
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
        domainStamp = stamp;	
        if(this->domainChanged() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; domainChanged() failed\n";
            return -1;
          }	
      }
    return 0;
  }




