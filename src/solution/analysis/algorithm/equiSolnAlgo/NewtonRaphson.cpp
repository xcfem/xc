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

// $Revision: 1.9 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/NewtonRaphson.cpp,v $


// File: ~/OOP/analysis/algorithm/NewtonRaphson.C
//
// Written: fmk
// Created: Sun Sept 15 15:06:47: 1996
// Revision: A
//

// Description: This file contains the class definition for
// NewtonRaphson. NewtonRaphson is a class which uses the
// Newton-Raphson solution algorithm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.
//
// What: "@(#)NewtonRaphson.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonRaphson.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/ID.h>
#include "solution/AnalysisAggregation.h"

//! @brief Constructor
//!
//! @param AnalysisAggregation: object the object which is used to access the
//! object that compound the analysis and that, at the end of each iteration,
//! are used to determine if convergence has been obtained.
XC::NewtonRaphson::NewtonRaphson(AnalysisAggregation *owr,int theTangentToUse)
  :NewtonBased(owr,EquiALGORITHM_TAGS_NewtonRaphson,theTangentToUse) {}

//! @brief Performs the Newton-Raphson iteration algorithm.
//!
//! When invoked the object first sets itself as the EquiSolnAlgo object
//! that the ConvergenceTest is testing and then it performs the
//! Newton-Raphson iteration algorithm: 
//! \begin{tabbing}
//! while \= \+ while \= \kill
//! theTest-\f$>\f$start()
//! theIntegrator-\f$>\f$formUnbalance();
//! do \{ \+
//! theIntegrator-\f$>\f$formTangent();
//! theSOE-\f$>\f$solveX();
//! theIntegrator-\f$>\f$update(theSOE-\f$>\f$getX());
//! theIntegrator-\f$>\f$formUnbalance(); \-
//! \} while (theTest-\f$>\f$test() \f$==\f$ false)\- 
//! \end{tabbing}
//!
//! The method returns a 0 if successful, otherwise a negative number is
//! returned; a \f$-1\f$ if error during formTangent(), a \f$-2\f$ if
//! error during formUnbalance(), a \f$-3\f$ if error during {\em
//! solve()}, and a \f$-4\f$ if error during update().
//! If an error occurs in any of the above operations the method stops at
//! that routine, none of the subsequent operations are invoked. A \f$-5\f$ is
//! returned if any one of the links has not been setup. NOTE it is up to
//! ConvergenceTest to ensure an infinite loop situation is not encountered.
int XC::NewtonRaphson::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnaModel==nullptr) || (theIntegrator==nullptr) || (theSOE==nullptr) || (theTest==nullptr))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; - setLinks() has"
                  << "undefined model, integrator or system of equations.\n";
        return -5;
      }

    if(theIntegrator->formUnbalance() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; the Integrator failed in formUnbalance().\n";
        return -2;
      }

    // set itself as the ConvergenceTest objects EquiSolnAlgo
    theTest->set_owner(getAnalysisAggregation());
    if(theTest->start() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";the ConvergenceTest object failed in start()\n";
        return -3;
      }

    int result = -1;
    int count = 0;
    do
      {

        if(tangent == INITIAL_THEN_CURRENT_TANGENT)
          {
            if(count == 0)
              {
                if(theIntegrator->formTangent(INITIAL_TANGENT) < 0)
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; the Integrator failed in formTangent()\n";
                    return -1;
                  }
              }
            else
              {
                if(theIntegrator->formTangent(CURRENT_TANGENT) < 0)
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; the Integrator failed in formTangent()\n";
                    return -1;
                  }
              }
          }
        else
          {
            if(theIntegrator->formTangent(tangent) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; the Integrator failed in formTangent()\n";
                return -1;
              }
          }
        if(theSOE->solve() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the LinearSysOfEqn failed in solve()\n";
            return -3;
          }
        if(theIntegrator->update(theSOE->getX()) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Integrator failed in update()\n";
            return -4;
          }

        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the Integrator failed in formUnbalance()\n";
            return -2;
          }

        result = theTest->test();
        this->record(count++); //Call the record(...) method of all recorders.
      }
    while(result == -1);

    if(result == -2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; the ConvergenceTest object failed in test()\n"
                  << "convergence test message: "
                  << theTest->getStatusMsg(1) << std::endl;
        return -3;
      }

    // note - if positive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

//! Sends the string 'NewtonRaphson' to the stream if \p flag equals \f$0\f$.
void XC::NewtonRaphson::Print(std::ostream &s, int flag) const
  {
    if(flag == 0)
      s << getClassName() << std::endl;
  }









