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
// Description: This file contains the class definition of XC::EigenSOE.
// EigenSOE is a subclass of Solver.
// This is an abstract base class and thus no objects of it's type
// can be instantiated. Instances of XC::EigenSolver are used to solve
// a XC::EigenSOE. (perform eigen analysis)
//
// This class is inheritanted from the base class of XC::Solver
// which was created by fmk (Frank).


#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>


#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

//! @brief Constructor.
XC::EigenSolver::EigenSolver(const int &classTag,const int &nModes)
  :Solver(classTag), numModes(nModes), which("LM") {}

int XC::EigenSolver::solve(int nEigen)
  {
    // check for quick return
    if(nEigen < 1)
      {
        numModes = 0;
        return 0;
      }
    else
      numModes= nEigen;
    return this->solve();
  }

//! @brief Return the angular frequency for the i-th mode.
double XC::EigenSolver::getAngularFrequency(int i) const
  { return sqrt(getEigenvalue(i)); }

//! @brief Returns the period for the i-th mode.
double XC::EigenSolver::getPeriod(int i) const
  { return 2.0*M_PI/getAngularFrequency(i); }

//! @brief Return the frequency for the i-th mode.
double XC::EigenSolver::getFrequency(int i) const
  { return 1./getPeriod(i); }

//! @brief Returns a vector con los eigenvalues calculados.
XC::Vector XC::EigenSolver::getEigenvalues(void) const
  {
    Vector retval(numModes);
    for(int i= 1;i<=numModes;i++)
      retval[i-1]= getEigenvalue(i);
    return retval;
  }

//! @brief Returns a vector with the computed angular frequencies
//! for each mode.
XC::Vector XC::EigenSolver::getAngularFrequencies(void) const
  {
    Vector retval= getEigenvalues();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= sqrt(retval(i));
    return retval;
  }

//! @brief Returns a vector with the computed periods
//! for each mode.
XC::Vector XC::EigenSolver::getPeriods(void) const
  {
    Vector retval= getAngularFrequencies();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 2.0*M_PI/retval(i);
    return retval;
  }

//! @brief Returns a vector with the computed frequencies
//! for each mode.
XC::Vector XC::EigenSolver::getFrequencies(void) const
  {
    Vector retval= getPeriods();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 1.0/retval(i);
    return retval;
  }

//! @brief Returns the autovector of the i-th mode
//! normalized so the maximal component is 1 (infinity norm).
XC::Vector XC::EigenSolver::getNormalizedEigenvector(int i) const
  { return normalize_inf(getEigenvector(i)); }

//! @brief Returns a matrix of eigenvectors disposed in columns.
XC::Matrix XC::EigenSolver::getEigenvectors(void) const
  {
    const int n_rows= getSize();
    Matrix retval(n_rows,numModes);
    for(int j= 1;j<=numModes;j++)
      {
        const Vector &eigenVector= getEigenvector(j);
        for(int i= 0;i<n_rows;i++)
          retval(i,j-1)= eigenVector(i);
      }
    return retval;
  }

//! @brief Returns a matrix with the normalized eigenvectors disposed
//! in columns (infinity norm).
XC::Matrix XC::EigenSolver::getNormalizedEigenvectors(void) const
  {
    const int n_rows= getSize();
    Matrix retval(n_rows,numModes);
    for(int j= 0;j<numModes;j++)
      {
        const Vector &eigenVector= getNormalizedEigenvector(j+1);
        for(int i= 0;i<n_rows;i++)
          retval(i,j)= eigenVector(i);
      }
    return retval;
  }

