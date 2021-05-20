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
//ActionRValueList.hxx
//Conjunto de acciones.

#ifndef LISTAVRACCION_HXX
#define LISTAVRACCION_HXX

#include "utility/kernel/CommandEntity.h"
#include <deque>
#include "ActionRValue.h"

namespace cmb_acc{
class Variation;
class Variations;
class CombinationFactorsMap;
class PartialSafetyFactorsMap;
class ActionsFamily;

//! @ingroup CMBACC
//
//! @brief List of representative values of actions (objetos ActionRValue).
class ActionRValueList: public std::deque<ActionRValue>, public CommandEntity
  {
    friend class Variation;
    friend class ActionsFamily;

    ActionRValue &push_back(const ActionRValue &a);
  public:
    int getIndex(const ActionRValue *) const;
    ActionRValue &insert(const Action &,const std::string &,const std::string &);
    Action buildCombination(const Variation &v,const LeadingActionInfo &) const;
    const CombinationFactorsMap *getPtrCombinationFactors(void) const;
    const PartialSafetyFactorsMap *getPtrPartialSafetyFactors(void) const;
    Variations computeVariations(const bool &,const bool &,const int &) const;
    LoadCombinationVector getCombinations(const bool &,const bool &,const LeadingActionInfo &) const;
    LoadCombinationVector getCombinationsWhenLeading(const bool &,const bool &,const short int &v) const;
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const ActionRValueList &vs);

} //fin namespace nmb_acc.

#endif
