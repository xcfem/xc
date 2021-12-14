// -*-c++-*-
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
//SolutionProcedureControl.h

#ifndef PROCSOLUCONTROL_H
#define PROCSOLUCONTROL_H

#include "utility/kernel/CommandEntity.h"
#include "analysis/MapModelWrapper.h"
#include "SolutionStrategyMap.h"

namespace XC {

class SolutionProcedure;
class Domain;

//!  @ingroup Solu
//! 
//! @brief Container for the objects that control the solution procedure.
class SolutionProcedureControl: public CommandEntity
  {
  private:
    MapModelWrapper solu_models; //!< Wrapper for the finite element model
    SolutionStrategyMap solu_methods; //!< Solution methods.

    SolutionProcedure *getSolutionProcedure(void);
    const SolutionProcedure *getSolutionProcedure(void) const;
  public:
    SolutionProcedureControl(SolutionProcedure *owr);

    Domain *getDomain(void);
    const Domain *getDomain(void) const;
    Integrator *getIntegratorPtr(void);
    const Integrator *getIntegratorPtr(void) const;

    DataOutputHandler::map_output_handlers *getOutputHandlers(void) const;
    const ModelWrapper *getModelWrapper(const std::string &) const;
    ModelWrapper *getModelWrapper(const std::string &);
    const std::string &getModelWrapperName(const ModelWrapper *) const;
    const SolutionStrategy *getSolutionStrategy(const std::string &) const;
    SolutionStrategy *getSolutionStrategy(const std::string &);

    MapModelWrapper &getModelWrapperContainer(void);
    SolutionStrategyMap &getSolutionStrategyContainer(void);

    void revertToStart(void);
    void clearAll(void);
  };

} // end of XC namespace

#endif
