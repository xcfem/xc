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
//ConvergenceTestNorm.cc

#include <solution/analysis/convergenceTest/ConvergenceTestNorm.h>

//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: identifier of the object class.
XC::ConvergenceTestNorm::ConvergenceTestNorm(CommandEntity *owr,int classTag)	    	
  : ConvergenceTestTol(owr,classTag), norm0(0.0) {}


//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: identifier of the object class.
//! @param theTol: the tolerance used in test().
//! @param maxIter the max number of iterations to be performed.
//! @param printFlag: what, if anything, is printed on each test.
//! @param normType: type of norm to use (1-norm, 2-norm, p-norm, max-norm).
//! @param sz_norms: size of the vector that contains computed norms.
XC::ConvergenceTestNorm::ConvergenceTestNorm(CommandEntity *owr,int classTag,double theTol, int maxIter, int printFlag, int normType, int sz_norms)
  : ConvergenceTestTol(owr,classTag,theTol,maxIter,printFlag,normType,sz_norms), norm0(0.0) {}

//! Sets an integer indicating the current number of iterations, {\em
//! currentNumIter} to \f$1\f$. Returns \f$0\f$ if successful, an error message
//! and \f$-1\f$ are returned if no LinearSOE object has been set. 
int XC::ConvergenceTestNorm::start(void)
  {
    int retval= ConvergenceTest::start();
    norm0 = 0.0;
    return retval;
  }

