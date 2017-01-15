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
  :Solver(classTag), numModes(nModes) {}

//! @brief Devuelve la pulsación correspondiente al modo
//! being passed as parameter.
double XC::EigenSolver::getPulsacion(int mode) const
  { return sqrt(getEigenvalue(mode)); }

//! @brief Devuelve el período correspondiente al modo
//! being passed as parameter.
double XC::EigenSolver::getPeriodo(int mode) const
  { return 2.0*M_PI/getPulsacion(mode); }

//! @brief Devuelve la frecuencia correspondiente al modo
//! being passed as parameter.
double XC::EigenSolver::getFrecuencia(int mode) const
  { return 1./getPeriodo(mode); }

//! @brief Devuelve un vector con los autovalores calculados.
XC::Vector XC::EigenSolver::getEigenvalues(void) const
  {
    Vector retval(numModes);
    for(int i= 1;i<=numModes;i++)
      retval[i-1]= getEigenvalue(i);
    return retval;
  }

//! @brief Devuelve un vector con las pulsaciones calculadas.
XC::Vector XC::EigenSolver::getPulsaciones(void) const
  {
    Vector retval= getEigenvalues();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= sqrt(retval(i));
    return retval;
  }

//! @brief Devuelve un vector con las periodos calculados.
XC::Vector XC::EigenSolver::getPeriodos(void) const
  {
    Vector retval= getPulsaciones();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 2.0*M_PI/retval(i);
    return retval;
  }

//! @brief Devuelve un vector con las frecuencias calculadas.
XC::Vector XC::EigenSolver::getFrecuencias(void) const
  {
    Vector retval= getPeriodos();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 1.0/retval(i);
    return retval;
  }

//! @brief Devuelve el autovector correspondiente al modo i
//! normalizado de modo que la componente máxima valga 1 (norma_infinito).
XC::Vector XC::EigenSolver::getNormalizedEigenvector(int mode) const
  { return normalize_inf(getEigenvector(mode)); }

//! @brief Devuelve una matriz con los autovectores calculados colocados
//! por columnas.
XC::Matrix XC::EigenSolver::getEigenvectors(void) const
  {
    const int nFilas= getSize();
    Matrix retval(nFilas,numModes);
    for(int j= 1;j<=numModes;j++)
      {
        const Vector &eigenVector= getEigenvector(j);
        for(int i= 0;i<nFilas;i++)
          retval(i,j-1)= eigenVector(i);
      }
    return retval;
  }

//! @brief Devuelve una matriz con los autovectores normalizados colocados
//! por columnas (norma_infinito).
XC::Matrix XC::EigenSolver::getNormalizedEigenvectors(void) const
  {
    const int nFilas= getSize();
    Matrix retval(nFilas,numModes);
    for(int j= 0;j<numModes;j++)
      {
        const Vector &eigenVector= getNormalizedEigenvector(j+1);
        for(int i= 0;i<nFilas;i++)
          retval(i,j)= eigenVector(i);
      }
    return retval;
  }

