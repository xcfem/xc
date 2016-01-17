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
//ProcSolu.cc

#include "ProcSolu.h"
#include "ProblemaEF.h"

//Analysis
#include <solution/analysis/analysis/Analysis.h>
#include <solution/analysis/analysis/EigenAnalysis.h>
#include "solution/analysis/analysis/ModalAnalysis.h"
#include <solution/analysis/analysis/LinearBucklingAnalysis.h>
#include <solution/analysis/analysis/LinearBucklingEigenAnalysis.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>
#include <solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h>


#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"

#include <domain/domain/Domain.h>

#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

void XC::ProcSolu::libera_analysis(void)
  {
    if(theAnalysis)
      {
        delete theAnalysis;
        theAnalysis= nullptr;
      }
  }

bool XC::ProcSolu::alloc_analysis(const std::string &nmb,const std::string &cod_solu_metodo,const std::string &cod_solu_eigenM)
  {
    libera_analysis();
    SoluMethod *metodo= solu_control.getSoluMethod(cod_solu_metodo);
    if(metodo)
      {
        if(metodo->CheckPointers())
          {
            if(nmb=="direct_integration_analysis")
              theAnalysis= new DirectIntegrationAnalysis(metodo);
            else if(nmb=="eigen_analysis")
              theAnalysis= new EigenAnalysis(metodo);
            else if(nmb=="modal_analysis")
              theAnalysis= new ModalAnalysis(metodo);
            else if(nmb=="linear_buckling_analysis")
              {
                SoluMethod *eigenM= solu_control.getSoluMethod(cod_solu_eigenM);
                if(eigenM)
                  theAnalysis= new LinearBucklingAnalysis(metodo,eigenM);
              }
            else if(nmb=="linear_buckling_eigen_analysis") //Sólo se usa dentro de LinearBucklingAnalysis.
              theAnalysis= new LinearBucklingEigenAnalysis(metodo);
            else if(nmb=="static_analysis")
              theAnalysis= new StaticAnalysis(metodo);
            else if(nmb=="variable_time_step_direct_integration_analysis")
              theAnalysis= new VariableTimeStepDirectIntegrationAnalysis(metodo);
	  }
      }
    else
      std::cerr << "ProcSolu::alloc_analysis; no se encontró el método de análisis: '"
                << cod_solu_metodo << "' para el comando: " << nmb << std::endl;
    if(theAnalysis)
      theAnalysis->set_owner(this);

    return (theAnalysis!=nullptr);
  }

void XC::ProcSolu::copia_analysis(Analysis *ptr)
  {
    if(ptr)
      {
        libera_analysis();
        theAnalysis= ptr->getCopy();
        theAnalysis->set_owner(this);
      }
    else
     std::cerr << "ProcSolu::copia_analysis; se pasó un puntero nulo." << std::endl;
  }

//! @brief Define el tipo de análisis (estático, dinámico,...)
XC::Analysis &XC::ProcSolu::newAnalysis(const std::string &nmb,const std::string &cod_solu_metodo,const std::string &cod_solu_eigenM)
  {
    alloc_analysis(nmb,cod_solu_metodo,cod_solu_eigenM);
    assert(theAnalysis);
    return *theAnalysis;
  }

void XC::ProcSolu::libera(void)
  {
    libera_analysis();
  }

//! @brief Constructor por defecto.
XC::ProcSolu::ProcSolu(ProblemaEF *owr)
  : EntCmd(owr), solu_control(this), theAnalysis(nullptr) {}

//! @brief Constructor de copia.
XC::ProcSolu::ProcSolu(const ProcSolu &otro)
  : EntCmd(otro), solu_control(otro.solu_control), theAnalysis(nullptr)
  { copia_analysis(otro.theAnalysis); }

//! @brief Operador asignación.
XC::ProcSolu &XC::ProcSolu::operator=(const ProcSolu &otro)
  {
    EntCmd::operator=(otro);
    solu_control= otro.solu_control;
    copia_analysis(otro.theAnalysis);
    return *this;
  }

//! @brief Devuelve el modelo a su estado original.
void XC::ProcSolu::revertToStart(void)
  { solu_control.revertToStart(); }

void XC::ProcSolu::clearAll(void)
  {
    solu_control.clearAll();
    libera_analysis();
  }

//! @brief Destructor.
XC::ProcSolu::~ProcSolu(void)
  { clearAll(); }

XC::ProblemaEF *XC::ProcSolu::getProblemaEF(void)
  { return dynamic_cast<ProblemaEF *>(Owner()); }

const XC::ProblemaEF *XC::ProcSolu::getProblemaEF(void) const
  { return dynamic_cast<const ProblemaEF *>(Owner()); }

XC::Domain *XC::ProcSolu::getDomainPtr(void)
  {
    ProblemaEF *prb= getProblemaEF();
    assert(prb);
    return prb->getDomain();
  }

const XC::Domain *XC::ProcSolu::getDomainPtr(void) const
  {
    const ProblemaEF *prb= getProblemaEF();
    assert(prb);
    return prb->getDomain();
  }

//! @brief Devuelve un puntero al integrador.
XC::Integrator *XC::ProcSolu::getIntegratorPtr(void)
  {
    Integrator *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getIntegratorPtr();
    return retval;
  }

//! @brief Devuelve un puntero al integrador.
const XC::Integrator *XC::ProcSolu::getIntegratorPtr(void) const
  {
    const Integrator *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getIntegratorPtr();
    return retval;
  }

XC::DataOutputHandler::map_output_handlers *XC::ProcSolu::getOutputHandlers(void) const
  {
    const ProblemaEF *prb= getProblemaEF();
    assert(prb);
    return prb->getOutputHandlers();
  }

//! @brief Devuelve un puntero al DomainSolver.
const XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void) const
  {
    const DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Devuelve un puntero al DomainSolver.
XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void)
  {
    DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Devuelve un puntero al subdominio.
const XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void) const
  {
    const Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Devuelve un puntero al subdominio.
XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void)
  {
    Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Devuelve la referencia al objeto que controla
//! el modo de obtener la solución.
XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void)
  { return solu_control; }

//! @brief Devuelve la referencia al objeto que controla
//! el modo de obtener la solución.
const XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void) const
  { return solu_control; }

//! @brief Devuelve un puntero al análisis.
XC::Analysis *XC::ProcSolu::getAnalysisPtr(void)
  { return theAnalysis; }

//! @brief Devuelve un puntero al análisis.
const XC::Analysis *XC::ProcSolu::getAnalysisPtr(void) const
  { return theAnalysis; }

//! @brief Devuelve una referencia al análisis.
XC::Analysis &XC::ProcSolu::getAnalysis(void)
  {
    if(!theAnalysis)
      std::cerr << "ProcSolu::getAnalysis; el análisis no se ha definido." << std::endl;
    return *theAnalysis;
  }
