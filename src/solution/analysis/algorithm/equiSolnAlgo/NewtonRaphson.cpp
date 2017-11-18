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
// Newton-Raphson solution algorihm
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
#include "solution/SoluMethod.h"

//! @brief Constructor
XC::NewtonRaphson::NewtonRaphson(SoluMethod *owr,int theTangentToUse)
  :NewtonBased(owr,EquiALGORITHM_TAGS_NewtonRaphson,theTangentToUse) {}


int XC::NewtonRaphson::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnaModel == 0) || (theIntegrator == 0) || (theSOE == 0) || (theTest == 0))
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
    theTest->set_owner(getSoluMethod());
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
        this->record(count++); //Llama al método record(...) de todos los recorders definidos.
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

    // note - if postive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

void XC::NewtonRaphson::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      s << "NewtonRaphson" << std::endl;
  }









