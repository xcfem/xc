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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymArpackSolver.h
//
// Written: Jun Peng
// Created: 2/1999
// Revision: A
//
// Description: This file contains the class definition for 
// SymArpackSolver. It solves the SymArpackSOE object by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymArpackSolver.h, revA"

#ifndef SymArpackSolver_h
#define SymArpackSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class SymArpackSOE;

//! @ingroup EigenSolver
//
//! @brief <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack</a> based symmetric matrix eigenvalue SOE solver.
class SymArpackSolver : public EigenSolver
  {
  private:
    SymArpackSOE *theSOE;
    bool factored;

    Vector value;
    Vector vector;
    mutable Vector eigenV;
    
    void myMv(int n, double *v, double *result);
    void myCopy(int n, double *v, double *result);
    int getNCV(int n, int nev);

    friend class EigenSOE;
    SymArpackSolver(int numE = 0);     
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:
    virtual int solve(void);
    virtual int solve(int numModes) {return this->solve();};    
    virtual int setSize(void);
    const int &getSize(void) const;

    virtual bool setEigenSOE(SymArpackSOE &theSOE); 
	
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *SymArpackSolver::getCopy(void) const
   { return new SymArpackSolver(*this); }
} // end of XC namespace

#endif

