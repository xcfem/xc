// -*-c++-*-
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
//ConvergenceTestNorm.h

#ifndef ConvergenceTestNorm_h
#define ConvergenceTestNorm_h

#include <solution/analysis/convergenceTest/ConvergenceTestTol.h>

namespace XC {

//! @ingroup CTest
//
//! @brief Convergence test that uses vector norm
//! value (solution vector,...).
class ConvergenceTestNorm: public ConvergenceTestTol
  {
  protected:
    double norm0; //!< norm at first iteration of each step

  public:
    // constructors
    ConvergenceTestNorm(CommandEntity *owr,int classTag);	    	
    ConvergenceTestNorm(CommandEntity *owr,int cTag,double tl, int maxNIter, int printFlag, int normType, int sz);

    virtual int start(void);  
  };
} // end of XC namespace

#endif
