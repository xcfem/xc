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
#include "xc_utils/src/base/CmdStatus.h"


//! @brief Constructor.
XC::LinearBucklingAnalysis::LinearBucklingAnalysis(SoluMethod *metodo,SoluMethod *esolu)
  :StaticAnalysis(metodo), eigen_solu(esolu), linearBucklingEigenAnalysis(esolu),
   numModes(0),linear_buckling_analysis_step(0) {}

//! @brief Lee un objeto Analysis desde archivo
bool XC::LinearBucklingAnalysis::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(LinearBucklingAnalysis) Procesando comando: " << cmd << std::endl;
    if(cmd == "num_modes") //Asigna el número de modos a obtener.
      {
        numModes= interpretaInt(status.GetString());
        return true;
      }
    else
      return StaticAnalysis::procesa_comando(status);
  }

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


//! @brief Devuelve el autovector que corresponde al modo que se pasa como parámetro.
const XC::Vector &XC::LinearBucklingAnalysis::getEigenvector(int mode)
  {
    static Vector retval(1);
    retval.Zero();
    linearBucklingEigenAnalysis.getEigenvector(mode);
    return retval;
  }

//! @brief Devuelve el autovalor que corresponde al modo que se pasa como parámetro.
const double &XC::LinearBucklingAnalysis::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    retval= linearBucklingEigenAnalysis.getEigenvalue(mode);
    return retval;
  }











