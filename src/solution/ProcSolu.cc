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
#include "FEProblem.h"

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
#include "solution/AnalysisAggregation.h"

#include <domain/domain/Domain.h>


#include "boost/any.hpp"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

void XC::ProcSolu::free_analysis(void)
  {
    if(theAnalysis)
      {
        delete theAnalysis;
        theAnalysis= nullptr;
      }
  }

bool XC::ProcSolu::alloc_analysis(const std::string &nmb,const std::string &analysis_aggregation_code,const std::string &cod_solu_eigenM)
  {
    free_analysis();
    AnalysisAggregation *analysis_aggregation= solu_control.getAnalysisAggregation(analysis_aggregation_code);
    if(analysis_aggregation)
      {
        if(analysis_aggregation->CheckPointers())
          {
            if(nmb=="direct_integration_analysis")
              theAnalysis= new DirectIntegrationAnalysis(analysis_aggregation);
            else if(nmb=="eigen_analysis")
              theAnalysis= new EigenAnalysis(analysis_aggregation);
            else if(nmb=="modal_analysis")
              theAnalysis= new ModalAnalysis(analysis_aggregation);
            else if(nmb=="linear_buckling_analysis")
              {
                AnalysisAggregation *eigenM= solu_control.getAnalysisAggregation(cod_solu_eigenM);
                if(eigenM)
                  theAnalysis= new LinearBucklingAnalysis(analysis_aggregation,eigenM);
              }
            else if(nmb=="linear_buckling_eigen_analysis") //Used only inside LinearBucklingAnalysis.
              theAnalysis= new LinearBucklingEigenAnalysis(analysis_aggregation);
            else if(nmb=="static_analysis")
              theAnalysis= new StaticAnalysis(analysis_aggregation);
            else if(nmb=="variable_time_step_direct_integration_analysis")
              theAnalysis= new VariableTimeStepDirectIntegrationAnalysis(analysis_aggregation);
	  }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; analysis type: '"
                    << nmb << "' unknown." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; analysis method: '"
                << analysis_aggregation_code << "' not found, in command: " << nmb << std::endl;
    if(theAnalysis)
      theAnalysis->set_owner(this);

    return (theAnalysis!=nullptr);
  }

void XC::ProcSolu::copy_analysis(Analysis *ptr)
  {
    if(ptr)
      {
        free_analysis();
        theAnalysis= ptr->getCopy();
        theAnalysis->set_owner(this);
      }
    else
     std::cerr << "ProcSolu::copy_analysis; pointer to analysis is null." << std::endl;
  }

//! @brief Defines type of analysis (static, dynamic,...)
XC::Analysis &XC::ProcSolu::newAnalysis(const std::string &nmb,const std::string &analysis_aggregation_code,const std::string &cod_solu_eigenM)
  {
    alloc_analysis(nmb,analysis_aggregation_code,cod_solu_eigenM);
    assert(theAnalysis);
    return *theAnalysis;
  }

void XC::ProcSolu::free_mem(void)
  {
    free_analysis();
  }

//! @brief Default constructor.
XC::ProcSolu::ProcSolu(FEProblem *owr)
  : EntCmd(owr), solu_control(this), theAnalysis(nullptr) {}

//! @brief Copy constructor.
XC::ProcSolu::ProcSolu(const ProcSolu &otro)
  : EntCmd(otro), solu_control(otro.solu_control), theAnalysis(nullptr)
  { copy_analysis(otro.theAnalysis); }

//! @brief Assignment operator.
XC::ProcSolu &XC::ProcSolu::operator=(const ProcSolu &otro)
  {
    EntCmd::operator=(otro);
    solu_control= otro.solu_control;
    copy_analysis(otro.theAnalysis);
    return *this;
  }

//! @brief Return the model to their initial state.
void XC::ProcSolu::revertToStart(void)
  { solu_control.revertToStart(); }

//! @brief Delete all entities in the FE problem.
void XC::ProcSolu::clearAll(void)
  {
    solu_control.clearAll();
    free_analysis();
  }

//! @brief Destructor.
XC::ProcSolu::~ProcSolu(void)
  { clearAll(); }

XC::FEProblem *XC::ProcSolu::getFEProblem(void)
  { return dynamic_cast<FEProblem *>(Owner()); }

const XC::FEProblem *XC::ProcSolu::getFEProblem(void) const
  { return dynamic_cast<const FEProblem *>(Owner()); }

//! @brief Return a pointer to the domain.
XC::Domain *XC::ProcSolu::getDomainPtr(void)
  {
    FEProblem *prb= getFEProblem();
    assert(prb);
    return prb->getDomain();
  }

//! @brief Return a const pointer to the domain.
const XC::Domain *XC::ProcSolu::getDomainPtr(void) const
  {
    const FEProblem *prb= getFEProblem();
    assert(prb);
    return prb->getDomain();
  }

//! @brief Return a pointer to the integrator.
XC::Integrator *XC::ProcSolu::getIntegratorPtr(void)
  {
    Integrator *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getIntegratorPtr();
    return retval;
  }

//! @brief Return a pointer to the integrator.
const XC::Integrator *XC::ProcSolu::getIntegratorPtr(void) const
  {
    const Integrator *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getIntegratorPtr();
    return retval;
  }

//! @brief Return a pointer to the output handlers
XC::DataOutputHandler::map_output_handlers *XC::ProcSolu::getOutputHandlers(void) const
  {
    const FEProblem *prb= getFEProblem();
    assert(prb);
    return prb->getOutputHandlers();
  }

//! @brief Return a pointer to the DomainSolver.
const XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void) const
  {
    const DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Return a pointer to the Domain Solver.
XC::DomainSolver *XC::ProcSolu::getDomainSolverPtr(void)
  {
    DomainSolver *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getDomainSolver();
    return retval;
  }

//! @brief Return a pointer to the subdomain.
const XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void) const
  {
    const Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Return a pointer to the subdomain.
XC::Subdomain *XC::ProcSolu::getSubdomainPtr(void)
  {
    Subdomain *retval= nullptr;
    if(theAnalysis)
      retval= theAnalysis->getSubdomain();
    return retval;
  }

//! @brief Return a reference to the objects
//! that control the solution procedure.
XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void)
  { return solu_control; }

//! @brief Return a reference to the objects
//! that control the solution procedure.
const XC::ProcSoluControl &XC::ProcSolu::getSoluControl(void) const
  { return solu_control; }

//! @brief Return a pointer to the analysis.
XC::Analysis *XC::ProcSolu::getAnalysisPtr(void)
  { return theAnalysis; }

//! @brief Return a pointer to the analysis.
const XC::Analysis *XC::ProcSolu::getAnalysisPtr(void) const
  { return theAnalysis; }

//! @brief Return a reference to the analysis object.
XC::Analysis &XC::ProcSolu::getAnalysis(void)
  {
    if(!theAnalysis)
      std::cerr << "ProcSolu::getAnalysis; analysis object is not defined." << std::endl;
    return *theAnalysis;
  }
