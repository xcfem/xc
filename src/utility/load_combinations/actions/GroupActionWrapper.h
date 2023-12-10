// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//GroupActionWrapper.hxx
//Representative values of an action.

#ifndef GROUPACTIONWRAPPER_H
#define GROUPACTIONWRAPPER_H

#include "ActionWrapper.h"
#include "ActionRepresentativeValues.h"

namespace cmb_acc{

//! @brief Wrapper for a group action.
//! @ingroup CMBACC
class GroupActionWrapper: public ActionWrapper
  {
    typedef std::deque<ActionRepresentativeValues> action_container;
    typedef action_container::iterator iterator;
    typedef action_container::const_iterator const_iterator;
    friend class ActionWrapperList;
  protected:
    action_container actions;
    //! @brief Default constructor.
    GroupActionWrapper(const std::vector<std::string> &names={""}, const std::vector<std::string> &descriptions={""}, ActionWrapperList *list= nullptr);
    GroupActionWrapper(const std::vector<Action> &, ActionWrapperList *list= nullptr,const std::vector<std::string> &combination_factors_names= {""}, const std::string &nmb_partial_safety_factors= "");
    virtual std::string getName(void) const;
  public:    
    std::vector<const Action *> getWrappedActions(void) const;
    const Action *getAction(const size_t &) const;
    virtual Action getRepresentativeValue(const LeadingActionInfo &) const;  
    const ActionRelationships &getRelaciones(void) const;
    bool notDeterminant(void) const;
  };
  
} //fin namespace nmb_acc.

#endif
