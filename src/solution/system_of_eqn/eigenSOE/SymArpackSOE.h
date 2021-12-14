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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymArpackSOE.h
//
// Written: Jun Peng
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymArpackSOE.h. It stores the sparse matrix A in a fashion
// that only store the none zero.
//
// What: "@(#) SymArpackSOE.h, revA"
//
// Almost all the information (Matrix A and Vector B) is stored as 
// global variables in the file "symbolic.h".


#ifndef SymArpackSOE_h
#define SymArpackSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>

extern "C" {
   #include <solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h>
}



namespace XC {
class SymArpackSolver;

//! @ingroup EigenSOE
//
//! @brief Arpack system of equations for symmetric matrices.
class SymArpackSOE: public ArpackSOE
  {
  private:
    int nnz; // number of non-zeros in A
    ID colA;
    ID rowStartA;  //These are (ADJNCY, XADJ) pair.

    int      nblks;
    int      *xblk,  *invp;
    double   *diag, **penv;
    int      *rowblks;
    OFFDBLK  **begblk;
    OFFDBLK  *first;
  protected:
    bool setSolver(EigenSolver *);


    friend class SolutionStrategy;
    friend class FEM_ObjectBroker;
    SymArpackSOE(SolutionStrategy *,double shift = 0.0);        
    SystemOfEqn *getCopy(void) const;
  public:
    virtual ~SymArpackSOE(void);

    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    
      
    virtual void zeroA(void);
    virtual void zeroM(void);
    virtual void identityM(void);
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    friend class SymArpackSolver;
  };
inline SystemOfEqn *SymArpackSOE::getCopy(void) const
  { return new SymArpackSOE(*this); }
} // end of XC namespace

#endif

