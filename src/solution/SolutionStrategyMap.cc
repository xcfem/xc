//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SolutionStrategyMap.cc

#include "SolutionStrategyMap.h"
#include "domain/domain/Domain.h"
#include "SolutionProcedure.h"

#include "solution/analysis/ModelWrapper.h"
#include "solution/SolutionStrategy.h"

#include "boost/any.hpp"


//! @brief Default constructor.
XC::SolutionStrategyMap::SolutionStrategyMap(SolutionProcedureControl *owr)
  : CommandEntity(owr) {}

//! @brief Return a pointer to the object owner.
XC::SolutionProcedureControl *XC::SolutionStrategyMap::getSolutionProcedureControl(void)
  { return dynamic_cast<SolutionProcedureControl *>(Owner()); }

//! @brief Return a pointer to the object owner.
const XC::SolutionProcedureControl *XC::SolutionStrategyMap::getSolutionProcedureControl(void) const
  { return dynamic_cast<const SolutionProcedureControl *>(Owner()); }

//! @brief Return a pointer to the model wrapper with the identifier
//! being passed as parameter.
const XC::ModelWrapper *XC::SolutionStrategyMap::getModelWrapper(const std::string &cod) const
  { return getSolutionProcedureControl()->getModelWrapper(cod); }

//! @brief Return a pointer to the model wrapper with the identifier
//! passed as parameter.
XC::ModelWrapper *XC::SolutionStrategyMap::getModelWrapper(const std::string &cod)
  { return getSolutionProcedureControl()->getModelWrapper(cod); }

//! @brief Return the name of a model wrapper.
const std::string &XC::SolutionStrategyMap::getModelWrapperName(const ModelWrapper *mw) const
  { return getSolutionProcedureControl()->getModelWrapperName(mw); }

//! @brief Return true if the solution method exists
bool XC::SolutionStrategyMap::SolutionStrategyExists(const std::string &cod) const
  { 
    map_solu_method::const_iterator i= solu_methods.find(cod);
    return (i != solu_methods.end());
  }

//! @brief Returns a const pointer to the solution method.
const XC::SolutionStrategy *XC::SolutionStrategyMap::getSolutionStrategy(const std::string &cod) const
  {
    const SolutionStrategy *retval= nullptr;
    map_solu_method::const_iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Returns a pointer to the solution method.
XC::SolutionStrategy *XC::SolutionStrategyMap::getSolutionStrategy(const std::string &cod)
  {
    SolutionStrategy *retval= nullptr;
    map_solu_method::iterator i= solu_methods.find(cod);
    if(i != solu_methods.end())
      retval= &((*i).second);
    return retval;
  }

//! @brief Creates a new solution method with the code being passed as parameter.
//! @brief Si ya existe returns a pointer al mismo.
XC::SolutionStrategy &XC::SolutionStrategyMap::createSolutionStrategy(const std::string &cod,ModelWrapper *sm)
  {
    SolutionStrategy *retval= nullptr;
    if(SolutionStrategyExists(cod))
      retval= getSolutionStrategy(cod);
    else
      {
        retval= &(solu_methods[cod]= SolutionStrategy(nullptr,sm));
	retval->set_owner(this);
      }
    return *retval;
  }

//! @brief Creates a new solution method with the code being passed as parameter.
XC::SolutionStrategy &XC::SolutionStrategyMap::newSolutionStrategy(const std::string &cod_solu_method,const std::string &cod_solu_model)
  {
    SolutionProcedureControl *sc= dynamic_cast<SolutionProcedureControl *>(Owner());
    ModelWrapper *mdl= sc->getModelWrapper(cod_solu_model);
    SolutionStrategy &retval= createSolutionStrategy(cod_solu_method,mdl);
    return retval;
  }

//! @brief Returns todo a su estado original.
void XC::SolutionStrategyMap::revertToStart(void)
  {
    for(iterator i= solu_methods.begin();i!=solu_methods.end();i++)
      (*i).second.revertToStart();
  }

//! @brief Clears all.
void XC::SolutionStrategyMap::clearAll(void)
  { solu_methods.clear(); }


