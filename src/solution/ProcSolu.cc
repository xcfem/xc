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
      std::cerr << "ProcSolu::alloc_analysis; no se encontró el analysis method: '"
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
     std::cerr << "ProcSolu::copia_analysis; se pasó a null pointer." << std::endl;
  }

//! @brief Define el analysis type (estático, dinámico,...)
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

//! @brief Default constructor.
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

//! @brief Returns a pointer to the integrator.
XC::Integrator *XC::ProcSolu::getIntegratorPtr(void)
  {
    Integrator *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getIntegratorPtr();
    return retval;
  }

//! @brief Returns a pointer to the integrator.
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

//! @brief Returns a pointer to the DomainSolver.
const XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void) const
  {
    const DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Returns a pointer to the DomainSolver.
XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void)
  {
    DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Returns a pointer to the subdomain.
const XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void) const
  {
    const Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Returns a pointer to the subdomain.
XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void)
  {
    Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Returns a reference to the objects
//! that controls the solution procedure.
XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void)
  { return solu_control; }

//! @brief Returns a reference to the objects
//! that controls the solution procedure.
const XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void) const
  { return solu_control; }

//! @brief Returns a pointer to the analysis.
XC::Analysis *XC::ProcSolu::getAnalysisPtr(void)
  { return theAnalysis; }

//! @brief Returns a pointer to the analysis.
const XC::Analysis *XC::ProcSolu::getAnalysisPtr(void) const
  { return theAnalysis; }

//! @brief Returns a reference to the analysis object.
XC::Analysis &XC::ProcSolu::getAnalysis(void)
  {
    if(!theAnalysis)
      std::cerr << "ProcSolu::getAnalysis; analysis object is not defined." << std::endl;
    return *theAnalysis;
  }
