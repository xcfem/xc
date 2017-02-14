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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymSparseLinSOE.h
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseLinSOE.h. It stores the sparse matrix A in a fashion
// that only store the non zero entries.
//
// What: "@(#) SymSparseLinSOE.h, revA"
//
// Almost all the information (Matrix A and Vector B) is stored as 
// global variables in the file "symbolic.h".


#ifndef SymSparseLinSOE_h
#define SymSparseLinSOE_h

#include <solution/system_of_eqn/linearSOE/SparseSOEBase.h>

extern "C" {
   #include <solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h>
}

namespace XC {
class SymSparseLinSolver;

//! @ingroup SOE
//
//! @brief Sparse symmetric matrix systems of equations.
class SymSparseLinSOE : public SparseSOEBase
  {
  private:
    ID colA, rowStartA;  //These are (ADJNCY, XADJ) pair.

    int      LSPARSE;
    int      nblks;
    int      *xblk,  *invp;
    double   *diag, **penv;
    int      *rowblks;
    OFFDBLK  **begblk;
    OFFDBLK  *first;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    SymSparseLinSOE(SoluMethod *,int lSparse= 0);
    SystemOfEqn *getCopy(void) const;
  public:
    ~SymSparseLinSOE(void);

    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    int addB(const Vector &, const ID &,const double &fact= 1.0);    
    
    void zeroA(void);

    int setSymSparseLinSolver(SymSparseLinSolver *);    

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class SymSparseLinSolver;
  };
inline SystemOfEqn *SymSparseLinSOE::getCopy(void) const
  { return new SymSparseLinSOE(*this); }
} // end of XC namespace

#endif

