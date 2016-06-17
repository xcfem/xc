//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

