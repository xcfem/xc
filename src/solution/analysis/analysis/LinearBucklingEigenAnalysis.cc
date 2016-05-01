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
//LinearBucklingEigenAnalysis.cpp


#include <solution/analysis/analysis/LinearBucklingEigenAnalysis.h>
#include <solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/integrator/eigen/LinearBucklingIntegrator.h>
#include <domain/domain/Domain.h>


//! @brief Constructor.
XC::LinearBucklingEigenAnalysis::LinearBucklingEigenAnalysis(SoluMethod *metodo)
  :EigenAnalysis(metodo) {}

//! @brief Ejecuta el análisis.
int XC::LinearBucklingEigenAnalysis::setupPreviousStep(void)
  {
    int result = 0;
    Domain *the_Domain = this->getDomainPtr();

    // check for change in XC::Domain since last step. As a change can
    // occur in a commit() in a domaindecomp with load balancing
    // this must now be inside the loop
    int stamp = the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp = stamp;
	result = domainChanged();
	if(result < 0)
          {
	    std::cerr << "LinearBucklingEigenAnalysis::setupPreviousStep() - domainChanged failed\n";
	    return -1;
	  }
      }
    result = getLinearBucklingIntegratorPtr()->formKt(); //Matriz de rigidez en el paso previo.
    return result;
  }

//! @brief Asigna el algoritmo a emplear en el análisis.
int XC::LinearBucklingEigenAnalysis::setAlgorithm(LinearBucklingAlgo &theAlgo)
  { return EigenAnalysis::setAlgorithm(theAlgo); }

//! @brief Asigna el integrador a emplear en el análisis.
int XC::LinearBucklingEigenAnalysis::setIntegrator(LinearBucklingIntegrator &theIntegrator)
  { return EigenAnalysis::setIntegrator(theIntegrator); }

//! @brief Asigna el sistema de autovalores a emplear en el análisis.
int XC::LinearBucklingEigenAnalysis::setEigenSOE(ArpackSOE &theSOE)
  { return EigenAnalysis::setEigenSOE(theSOE); }

//! @brief Devuelve el autovalor que corresponde al modo que se pasa como parámetro.
const double &XC::LinearBucklingEigenAnalysis::getEigenvalue(int mode) const
  {
    const double &gamma= EigenAnalysis::getEigenvalue(mode);
    static double retval= 0.0;
    retval= 1.0/(1.0-gamma); //Bathe página 632.
    return retval;
  }

