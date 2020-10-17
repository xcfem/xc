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
// $Date: 2005/12/06 18:20:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.C
// 
// Written: fmk 
// Created: 10/00
// Revision: A
//
// Description: This file contains the implementation of the
// VariableTimeStepDirectIntegrationAnalysis class.
//
// What: "@(#) VariableTimeStepDirectIntegrationAnalysis.C, revA"

#include <solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <cfloat>
#include "solution/SolutionStrategy.h"

//! @brief Constructor.
XC::VariableTimeStepDirectIntegrationAnalysis::VariableTimeStepDirectIntegrationAnalysis(SolutionStrategy *analysis_aggregation)
  :DirectIntegrationAnalysis(analysis_aggregation) {}    

//! @brief Performs the analysis.
//! 
//! @param numSteps: number of steps in the analysis.
//! @param dT: time increment.
//! @param dtMin: Minimum value for the time increment.
//! @param dtMax: Maximum value for the time increment.
int XC::VariableTimeStepDirectIntegrationAnalysis::analyze(int numSteps, double dT, double dtMin, double dtMax, int Jd)
  {
    assert(solution_strategy);
    CommandEntity *old= solution_strategy->Owner();
    solution_strategy->set_owner(this);

    // get some pointers
    Domain *theDom = this->getDomainPtr();
    EquiSolnAlgo *theAlgo= this->getEquiSolutionAlgorithmPtr();
    TransientIntegrator *theIntegratr = getTransientIntegratorPtr();
    ConvergenceTest *theTest= this->getConvergenceTestPtr();

    // set some variables
    int result = 0;  
    double totalTimeIncr = numSteps * dT;
    double currentTimeIncr = 0.0;
    double currentDt = dT;
  
    // loop until analysis has performed the total time incr requested
    while(currentTimeIncr < totalTimeIncr)
      {

        if(this->checkDomainChange() != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed checkDomainChange\n";
            return -1;
          }

        //
        // do newStep(), solveCurrentStep() and commit() as in regular
        // DirectINtegrationAnalysis - difference is we do not return
        // if a failure - we stop the analysis & resize time step if failure
        //

        if(theIntegratr->newStep(currentDt) < 0)
          { result = -2; }
    
        if(result >= 0)
          {
            result = theAlgo->solveCurrentStep();
            if(result < 0) 
	      result = -3;
          }    

        if(result >= 0)
          {
            result = theIntegratr->commit();
            if(result < 0) 
	      result = -4;
          }

        // if the time step was successful increment delta T for the analysis
        // otherwise revert the XC::Domain to last committed state & see if can go on

        if(result >= 0) 
          currentTimeIncr += currentDt;
        else
          {
            // invoke the revertToLastCommit
            theDom->revertToLastCommit();	    
            theIntegratr->revertToLastStep();

            // if last dT was <= min specified the analysis FAILS - return FAILURE
            if(currentDt <= dtMin)
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; failed at time "
			  << theDom->getTimeTracker().getCurrentTime()
			  << std::endl;
                return result;
              }
            // if still here reset result for next loop
            result = 0;
          }
        // now we determine a new_ delta T for next loop
        currentDt = this->determineDt(currentDt, dtMin, dtMax, Jd, theTest);
      }
    solution_strategy->set_owner(old);
    return 0;
  }

//! @brief 
double XC::VariableTimeStepDirectIntegrationAnalysis::determineDt(double dT, double dtMin, double dtMax, int Jd, ConvergenceTest *theTest)
  {
    double newDt = dT;
    
    // get the number of trial steps in the last solveCurrentStep()
    double numLastIter = 1.0;
    if(theTest) numLastIter = theTest->getNumTests();
  
  
    // determine new_ dT based on last dT and Jd and #iter of last step
    double factor = Jd/numLastIter;
    newDt *= factor;
  
    // ensure: dtMin <~~ dT <= dtMax
    if(newDt < dtMin)
      newDt = dtMin - DBL_EPSILON;  // to ensure we get out of the analysis 
                               // loop if can't converge on next step
    else if(newDt > dtMax)
      newDt = dtMax;
    return newDt;
  }


