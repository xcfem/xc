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
// File: ~/system_of_eqn/eigenSOE/EigenSOE.C
//
// Written: Jun Peng
// Created: Sat Feb. 6, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::EigenSOE.
// EigenSOE is a subclass of XC::SystemOfEqn.
// It has pure virtual functions which must be implemented in it's derived
// subclasses. To solve the genreal eigen value equations means that
// by the given K and M, find the corresponding eigen value and eigen
// vectors.
//
// This class is inheritanted from the base class of XC::SystemOfEqn
// which was created by fmk (Frank).


#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include <solution/analysis/model/AnalysisModel.h>

#include <solution/system_of_eqn/eigenSOE/BandArpackSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include <solution/system_of_eqn/eigenSOE/SymArpackSolver.h>
#include <solution/system_of_eqn/eigenSOE/SymBandEigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/FullGenEigenSolver.h>



#include "boost/numeric/ublas/io.hpp"
#include "boost/numeric/ublas/vector.hpp"
#include "utility/matrix/Matrix.h"

//! @brief Constructor.
XC::EigenSOE::EigenSOE(SoluMethod *owr,int classTag)
  :SystemOfEqn(owr,classTag), size(0), factored(false), theSolver(nullptr) {}

void XC::EigenSOE::libera(void)
  {
    if(theSolver)
      {
        delete theSolver;
        theSolver= nullptr;
      }
  }

void XC::EigenSOE::copia(const EigenSolver *newSolver)
  {
    if(newSolver)
      {
        libera();
        EigenSolver *tmp= newSolver->getCopy();
        if(tmp)
          setSolver(tmp);
        else
	  std::cerr << "Eigen::copia; no se pudo crear el solver."
                    << std::endl;
      }
  }

//! @brief Asigna el solver que se empleará para resolver el problema de eigenvalues.
bool XC::EigenSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    if(newSolver)
      {
        libera();
        theSolver= newSolver;
        theSolver->setEigenSOE(this);
        const int solverOK= theSolver->setSize();
        if(solverOK < 0)
          {
            std::cerr << "WARNING EigenSOE::setSolver :";
            std::cerr << " solver failed setSize() in constructor\n";
          }
        retval= true;
      }
    else
      std::cerr << "EigenSOE::setSolver; se pasó un puntero nulo." << std::endl;
    return retval;
  }

//! @brief Devuelve el número de ecuaciones.
int XC::EigenSOE::getNumEqn(void) const
  { return size; }

XC::EigenSolver &XC::EigenSOE::newSolver(const std::string &tipo)
  {
    if(tipo=="band_arpack_solver")
      setSolver(new BandArpackSolver());
    else if(tipo=="band_arpackpp_solver")
      setSolver(new BandArpackppSolver());
    else if(tipo=="sym_band_eigen_solver")
      setSolver(new SymBandEigenSolver());
    else if(tipo=="full_gen_eigen_solver")
      setSolver(new FullGenEigenSolver());
    else if(tipo=="sym_arpack_solver")
      setSolver(new SymArpackSolver());
    else
      std::cerr << "Solver of type: '"
                << tipo << "' unknown." << std::endl;
    assert(theSolver);
    return *theSolver;
  }

//! @brief Destructor.
XC::EigenSOE::~EigenSOE(void)
  { libera(); }

void XC::EigenSOE::resize_mass_matrix_if_needed(const size_t &sz)
  {
    if((massMatrix.size1() != sz) || (massMatrix.size2() != sz))
      massMatrix= sparse_matrix(sz,sz,0.0);
  }

//! @brief Resuelve el problema de eigenvalues con el número de modos being passed as parameter.
int XC::EigenSOE::solve(int numModes)
  { return (theSolver->solve(numModes)); }

//! @brief No hace nada.
int XC::EigenSOE::solve(void)
  {
    std::cerr << "ERROR XC::EigenSOE::solve(void) - need to specify numModes\n";
    return -1;
  }

//! @brief Devuelve un puntero al solver que se emplea para resolver el problema de eigenvalues.
XC::EigenSolver *XC::EigenSOE::getSolver(void)
  { return theSolver; }

//! @brief Anula la matriz M.
void XC::EigenSOE::zeroM(void)
  {
    massMatrix.clear();
    return;
  }

//! @brief Devuelve el autovector que corresponde al modo being passed as parameter.
const XC::Vector &XC::EigenSOE::getEigenvector(int mode) const
  { return theSolver->getEigenvector(mode); }

//! @brief Devuelve el autovector que corresponde al modo being passed as parameter
//! normalizado de modo que la componente máxima valga 1 (norma_infinito).
XC::Vector XC::EigenSOE::getNormalizedEigenvector(int mode) const
  { return theSolver->getNormalizedEigenvector(mode); }

//! @brief Devuelve una matriz con los eigenvectors calculados colocados
//! por columnas.
XC::Matrix XC::EigenSOE::getEigenvectors(void) const
  { return theSolver->getEigenvectors(); }

//! @brief Devuelve una matriz con los eigenvectors normalizados colocados
//! por columnas (norma_infinito).
XC::Matrix XC::EigenSOE::getNormalizedEigenvectors(void) const
  { return theSolver->getNormalizedEigenvectors(); }

//! @brief Devuelve el autovalor que corresponde al modo being passed as parameter.
const double &XC::EigenSOE::getEigenvalue(int mode) const
  { return theSolver->getEigenvalue(mode); }

//! @brief Devuelve la pulsación correspondiente al modo
//! being passed as parameter.
double XC::EigenSOE::getPulsacion(int mode) const
  { return sqrt(getEigenvalue(mode)); }

//! @brief Devuelve el período correspondiente al modo
//! being passed as parameter.
double XC::EigenSOE::getPeriodo(int mode) const
  { return 2.0*M_PI/getPulsacion(mode); }

//! @brief Devuelve la frecuencia correspondiente al modo
//! being passed as parameter.
double XC::EigenSOE::getFrecuencia(int mode) const
  { return 1./getPeriodo(mode); }

//! @brief Devuelve un vector con los eigenvalues calculados.
XC::Vector XC::EigenSOE::getEigenvalues(void) const
  { return theSolver->getEigenvalues(); }

//! @brief Devuelve un vector con las pulsaciones calculadas.
XC::Vector XC::EigenSOE::getPulsaciones(void) const
  { return theSolver->getPulsaciones(); }


//! @brief Devuelve un vector con las periodos calculados.
XC::Vector XC::EigenSOE::getPeriodos(void) const
  { return theSolver->getPeriodos(); }

//! @brief Devuelve un vector con las frecuencias calculadas.
XC::Vector XC::EigenSOE::getFrecuencias(void) const
  { return theSolver->getFrecuencias(); }

//! @brief Devuelve el número de eigenvalues que se han calculado.
const int &XC::EigenSOE::getNumModes(void) const
  { return theSolver->getNumModes(); }

//! @brief Devuelve el factor de participación modal
//! correspondiente al modo i.
double XC::EigenSOE::getModalParticipationFactor(int mode) const
  {
    const Vector ev= getEigenvector(mode);
    const size_t sz= ev.Size();
    if((massMatrix.size1()!=sz) || (massMatrix.size2()!=sz))
      std::cerr << "EigenSOE::getModalParticipationFactor; ERROR "
                << "el autovector es de dimensión " << sz
                << " y la matriz de masas de " << massMatrix.size1()
                << "x" << massMatrix.size2() << ".\n";
    boost::numeric::ublas::vector<double> fi_mode(sz);
    for(size_t i= 0;i<sz;i++)
      fi_mode(i)= ev(i);
    const boost::numeric::ublas::vector<double> J(sz,1.0);
    const double num= boost::numeric::ublas::inner_prod(fi_mode,prod(massMatrix,J));
    const boost::numeric::ublas::vector<double> tmp= prod(massMatrix,fi_mode);
    const double denom= boost::numeric::ublas::inner_prod(fi_mode,tmp);
    return num/denom;
  }

//! @brief Devuelve los factores de participación modal.
XC::Vector XC::EigenSOE::getModalParticipationFactors(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getModalParticipationFactor(i);
    return retval;
  }

//! @brief Devuelve el factor de distribución correspondiente al modo
//! being passed as parameter.
XC::Vector XC::EigenSOE::getDistributionFactor(int mode) const
  { return getModalParticipationFactor(mode)*getEigenvector(mode); }

//! @brief Devuelve una matriz con los factores de distribución
//! calculados colocados por columnas.
XC::Matrix XC::EigenSOE::getDistributionFactors(void) const
  {
    Matrix retval;
    const int nm= getNumModes();
    if(nm>0)
      {
        Vector distribFactor= getDistributionFactor(1);
        const int nFilas= distribFactor.Size();
        retval= Matrix(nFilas,nm);
        for(int i= 0;i<nFilas;i++)
          retval(i,0)= distribFactor(i);
        for(int j= 2;j<=nm;j++)
          {
            distribFactor= getDistributionFactor(j);
            for(int i= 0;i<nFilas;i++)
              retval(i,j-1)= distribFactor(i);
          }
      }
    return retval;
  }

//! @brief Devuelve la masa modal efectiva 
//! correspondiente al modo i.
double XC::EigenSOE::getEffectiveModalMass(int mode) const
  {
    const double tau= getModalParticipationFactor(mode);
    const Vector ev= getEigenvector(mode);
    const int sz= ev.Size();
    boost::numeric::ublas::vector<double> fi_mode(sz);
    for(int i= 0;i<sz;i++)
      fi_mode(i)= ev(i);
    boost::numeric::ublas::vector<double> J(sz,1.0);
    const double p= boost::numeric::ublas::inner_prod(fi_mode,prod(massMatrix,J));
    return tau*p;
  }

//! @brief Devuelve las masas modales efectivas.
XC::Vector XC::EigenSOE::getEffectiveModalMasses(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getEffectiveModalMass(i);
    return retval;
  }

//! @brief Devuelve la masa total del modelo.
double XC::EigenSOE::getTotalMass(void) const
  {
    const size_t sz= massMatrix.size1();
    const boost::numeric::ublas::vector<double> J(sz,1.0);
    const double retval= boost::numeric::ublas::inner_prod(J,prod(massMatrix,J));
    return retval;
  }

//! @brief Devuelve la fuerza estática equivalente para el modo
//! being passed as parameter.
XC::Vector XC::EigenSOE::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    Vector tmp= getDistributionFactor(mode);
    const int sz= tmp.Size();
    boost::numeric::ublas::vector<double> df(sz);
    for(int i= 0;i<sz;i++)
      df(i)= tmp(i);
    df= prod(massMatrix,df);
    for(int i= 0;i<sz;i++)
      tmp(i)= df(i)*accel_mode;
    return tmp;
  }

