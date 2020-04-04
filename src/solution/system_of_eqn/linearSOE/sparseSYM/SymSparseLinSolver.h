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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymSparseLinSolver.h
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseinSolver. It solves the SymSparseLinSOEobject by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymSparseLinSolver.h, revA"


#ifndef SymSparseLinSolver_h
#define SymSparseLinSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>


namespace XC {
class SymSparseLinSOE;

//! @ingroup Solver
//
//! @brief Solver for symmetric sparse linear SOE.
class SymSparseLinSolver: public LinearSOESolver
  {
  private:
    SymSparseLinSOE *theSOE;

    friend class LinearSOE;
    SymSparseLinSolver();     
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:

    int solve(void);
    int setSize(void);

    bool setLinearSOE(SymSparseLinSOE &theSOE); 
	
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
  };

inline LinearSOESolver *SymSparseLinSolver::getCopy(void) const
   { return new SymSparseLinSolver(*this); }
} // end of XC namespace

#endif

