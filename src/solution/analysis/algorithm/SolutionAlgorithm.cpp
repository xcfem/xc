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
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/SolutionAlgorithm.cpp,v $
                                                                        
                                                                        
#include "SolutionAlgorithm.h"
#include "solution/AnalysisAggregation.h"
// #include <utility/recorder/Recorder.h>
// #include <utility/recorder/DamageRecorder.h>
// #include <utility/recorder/DatastoreRecorder.h>
// #include <utility/recorder/DriftRecorder.h>
// #include <utility/recorder/ElementRecorder.h>
// #include <utility/recorder/EnvelopeElementRecorder.h>
// #include <utility/recorder/EnvelopeNodeRecorder.h>
// #include <utility/recorder/FilePlotter.h>
// #include <utility/recorder/NodeRecorder.h>
// #include <utility/recorder/GSA_Recorder.h>
// #include <utility/recorder/MaxNodeDispRecorder.h>
// #include <utility/recorder/PatternRecorder.h>
#include <solution/analysis/model/AnalysisModel.h>


//! @brief Constructor.
XC::SolutionAlgorithm::SolutionAlgorithm(AnalysisAggregation *owr,int clasTag)
  :MovableObject(clasTag), ObjWithRecorders(owr,nullptr)
  {
    Domain *dom= get_domain_ptr();
    if(dom)
      ObjWithRecorders::setLinks(dom);
  }

//! @brief Perform the appropriate modifications as a result of a change
//! in the domain.
//!
//! Is called by the Analysis if the domain changes. It is called after
//! domainChange() has been called on the ConstraintHandler,
//! DOF\_Numberer and the Integrator and after setSize() has been
//! called on the SystemOfEqn object. For base class nothing is done and
//! \f$0\f$ is returned. The subclasses can provide their own implementation
//! of this method if anything needs to be done, e.g. memory allocation,
//! To return \f$0\f$ if successful, a negative number if not.
int XC::SolutionAlgorithm::domainChanged(void)
  { return 0; }

//! @brief Return a pointer to the solution method which owns this object.
XC::AnalysisAggregation *XC::SolutionAlgorithm::getAnalysisAggregation(void)
  { return dynamic_cast<AnalysisAggregation *>(Owner()); }

//! @brief Return a pointer to the solution method which owns this object.
const XC::AnalysisAggregation *XC::SolutionAlgorithm::getAnalysisAggregation(void) const
  { return dynamic_cast<const AnalysisAggregation *>(Owner()); }


//! @brief Return a const pointer to the analysis model.
const XC::AnalysisModel *XC::SolutionAlgorithm::getAnalysisModelPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Return a pointer to the analysis model.
XC::AnalysisModel *XC::SolutionAlgorithm::getAnalysisModelPtr(void)
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Return a pointer to the integrator.
XC::Integrator *XC::SolutionAlgorithm::getIntegratorPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Return a const pointer to the integrator.
const XC::Integrator *XC::SolutionAlgorithm::getIntegratorPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Return a pointer to the linear system of equations.
XC::LinearSOE *XC::SolutionAlgorithm::getLinearSOEPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Return a const pointer to the linear system of equations.
const XC::LinearSOE *XC::SolutionAlgorithm::getLinearSOEPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Return a pointer to the eigen system of equations.
XC::EigenSOE *XC::SolutionAlgorithm::getEigenSOEPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Return a const pointer to the eigen system of equations.
const XC::EigenSOE *XC::SolutionAlgorithm::getEigenSOEPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Return a const pointer to the DomainSolver.
const XC::DomainSolver *XC::SolutionAlgorithm::getDomainSolverPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getDomainSolverPtr();
  }

//! @brief Return a pointer to the DomainSolver.
XC::DomainSolver *XC::SolutionAlgorithm::getDomainSolverPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getDomainSolverPtr();
  }

//! @brief Return a const pointer to the subdomain.
const XC::Subdomain *XC::SolutionAlgorithm::getSubdomainPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getSubdomainPtr();
  }

//! @brief Return a pointer to the subdomain.
XC::Subdomain *XC::SolutionAlgorithm::getSubdomainPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getSubdomainPtr();
  }

//! @brief Return a pointer to the domain.
XC::Domain *XC::SolutionAlgorithm::get_domain_ptr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Send object members through the communicator argument.
int XC::SolutionAlgorithm::sendData(Communicator &comm)
  {
    return 0;
  }

//! @brief Receive object members through the communicator argument.
int XC::SolutionAlgorithm::recvData(const Communicator &comm)
  {
    return 0;
  }
