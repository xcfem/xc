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
// $Date: 2006/01/10 18:13:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/CTestNormDispIncr.cpp,v $


#include <solution/analysis/convergenceTest/CTestNormDispIncr.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>


//! @brief Default constructor.
//!
//! @param owr: object that owns this one.
XC::CTestNormDispIncr::CTestNormDispIncr(EntCmd *owr)	    	
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestNormDispIncr,0.0,0,0,2,25)
  {}


//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param theTol: the tolerance used in test().
//! @param maxIter the max number of iterations to be performed.
//! @param printFlag: what, if anything, is printed on each test.
//! @param normType: type of norm to use (1-norm, 2-norm, p-norm, max-norm).
XC::CTestNormDispIncr::CTestNormDispIncr(EntCmd *owr,double theTol, int maxIter, int printFlag, int normType)
  : ConvergenceTestTol(owr,CONVERGENCE_TEST_CTestNormDispIncr,theTol,maxIter,printFlag,normType,maxIter)
  {}

//! @brief Virtual constructor.
XC::ConvergenceTest* XC::CTestNormDispIncr::getCopy(void) const
  { return new CTestNormDispIncr(*this); }

//! @brief Returns a message showing the values of the principal parameters.
std::string XC::CTestNormDispIncr::getStatusMsg(const int &flag) const
  {
    std::string retval= getTestIterationMessage();
    retval+= getDispIncrMessage();
    if(flag >= 4)
      retval+= '\n'+getDeltaXRNormsMessage() + '\n' + getDeltaXRMessage();
    return retval;
  }

//! @brief Returns {currentNumIter} if the two norm of the LinearSOE objects X
//! Vector is less than the tolerance \p tol.
//!
//! Returns {currentNumIter} if the two norm of the LinearSOE objects X
//! Vector is less than the tolerance \p tol. If no LinearSOE has been
//! set \f$-2\f$ is returned. If the \p currentNumIter \f$>=\f$ {\em
//! maxNumIter} an error message is printed and \f$-2\f$ is returned. If none
//! of these conditions is met, the \p currentnumIter is incremented
//! and \f$-1\f$ is returned. If the print flag is \f$0\f$ nothing is printed to
//! cerr during the method, if \f$1\f$ the current iteration and norm are
//! printed to cerr, and if \f$2\f$ the norm and number of iterations to
//! convergence are printed to cerr. 
int XC::CTestNormDispIncr::test(void)
  {
    // check to ensure the SOE has been set - this should not happen if the 
    // return from start() is checked
    if(!hasLinearSOE()) return -2;
    
    // check to ensure the algo does invoke start() - this is needed otherwise
    // may never get convergence later on in analysis!
    if(currentIter == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING: start() was never invoked.\n";	
        return -2;
      }
    
    // get the X vector & determine it's norm & save the value in norms vector
    calculatedNormX= getNormX();
    if(currentIter <= maxNumIter) 
      norms(currentIter-1)= calculatedNormX;
    
    // print the data if required
    if(printFlag)
      std::clog << getStatusMsg(printFlag);

    //
    // check if the algorithm converged
    //
    
    // if converged - print & return ok
    if(calculatedNormX <= tol)
      {   
        // do some printing first
        if(printFlag != 0)
          {
            if(printFlag == 1 || printFlag == 4) 
              std::clog << std::endl;
            else if(printFlag == 2 || printFlag == 6)
              std::clog << getTestIterationMessage() << getDispIncrMessage();
          }
        // return the number of times test has been called
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - but RETURN OK
    else if((printFlag == 5 || printFlag == 6) && currentIter >= maxNumIter)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: failed to converge but going on - "
		  << getDispIncrMessage() << std::endl;
        return currentIter;
      }
    // algo failed to converged after specified number of iterations - return FAILURE -2
    else if(currentIter >= maxNumIter)
      { // failes to converge
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



