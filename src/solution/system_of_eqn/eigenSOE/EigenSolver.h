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
// File: ~/system_of_eqn/eigenSOE/EigenSolver.C
//
// Written: Jun Peng
// Created: Sat Feb. 6, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenSOE.
// EigenSOE is a subclass of Solver.
// This is an abstract base class and thus no objects of it's type
// can be instantiated. Instances of EigenSolver are used to solve
// a EigenSOE. (perform eigen analysis)
//
// This class is inheritanted from the base class of Solver
// which was created by fmk (Frank).


#ifndef EigenSolver_h
#define EigenSolver_h

#include <solution/system_of_eqn/Solver.h>

namespace XC {
class EigenSOE;
class Vector;

//!  @ingroup Solver
//! 
//! \defgroup EigenSolver Solvers for eigenvalue problems.
//
//! @ingroup EigenSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de problemas de eigenvalues.
class EigenSolver : public Solver
  {
  protected:
    int numModes; //! @brief Número de eigenvalues a obtener.

    friend class EigenSOE;
    virtual EigenSolver *getCopy(void) const= 0;
    virtual bool setEigenSOE(EigenSOE *theSOE) = 0;
    EigenSolver(const int &classTag, const int &nModes= 0);
  public:
    virtual ~EigenSolver(void) 
      {}

    virtual int solve(void) =0;
    virtual int solve(int numModes) =0;

    const int &getNumModes(void) const
      { return numModes; }
    virtual const Vector &getEigenvector(int mode) const = 0;
    Vector getNormalizedEigenvector(int mode) const;
    Matrix getEigenvectors(void) const;
    Matrix getNormalizedEigenvectors(void) const;
    virtual const double &getEigenvalue(int mode) const= 0;
    double getPulsacion(int mode) const;
    double getPeriodo(int mode) const;
    double getFrecuencia(int mode) const;
    Vector getEigenvalues(void) const;
    Vector getPulsaciones(void) const;
    Vector getPeriodos(void) const;
    Vector getFrecuencias(void) const;

    virtual int setSize(void)= 0;
    virtual const int &getSize(void) const= 0;
  };
} // end of XC namespace

#endif
