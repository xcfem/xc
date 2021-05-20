// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ActionsFamily.h


#ifndef FAMILIAACCIONES_H
#define FAMILIAACCIONES_H

#include "utility/loadCombinations/actions/ActionRValueList.h"

namespace cmb_acc{
class LoadCombinationVector;
class CombinationFactorsMap;
class ActionsAndFactors;
class LeadingActionInfo;

//! @ingroup CMBACC
//
//! @brief Family of actions (permanent, variable, accidental,...)
//!
//! Set of actions that share its variation in time, for example::
//! -Permanent actions.
//! -Variable actions.
//! -Accidental actions.
//! -Earthquake actions.
class ActionsFamily: public NamedEntity
  {
  private:
    ActionRValueList actions; //!< Family actions container.

  protected:
    friend class ActionsAndFactors;
  public:
    ActionsFamily(const std::string &nmb="");
    inline virtual ~ActionsFamily(void) {}
    ActionRValue &insert(const Action &,const std::string &combination_factors_name,const std::string &partial_safety_factors_name);

    //! @brief Return the actions in the family.
    inline const ActionRValueList &getActions(void) const
      { return actions; }
    //! @brief Return the actions in the family.
    inline ActionRValueList &getActions(void)
      { return actions; }
    //! @brief Set the actions in the family.
    inline void setActions(const ActionRValueList &acc)
      { actions= acc; }

    //! brief Return true if no actions in the family.
    inline bool empty(void) const
      { return actions.empty(); }

    const ActionsAndFactors *getActionsAndFactors(void) const;
    const CombinationFactorsMap *getPtrCombinationFactors(void) const;
    const PartialSafetyFactorsMap *getPtrPartialSafetyFactors(void) const;
  };
} //fin namespace nmb_acc.

#endif
