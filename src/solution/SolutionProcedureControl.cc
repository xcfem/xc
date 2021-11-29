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
//SolutionProcedureControl.cc

#include "SolutionProcedureControl.h"
#include "domain/domain/Domain.h"
#include "SolutionProcedure.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SolutionStrategy.h"

#include "boost/any.hpp"


//! @brief Default constructor.
XC::SolutionProcedureControl::SolutionProcedureControl(SolutionProcedure *owr)
  : CommandEntity(owr), solu_models(this), solu_methods(this) {}


XC::SolutionProcedure *XC::SolutionProcedureControl::getSolutionProcedure(void)
  { return dynamic_cast<SolutionProcedure *>(Owner()); }

const XC::SolutionProcedure *XC::SolutionProcedureControl::getSolutionProcedure(void) const
  { return dynamic_cast<const SolutionProcedure *>(Owner()); }

//! @brief Return a pointer to the domain on which
//! the solution algorithm operates.
XC::Domain *XC::SolutionProcedureControl::getDomain(void)
  {
    SolutionProcedure *ps= getSolutionProcedure();
    assert(ps); 
    return ps->getDomainPtr();
  }

//! @brief Return a pointer to the domain on which
//! the solution algorithm operates.
const XC::Domain *XC::SolutionProcedureControl::getDomain(void) const
  {
    const SolutionProcedure *ps= getSolutionProcedure();
    assert(ps); 
    return ps->getDomainPtr();
  }

//! @brief Return a pointer to the integrator.
XC::Integrator *XC::SolutionProcedureControl::getIntegratorPtr(void)
  {
    SolutionProcedure *ps= getSolutionProcedure();
    assert(ps);
    return ps->getIntegratorPtr();
  }

//! @brief Return a const pointer to the integrator.
const XC::Integrator *XC::SolutionProcedureControl::getIntegratorPtr(void) const
  {
    const SolutionProcedure *ps= getSolutionProcedure();
    assert(ps);
    return ps->getIntegratorPtr();
  }

//! @bried Return a reference to the model wrapper container.
XC::MapModelWrapper &XC::SolutionProcedureControl::getModelWrapperContainer(void)
  { return solu_models; }

//! @bried Return a reference to the solution procedures container.
XC::SolutionStrategyMap &XC::SolutionProcedureControl::getSolutionStrategyContainer(void)
  { return solu_methods; }

//! @brief Return a pointer to the model wrapper with the identifier
//! being passed as parameter.
const XC::ModelWrapper *XC::SolutionProcedureControl::getModelWrapper(const std::string &cod) const
  { return solu_models.getModelWrapper(cod); }

//! @brief Return a pointer to the model wrapper with the identifier
//! passed as parameter.
XC::ModelWrapper *XC::SolutionProcedureControl::getModelWrapper(const std::string &cod)
  { return solu_models.getModelWrapper(cod); }

//! @brief Return the name of a model wrapper.
const std::string &XC::SolutionProcedureControl::getModelWrapperName(const ModelWrapper *mw) const
  { return solu_models.getModelWrapperName(mw); }


//! @brief Return a const pointer to the solution method.
const XC::SolutionStrategy *XC::SolutionProcedureControl::getSolutionStrategy(const std::string &cod) const
  { return solu_methods.getSolutionStrategy(cod); }

//! @brief Return a const pointer to the solution method.
XC::SolutionStrategy *XC::SolutionProcedureControl::getSolutionStrategy(const std::string &cod)
  { return solu_methods.getSolutionStrategy(cod); }

//! @brief Revert to the initial state.
void XC::SolutionProcedureControl::revertToStart(void)
  {
    getDomain()->revertToStart();
    solu_methods.revertToStart();
  }

//! @brief Clear all.
void XC::SolutionProcedureControl::clearAll(void)
  {
    solu_models.clearAll();
    solu_methods.clearAll();
  }

