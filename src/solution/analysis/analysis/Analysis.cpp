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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/Analysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/Analysis.cpp
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of Analysis.
// Analysis is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) Analysis.C, revA"

#include <solution/analysis/analysis/Analysis.h>
#include "solution/analysis/ModelWrapper.h"
#include "solution/SoluMethod.h"
#include "solution/ProcSolu.h"
#include "solution/analysis/model/AnalysisModel.h"



//! @brief Constructor.
//!
//! All analysis are associated with a single solution method, this constructor
//! sets up the link.
XC::Analysis::Analysis(SoluMethod *s)
  :analysis_result(-100), solution_method(s) {}

int XC::Analysis::newStepDomain(AnalysisModel *theModel,const double &dT)
  { return theModel->newStepDomain(dT); }

XC::ProcSolu *XC::Analysis::getProcSolu(void)
  { return dynamic_cast<ProcSolu *>(Owner()); }

const XC::ProcSolu *XC::Analysis::getProcSolu(void) const
  { return dynamic_cast<const ProcSolu *>(Owner()); }


//! @brief Returns a pointer to the domain.
XC::Domain *XC::Analysis::getDomainPtr(void)
  {
    ProcSolu *ps= getProcSolu();
    assert(ps);
    return ps->getDomainPtr();
  }

//! @brief Returns a pointer to the domain.
const XC::Domain *XC::Analysis::getDomainPtr(void) const
  {
    const ProcSolu *ps= getProcSolu();
    assert(ps);
    return ps->getDomainPtr();
  }

//! @brief Returns a pointer to the constraint handler.
XC::ConstraintHandler *XC::Analysis::getConstraintHandlerPtr(void)
  {
    if(solution_method)
      return solution_method->getConstraintHandlerPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the DOF numberer.
XC::DOF_Numberer *XC::Analysis::getDOF_NumbererPtr(void) const
  {
    if(solution_method)
      return solution_method->getDOF_NumbererPtr();
    else
      return nullptr;
  }
//! @brief Returns a pointer to the analysis model.
XC::AnalysisModel *XC::Analysis::getAnalysisModelPtr(void) const
  {
    if(solution_method)
      return solution_method->getAnalysisModelPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the linear system of equations.
XC::LinearSOE *XC::Analysis::getLinearSOEPtr(void) const
  {
    if(solution_method)
      return solution_method->getLinearSOEPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
XC::EigenSOE *XC::Analysis::getEigenSOEPtr(void) const
  {
    if(solution_method)
      return solution_method->getEigenSOEPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al integrator otherwise it returns nullptr.
XC::Integrator *XC::Analysis::getIntegratorPtr(void)
  {
    if(solution_method)
      return solution_method->getIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al integrator otherwise it returns nullptr.
const XC::Integrator *XC::Analysis::getIntegratorPtr(void) const
  {
    if(solution_method)
      return solution_method->getIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al integrator incremental otherwise it returns nullptr.
XC::IncrementalIntegrator *XC::Analysis::getIncrementalIntegratorPtr(void)
  {
    if(solution_method)
      return solution_method->getIncrementalIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al EigenIntegrator otherwise it returns nullptr.
XC::EigenIntegrator *XC::Analysis::getEigenIntegratorPtr(void)
  {
    if(solution_method)
      return solution_method->getEigenIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al LinearBucklingIntegrator otherwise it returns nullptr.
XC::LinearBucklingIntegrator *XC::Analysis::getLinearBucklingIntegratorPtr(void)
  {
    if(solution_method)
      return solution_method->getLinearBucklingIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al EigenIntegrator otherwise it returns nullptr.
XC::TransientIntegrator *XC::Analysis::getTransientIntegratorPtr(void)
  {
    if(solution_method)
      return solution_method->getTransientIntegratorPtr();
    else
      return nullptr;
  }

//! @brief Returns, if possible, a pointer al StaticIntegrator otherwise it returns nullptr.
XC::StaticIntegrator *XC::Analysis::getStaticIntegratorPtr(void)
  {
    StaticIntegrator *retval= nullptr;
    if(solution_method)
      retval= solution_method->getStaticIntegratorPtr();
    return retval;
  }

//! @brief Return a pointer to the eigenproblem solution
//! algorithm (if it's not defined it returns nullptr).
XC::EigenAlgorithm *XC::Analysis::getEigenSolutionAlgorithmPtr(void)
  {
    if(solution_method)
      return solution_method->getEigenSolutionAlgorithmPtr();
    else
      return nullptr;
  }

//! @brief Return a pointer to the linear SOE solution
//! algorithm (if it's not defined it returns nullptr).
XC::EquiSolnAlgo *XC::Analysis::getEquiSolutionAlgorithmPtr(void)
  {
    if(solution_method)
      return solution_method->getEquiSolutionAlgorithmPtr();
    else
      return nullptr;
  }

//! @brief Return a pointer to the domain decomposition solution
//! algorithm (if it's not defined it returns nullptr).
XC::DomainDecompAlgo *XC::Analysis::getDomainDecompSolutionAlgorithmPtr(void)
  {
    if(solution_method)
      return solution_method->getDomainDecompSolutionAlgorithmPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the convergence test (only for suitable analysis).
XC::ConvergenceTest *XC::Analysis::getConvergenceTestPtr(void)
  {
    if(solution_method)
      return solution_method->getConvergenceTestPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the convergence test (only for suitable analysis).
const XC::ConvergenceTest *XC::Analysis::getConvergenceTestPtr(void) const
  {
    if(solution_method)
      return solution_method->getConvergenceTestPtr();
    else
      return nullptr;
  }

//! @brief Returns a pointer to the DomainSolver.
const XC::DomainSolver *XC::Analysis::getDomainSolver(void) const
  {
    std::cerr << "Analysis::getDomainSolver must be redefined in derived classes"
              << std::endl;
    return nullptr;
  }

//! @brief Returns a pointer to the DomainSolver.
XC::DomainSolver *XC::Analysis::getDomainSolver(void)
  {
    std::cerr << "Analysis::getDomainSolver must be redefined in derived classes"
              << std::endl;
    return nullptr;
  }

//! @brief Returns a pointer to the subdomain.
const XC::Subdomain *XC::Analysis::getSubdomain(void) const
  {
    std::cerr << "Analysis::getSubdomain must be redefined in derived classes"
              << std::endl;
    return nullptr;
  }

//! @brief Returns a pointer to the subdomain.
XC::Subdomain *XC::Analysis::getSubdomain(void)
  {
    std::cerr << "Analysis::getSubdomain must be redefined in derived classes"
              << std::endl;
    return nullptr;
  }

//! @brief Deletes all members (Constraint handler, analysis model,...).
void XC::Analysis::clearAll(void)
  {
    std::cerr << "Analysis::clearAll() DEPRECATED." << std::endl;
  }

//! @brief Sets the renumerador to use in the analysis.
int XC::Analysis::setNumberer(DOF_Numberer &theNewNumberer) 
  {
    if(solution_method)
      if(solution_method->getModelWrapperPtr())
        return solution_method->getModelWrapperPtr()->setNumberer(theNewNumberer);
    return 0;
  }

//! @brief Sets the linear system of equations to use in the analysis.
int XC::Analysis::setLinearSOE(LinearSOE &theNewSOE)
  {
    if(solution_method)
      return solution_method->setLinearSOE(theNewSOE);
    else
      return 0;
  }

//! @brief Sets the sistema de eigenvalues to use in the analysis.
int XC::Analysis::setEigenSOE(EigenSOE &theSOE)
  {
    if(solution_method)
      return solution_method->setEigenSOE(theSOE);
    else
      return 0;
  }

//! @brief Sets the integrator to use in the analysis.
int XC::Analysis::setIntegrator(Integrator &theNewIntegrator)
  {
    if(solution_method)
      return solution_method->setIntegrator(theNewIntegrator);
    else
      return 0;
  }

//! @brief Set the solution algorithm to be used in the analysis.
int XC::Analysis::setAlgorithm(SolutionAlgorithm &theNewAlgorithm) 
  {
    if(solution_method)
      return solution_method->setAlgorithm(theNewAlgorithm);
    else
      return 0;
  }

void XC::Analysis::brokeConstraintHandler(const CommParameters &cp,const ID &data)
  { solution_method->getModelWrapperPtr()->brokeConstraintHandler(cp,data); }

void XC::Analysis::brokeNumberer(const CommParameters &cp,const ID &data)
  { solution_method->getModelWrapperPtr()->brokeNumberer(cp,data); }

void XC::Analysis::brokeAnalysisModel(const CommParameters &cp,const ID &data)
  { solution_method->getModelWrapperPtr()->brokeAnalysisModel(cp,data); }

void XC::Analysis::brokeDDLinearSOE(const CommParameters &cp,const ID &data)
  { solution_method->brokeDDLinearSOE(cp,data); }

void XC::Analysis::brokeLinearSOE(const CommParameters &cp,const ID &data)
  { solution_method->brokeLinearSOE(cp,data); }

void XC::Analysis::brokeIncrementalIntegrator(const CommParameters &cp,const ID &data)
  { solution_method->brokeIncrementalIntegrator(cp,data); }

void XC::Analysis::brokeStaticIntegrator(const CommParameters &cp,const ID &data)
  { solution_method->brokeStaticIntegrator(cp,data); }

void XC::Analysis::brokeTransientIntegrator(const CommParameters &cp,const ID &data)
  { solution_method->brokeTransientIntegrator(cp,data); }

void XC::Analysis::brokeDomainDecompAlgo(const CommParameters &cp,const ID &data)
  { solution_method->brokeDomainDecompAlgo(cp,data); }

void XC::Analysis::brokeEquiSolnAlgo(const CommParameters &cp,const ID &data)
  { solution_method->brokeEquiSolnAlgo(cp,data); }

