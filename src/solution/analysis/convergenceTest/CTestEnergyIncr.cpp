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

// $Revision: 1.6 $
// $Date: 2005/12/15 00:19:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestEnergyIncr.cpp,v $


// File: ~/convergenceTest/CTestEnergyIncr.C
//
// Written: fmk 
// Date: 09/98
// Revised:
//
// Purpose: This file contains the class implementation for XC::CTestEnergyIncr.
// A CTestEnergyIncr object tests for convergence using the energy increment,
// which is 0.5 times the absolute value of the product of the rhs and 
// the solution vector of the LinearSOE.

#include <solution/analysis/convergenceTest/CTestEnergyIncr.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

XC::CTestEnergyIncr::CTestEnergyIncr(EntCmd *owr)	    	
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestEnergyIncr) {}

XC::CTestEnergyIncr::CTestEnergyIncr(EntCmd *owr,double theTol, int maxIter, int printIt, int normType)
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestEnergyIncr,theTol,maxIter,printIt,normType,maxIter) {}

XC::ConvergenceTest* XC::CTestEnergyIncr::getCopy(void) const
  { return new CTestEnergyIncr(*this); }

//! @brief Returns a message showing the values of the principal parameters.
std::string XC::CTestEnergyIncr::getStatusMsg(const int &flag) const
  {
    std::string retval= getTestIterationMessage();
    retval+= getEnergyProductMessage();
    if(flag >= 4)
      {
        retval+= '\n'+getDeltaXRNormsMessage() + '\n'+getDeltaXRMessage();
      }
    return retval;
  }

//! @brief Comprueba si se ha producido la convergencia.
int XC::CTestEnergyIncr::test(void)
  {
    // check to ensure the SOE has been set - this should not happen if the 
    // return from start() is checked
    if(!hasLinearSOE()) return -2;
    
    // check to ensure the algo does invoke start() - this is needed otherwise
    // may never get convergence later on in analysis!
    if(currentIter == 0)
      {
        std::cerr << "WARNING: XC::CTestEnergyIncr::test() - start() was never invoked.\n";	
        return -2;
      }
    
    // determine the energy & save value in norms vector
    calculatedEnergyProduct= getEnergyProduct();
    
    if(currentIter <= maxNumIter) 
      norms(currentIter-1)= calculatedEnergyProduct;
    
    // print the data if required
    if(printFlag)
      std::clog << getStatusMsg(printFlag);
    
    //
    // check if the algorithm converged
    //
    
    // if converged - print & return ok
    if(calculatedEnergyProduct <= tol)
      {
        // do some printing first
        if(printFlag != 0)
          {
            if(printFlag == 1 || printFlag == 4) 
              std::clog << std::endl;
            else if(printFlag == 2 || printFlag == 6)
              {
                std::clog << getTestIterationMessage();
                std::clog << getEnergyProductMessage();
              }
          }
        // return the number of times test has been called - SUCCESSFULL
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - but RETURN OK
    else if((printFlag == 5 || printFlag == 6) && currentIter >= maxNumIter)
      {
        std::cerr << "WARNING: CTestEnergyIncr::test() - failed to converge but goin on -";
        std::cerr << getEnergyProductMessage();
        calculatedNormX= getNormX(); //Update values.
	calculatedNormB= getNormB();
        std::cerr << getDeltaXRNormsMessage() << std::endl;
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - return FAILURE -2
    else if(currentIter >= maxNumIter)
      { // >= in case algorithm does not check
        std::cerr << getFailedToConvergeMessage();
        currentIter++;
        return -2;
      } 
    // algorithm not yet converged - increment counter and return -1
    else
      {
        currentIter++;    
        return -1;
      }
  }




