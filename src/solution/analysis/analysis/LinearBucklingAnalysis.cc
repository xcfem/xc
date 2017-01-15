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
//LinearBucklingAnalysis.cpp

#include "LinearBucklingAnalysis.h"
#include "solution/analysis/analysis/LinearBucklingEigenAnalysis.h"
#include "solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h"
#include "solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.h"
#include "solution/analysis/model/AnalysisModel.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "solution/system_of_eqn/eigenSOE/ArpackSOE.h"
#include "solution/system_of_eqn/eigenSOE/BandArpackppSOE.h"
#include "solution/analysis/numberer/DOF_Numberer.h"
#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/convergenceTest/ConvergenceTest.h"
#include "solution/analysis/integrator/StaticIntegrator.h"
#include "solution/analysis/integrator/eigen/LinearBucklingIntegrator.h"
#include "domain/domain/Domain.h"
#include "solution/SoluMethod.h"
#include "solution/ProcSolu.h"


//! @brief Constructor.
XC::LinearBucklingAnalysis::LinearBucklingAnalysis(SoluMethod *metodo,SoluMethod *esolu)
  :StaticAnalysis(metodo), eigen_solu(esolu), linearBucklingEigenAnalysis(esolu),
   numModes(0),linear_buckling_analysis_step(0) {}

//! @brief Borra todos los miembros del objeto (Manejador coacciones, modelo de análisis,...).
void XC::LinearBucklingAnalysis::clearAll(void)
  {
    // invoke the destructor on all the objects in the aggregation
    StaticAnalysis::clearAll();
    linearBucklingEigenAnalysis.clearAll();
  }

//! @brief Ejecuta el análisis.
int XC::LinearBucklingAnalysis::analyze(int numSteps)
  {
    assert(metodo_solu);
    EntCmd *old= metodo_solu->Owner();
    metodo_solu->set_owner(this);
    assert(eigen_solu);
    EntCmd *oldE= eigen_solu->Owner();
    eigen_solu->set_owner(this);
    linearBucklingEigenAnalysis.set_owner(getProcSolu());

    int result = 0;

    for(int i=0; i<numSteps; i++)
      {
        if(i == linear_buckling_analysis_step)
          linearBucklingEigenAnalysis.setupPreviousStep(); //Prepara el linear buckling Analysis.

        if(i == (linear_buckling_analysis_step+1))
          linearBucklingEigenAnalysis.analyze(numModes); //Ejecuta el linear buckling Analysis.

        result= run_analysis_step(i,numSteps);

        if(result < 0) //Fallo en run_analysis_step.
          return result;
      }
    metodo_solu->set_owner(old);
    eigen_solu->set_owner(oldE);
    return result;
  }

//! @brief Hace los cambios que sean necesarios tras un cambio en el dominio.
int XC::LinearBucklingAnalysis::domainChanged(void)
  {
    int retval= StaticAnalysis::domainChanged();
    linearBucklingEigenAnalysis.domainChanged();
    return retval;
  }

//! @brief Asigna el algoritmo de solución a emplear en el análisis de autovalores.
int XC::LinearBucklingAnalysis::setLinearBucklingAlgorithm(LinearBucklingAlgo &theLinearBucklingAlgorithm)
  { return linearBucklingEigenAnalysis.setAlgorithm(theLinearBucklingAlgorithm); }


//! @brief Asigna el integrador a emplear en el análisis de autovalores.
int XC::LinearBucklingAnalysis::setLinearBucklingIntegrator(LinearBucklingIntegrator &theLinearBucklingIntegrator)
  { return linearBucklingEigenAnalysis.setIntegrator(theLinearBucklingIntegrator); }

//! @brief Asigna el sistema de ecuaciones lineal a emplear en el análisis de autovalores.
int XC::LinearBucklingAnalysis::setArpackSOE(ArpackSOE &theEigenSOE)
  { return linearBucklingEigenAnalysis.setEigenSOE(theEigenSOE); }


//! @brief Devuelve el autovector que corresponde al modo being passed as parameter.
const XC::Vector &XC::LinearBucklingAnalysis::getEigenvector(int mode)
  {
    static Vector retval(1);
    retval.Zero();
    linearBucklingEigenAnalysis.getEigenvector(mode);
    return retval;
  }

//! @brief Devuelve el autovalor que corresponde al modo being passed as parameter.
const double &XC::LinearBucklingAnalysis::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    retval= linearBucklingEigenAnalysis.getEigenvalue(mode);
    return retval;
  }











