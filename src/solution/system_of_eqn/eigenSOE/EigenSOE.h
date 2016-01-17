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
// File: ~/system_of_eqn/eigenSOE/EigenSOE.h
//
// Written: Jun Peng
// Created: Sat Feb. 6, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenSOE.
// EigenSOE is a subclass of SystemOfEqn.
// It has pure virtual functions which must be implemented in it's derived
// subclasses. To solve the general eigen value equations means that
// by the given K and M, find the corresponding eigen value and eigen
// vectors.
//
// This class is inheritanted from the base class of SystemOfEqn
// which was created by fmk (Frank).


#ifndef EigenSOE_h
#define EigenSOE_h

#include <solution/system_of_eqn/SystemOfEqn.h>
#include "/usr/include/boost/numeric/ublas/matrix_sparse.hpp"

namespace XC {
class EigenSolver;
class Graph;
class Matrix;
class Vector;
class ID;

//!  @ingroup SOE
//! 
//! @defgroup EigenSOE Sistemas de ecuaciones para problemas de autovalores.
//
//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de autovalores.
class EigenSOE : public SystemOfEqn
  {
  public:
    typedef boost::numeric::ublas::mapped_matrix<double> sparse_matrix;
  protected:
    int size; //!< order of A
    bool factored;
    sparse_matrix massMatrix; //!< Matriz de masas (se usa en getModalParticipationFactor).
    EigenSolver *theSolver;

    void libera(void);
    void copia(const EigenSolver *);
    virtual bool setSolver(EigenSolver *);
    void resize_mass_matrix_if_needed(const size_t &);

    EigenSOE(SoluMethod *,int classTag);
  public:
    virtual ~EigenSOE(void);
     
    int getNumEqn(void) const;

    virtual int solve(int numModes);
    virtual int solve(void);
     
    // pure virtual functions
    virtual int addA(const Matrix &, const ID &, double fact = 1.0) = 0;
    virtual int addM(const Matrix &, const ID &, double fact = 1.0) = 0;

    virtual int setSize(Graph &theGraph) = 0;
    virtual void zeroA(void) = 0;
    virtual void zeroM(void);

    const int &getNumModes(void) const;

    //Autovectores
    virtual const Vector &getEigenvector(int mode) const;
    Vector getNormalizedEigenvector(int mode) const;
    Matrix getEigenvectors(void) const;
    Matrix getNormalizedEigenvectors(void) const;

    //Autovalores
    virtual const double &getEigenvalue(int mode) const;
    double getPulsacion(int mode) const;
    double getPeriodo(int mode) const;
    double getFrecuencia(int mode) const;
    Vector getEigenvalues(void) const;
    Vector getPulsaciones(void) const;
    Vector getPeriodos(void) const;
    Vector getFrecuencias(void) const;

    //Factores de participación modal
    virtual double getModalParticipationFactor(int mode) const;
    Vector getModalParticipationFactors(void) const;

    //Factores de distribución
    Vector getDistributionFactor(int mode) const;
    Matrix getDistributionFactors(void) const;

    //Masas modales efectivas.
    double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;
    double getTotalMass(void) const;

    //Fuerza estática equivalente.
    Vector getEquivalentStaticLoad(int mode,const double &) const;

    EigenSolver *getSolver(void);
    EigenSolver &newSolver(const std::string &);
  };
} // end of XC namespace

#endif



