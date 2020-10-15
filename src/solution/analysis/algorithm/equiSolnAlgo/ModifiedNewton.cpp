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
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/ModifiedNewton.cpp,v $


// File: ~/OOP/analysis/algorithm/ModifiedNewton.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for
// ModifiedNewton. ModifiedNewton is a class which uses the
// Newton-Raphson solution algorithm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.
//
// What: "@(#)ModifiedNewton.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/ModifiedNewton.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/ID.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/Timer.h>
#include "solution/AnalysisAggregation.h"

//! @brief Constructor
//!
//! @param AnalysisAggregation: object that owns this one.
XC::ModifiedNewton::ModifiedNewton(AnalysisAggregation *owr,int theTangentToUse)
  :NewtonBased(owr,EquiALGORITHM_TAGS_ModifiedNewton,theTangentToUse) {}

//! @brief Virtual constructor.
XC::SolutionAlgorithm *XC::ModifiedNewton::getCopy(void) const
  { return new ModifiedNewton(*this); }

//! @brief Solves for the current analysis step.
//!
//! When invoked the object first sets itself as the EquiSolnAlgo object
//! that the ConvergenceTest is testing and then it performs the
//! modified Newton-Raphson iteration algorithm: 
//! \begin{tabbing}
//! while \= \+ while \= \kill
//! theTest-\f$>\f$start();
//! theIntegrator-\f$>\f$formTangent();
//! do \{ \+
//! theIntegrator-\f$>\f$formUnbalance();
//! theSOE-\f$>\f$solveX();
//! theIntegrator-\f$>\f$update(theSOE-\f$>\f$getX()); \-
//! \} while (theTest-\f$>\f$test() \f$==\f$ false)\-
//! \end{tabbing}
//! The method returns a 0 if successful, otherwise a negative number is
//! returned; a \f$-1\f$ if error during formTangent(), a \f$-2\f$ if
//! error during formUnbalance(), a \f$-3\f$ if error during {\em
//! solve()}, and a \f$-4\f$ if error during update().
//! If an error occurs in any of the above operations the method stops at
//! that routine, none of the subsequent operations are invoked. A \f$-5\f$ is
//! returned if any one of the links has not been setup.
int XC::ModifiedNewton::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protected in superclass
    AnalysisModel *theAnalysisModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIncIntegratorr= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((!theAnalysisModel) || (!theIncIntegratorr) || (!theSOE) || (!theTest))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING undefined model, integrator "
	          << "or system of equations.\n";
        return -5;
      }

    // we form the tangent
    //    Timer timer1;
    // timer1.start();
    
    if(theIncIntegratorr->formUnbalance() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; the Integrator failed in formUnbalance()\n";
        return -2;
      }


    if(theIncIntegratorr->formTangent(tangent) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; the Integrator failed in formTangent()\n";
        return -1;
      }


    // set itself as the ConvergenceTest objects EquiSolnAlgo
    theTest->set_owner(getAnalysisAggregation());
    if(theTest->start() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; the ConvergenceTest object failed in start()\n";
        return -3;
      }

    // repeat until convergence is obtained or reach max num iterations
    int result = -1;
    int count = 0;
    do
      {
        //Timer timer2;
        //timer2.start();
        if(theSOE->solve() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the LinearSysOfEqn failed in solve()\n";
            return -3;
          }
        //timer2.pause();
        //std::cerr << "TIMER::SOLVE()- " << timer2;
        if(theIncIntegratorr->update(theSOE->getX()) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Integrator failed in update()\n";
            return -4;
          }

        if(theIncIntegratorr->formUnbalance() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Integrator failed in formUnbalance()\n";
            return -2;
          }

        record(count++); //Calls record(...) method for all defined recorders.
        result = theTest->test();
      }
    while(result == -1);

    //timer1.pause();
    //std::cerr << "TIMER::solveCurrentStep - " << timer1;
    if(result == -2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "the ConvergenceTest object failed in test()." << std::endl
                  << "convergence test message: "
                  << theTest->getStatusMsg(1) << std::endl;

        return -3;
      }

    return result;
  }

//! @brief Sends the class name to the stream if \p flag equals \f$0\f$.
void XC::ModifiedNewton::Print(std::ostream &s, int flag) const
  {
    if(flag == 0)
      { s << getClassName(); }
  }
