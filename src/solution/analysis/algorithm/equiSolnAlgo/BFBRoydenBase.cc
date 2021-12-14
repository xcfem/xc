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
//BFBRoydenBase.cc
                                                                        
#include <solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>

void XC::BFBRoydenBase::free_mem(void)
  {
    if(localTest)
      {
        delete localTest;
        localTest= nullptr;
      }
  }

void XC::BFBRoydenBase::alloc(const ConvergenceTest *theT)
  {
    free_mem();
    localTest= theT->getCopy(numberLoops);
    if(!localTest)
      std::cerr << "BFBRoydenBase::alloc() - could not get a copy\n";
  }

//! @brief Constructor
XC::BFBRoydenBase::BFBRoydenBase(SolutionStrategy *owr,int classTag,int theTangentToUse, int n)
  :EquiSolnConvAlgo(owr,classTag,nullptr), localTest(nullptr) , tangent(theTangentToUse), numberLoops(n),s(n+3),z(n+3)
  {}

//! @brief Constructor
XC::BFBRoydenBase::BFBRoydenBase(SolutionStrategy *owr,int classTag,ConvergenceTest &theT, int theTangentToUse, int n)
  :EquiSolnConvAlgo(owr,classTag,&theT), localTest(nullptr), tangent(theTangentToUse), numberLoops(n),s(n+3),z(n+3)
  {
    alloc(theTest);
  }

//! @brief Destructor
XC::BFBRoydenBase::~BFBRoydenBase(void)
  { free_mem(); }

//! @brief Sets convergence test to use with the algorithm.
int XC::BFBRoydenBase::setConvergenceTest(ConvergenceTest *nwTest)
  {
    theTest= nwTest;
    alloc(nwTest);
    return 0;
  }
