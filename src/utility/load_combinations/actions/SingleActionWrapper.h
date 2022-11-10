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
//SingleActionWrapper.hxx
//Representative values of an action.

#ifndef SINGLEACTIONWRAPPER_H
#define SINGLEACTIONWRAPPER_H

#include "ActionWrapper.h"
#include "ActionRepresentativeValues.h"

namespace cmb_acc{

//! @brief Wrapper for a single action.
//! @ingroup CMBACC
class SingleActionWrapper: public ActionWrapper
  {
    friend class ActionWrapperList;
  protected:
    ActionRepresentativeValues action;
    //! @brief Default constructor.
    SingleActionWrapper(const std::string &n="", const std::string &descrip="", ActionWrapperList *list= nullptr);
    SingleActionWrapper(const Action &a, ActionWrapperList *list= nullptr,const std::string &nmb_comb_factors= "", const std::string &nmb_partial_safety_factors= "");
    virtual std::string getName(void) const;
  public:    
    std::vector<const Action *> getWrappedActions(void) const;
    virtual Action getRepresentativeValue(const LeadingActionInfo &) const;  
    bool Incompatible(const ActionWrapper &f) const;
    const ActionRelationships &getRelaciones(void) const;
  };
  
} //fin namespace nmb_acc.

#endif
