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
// File: ~/system_of_eqn/eigenSOE/ArpackSolver.h
//
// Written: Jun Peng
// Created: Feb. 11, 1999
// Revision: A
//
// Description: This file contains the class definition for 
// ArpackSolver. It solves the ArpackSOE object by calling
// Arpack routines.


#ifndef ArpackSolver_h
#define ArpackSolver_h

#include "solution/system_of_eqn/eigenSOE/EigenSolver.h"
#include "utility/matrix/Vector.h"

namespace XC {

  class LinearSOE;
  class ArpackSOE;

//! @ingroup EigenSolver
//
//! @brief Arpack solver for banded matrices.
//! The ARnoldi PACKage, is a numerical software
//! library written in FORTRAN 77 for solving large scale eigenvalue problems.
class ArpackSolver: public EigenSolver
  {
  private:
    LinearSOE *theSOE;
    ArpackSOE *theArpackSOE;
    int numModesMax;
    int numMode;
    int size;
    std::vector<double> eigenvalues;
    std::vector<double> eigenvectors;
    mutable Vector eigenV;

    double shift;
    int ncv;
    std::vector<double> v;
    std::vector<double> workl;
    std::vector<double> workd;
    std::vector<double> resid;
    int iparam[11];
    int ipntr[11];
    std::vector<int> select;
    
    void myMv(int n, double *v, double *result);
    void myCopy(int n, double *v, double *result);
    int getNCV(int n, int nev);
    void seupd_error_msg(const int &);
    void saupd_error_msg(const int &);

  protected:
    friend class EigenSOE;
    ArpackSolver(const int &nModes= 1);   
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:

    virtual int solve(void);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(ArpackSOE &theSOE);
    
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
    
    double getRCond(const char &);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

inline EigenSolver *ArpackSolver::getCopy(void) const
   { return new ArpackSolver(*this); }
} // end of XC namespace

#endif


