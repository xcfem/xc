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
//! \defgroup EigenSolver Solución de problemas de autovalores.
//
//! @ingroup EigenSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de problemas de autovalores.
class EigenSolver : public Solver
  {
  protected:
    int numModes; //! @brief Número de autovalores a obtener.

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
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
