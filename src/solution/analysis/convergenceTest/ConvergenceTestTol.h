// -*-c++-*-
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
//ConvergenceTestTol.h

#ifndef ConvergenceTestTol_h
#define ConvergenceTestTol_h

#include <solution/analysis/convergenceTest/ConvergenceTest.h>

namespace XC {

//!  @ingroup Analysis
//! 
//! @defgroup CTest convergence test.
//
//! @ingroup CTest
//
//! @brief Convergence test with tolerance threshold.
class ConvergenceTestTol: public ConvergenceTest
  {
  protected:
    double tol; //!< the tol on the energy used to test for convergence

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    ConvergenceTestTol(CommandEntity *owr,int classTag);	    	
    ConvergenceTestTol(CommandEntity *owr,int classTag,double tol, int maxNumIter, int printFlag, int normType,int sz_norms= 1);

    void setTolerance(double newTol);
    double getTolerance(void) const;
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    std::string getRatioMessage(const std::string &) const;
    std::string getDispIncrMessage(void) const;
    std::string getUnbalanceMessage(void) const;
    std::string getEnergyProductMessage(void) const;
  };
} // end of XC namespace

#endif
