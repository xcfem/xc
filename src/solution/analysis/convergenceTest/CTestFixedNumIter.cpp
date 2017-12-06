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

// $Revision: 1.1 $
// $Date: 2005/12/15 00:13:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestFixedNumIter.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A
//
// Purpose: This file contains the class implementation of XC::CTestFixedNumIter.
//
// What: "@(#) CTestFixedNumIter.cpp, revA"


#include <solution/analysis/convergenceTest/CTestFixedNumIter.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>


//! @brief Default constructor.
//!
//! @param owr: object that owns this one.
XC::CTestFixedNumIter::CTestFixedNumIter(EntCmd *owr)	    	
  : ConvergenceTest(owr,CONVERGENCE_TEST_CTestFixedNumIter)
  {}


//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param theTol: the tolerance used in test().
//! @param maxIter the max number of iterations to be performed.
//! @param printFlag: what, if anything, is printed on each test.
//! @param normType: type of norm to use (1-norm, 2-norm, p-norm, max-norm).
XC::CTestFixedNumIter::CTestFixedNumIter(EntCmd *owr,int maxIter, int printFlag, int normType)
  : ConvergenceTest(owr,CONVERGENCE_TEST_CTestFixedNumIter,maxIter,printFlag,normType,maxIter)
  {}

//! @brief Virtual constructor.
XC::ConvergenceTest* XC::CTestFixedNumIter::getCopy(void) const
  { return new CTestFixedNumIter(*this); }


//! @brief Returns a message showing the values of the principal parameters.
std::string XC::CTestFixedNumIter::getStatusMsg(const int &flag) const
  {
    std::string retval= getTestIterationMessage();
    retval+= getEnergyProductMessage();
    if(flag >= 4)
      {
        retval+= '\n'+getDeltaXRNormsMessage();
        retval+= '\n'+getDeltaXRMessage();
      }
    return retval;
  }

//! @brief Check for convergence.
int XC::CTestFixedNumIter::test(void)
  {
    // check to ensure the SOE has been set - this should not happen if the 
    // return from start() is checked
    if(!hasLinearSOE()) return -2;
    
    // check to ensure the algo does invoke start() - this is needed otherwise
    // may never get convergence later on in analysis!
    if(currentIter == 0)
      {
        std::cerr << "WARNING: CTestFixedNumIter::test() - start() was never invoked.\n";	
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
    if(currentIter == maxNumIter)
      {   
        // do some printing first
        if(printFlag != 0)
          {
            if(printFlag == 1 || printFlag == 4) 
              std::clog << std::endl;
            else if(printFlag == 2 || printFlag == 6)
              {
                std::clog << getTestIterationMessage();
                std::clog << " last EnergyIncr: " << calculatedEnergyProduct;
		calculatedNormX= getNormX(); //Update values.
		calculatedNormB= getNormB();
                std::clog << " (" << getDeltaXRNormsMessage() << ")\n";
              }
          }
        // return the number of times test has been called
        return currentIter;
      }  
    // algorithm not yet converged - increment counter and return -1
    else
      {
        currentIter++;    
        return -1;
      }
  }


//! @brief Display current energy product and tolerance.
std::string XC::CTestFixedNumIter::getEnergyProductMessage(void) const
  {
    std::ostringstream retval; 
    calculatedNormX= getNormX(); //Update values.
    calculatedNormB= getNormB();
    retval << " current EnergyIncr: " << calculatedEnergyProduct
	   << " (" << getDeltaXRNormsMessage() << ")";
    return retval.str();
  }
