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
//ActionWrapper.hxx
//Representative values of an action.

#ifndef ACTIONWRAPPER_H
#define ACTIONWRAPPER_H

#include <iostream>
#include "Action.h"
#include "utility/kernel/EntityWithOwner.h"

namespace cmb_acc{

class PartialSafetyFactors;
class ActionWrapperList;
class ActionsFamily;
class LeadingActionInfo;
class Variations;
  
//! @ingroup CMBACC
//
//! @brief Base class for action design values and action groups.
class ActionWrapper: public EntityWithOwner
  {
  protected:
    friend class ActionWrapperList;
    ActionWrapperList *container; //!< Container for the actions of the family.
    const PartialSafetyFactors *partial_safety_factors; //!< Partial safety factors for the actions in this wrapper.
    
    //! @brief Default constructor.
    ActionWrapper(ActionWrapperList *list= nullptr, const std::string &nmb_partial_safety_factors= "");
  public:
    const PartialSafetyFactors *getPartialSafetyFactors(void) const;
    void setPartialSafetyFactors(const std::string &);
    const ActionsFamily *getFamily(void) const;
    int getIndex(void) const;
    virtual std::string getName(void) const= 0;
    virtual bool notDeterminant(void) const= 0;

    virtual std::vector<const Action *> getWrappedActions(void) const= 0;
    virtual const Action *getAction(const size_t &) const= 0;
    virtual const ActionRelationships &getRelaciones(void) const= 0;
    virtual bool Incompatible(const ActionWrapper &f) const;
    //! @brief Return true if this actions is compatible with
    //! the argument one.
    inline bool Compatible(const ActionWrapper &aw) const
      { return !Incompatible(aw); }
    std::string incompatibleStringList(ActionWrapperList *) const;

    Action::map_descomp getComponents(void) const;
    boost::python::dict getComponentsPy(void) const;
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);    
   
    Variations getVariations(const bool &,const bool &) const;
    virtual Action getRepresentativeValue(const LeadingActionInfo &) const= 0;    
    Action getCombinationValue(const LeadingActionInfo &, const double &) const;    
    void Print(std::ostream &os) const;    
  };
  
bool incompatibles(const ActionWrapper &acc_i,const ActionWrapper &acc_j);
  
//! @brief Return the list of actions wrappers incompatibles with the one 
//! being passed as parameter.
template <class InputIterator>
std::deque<const ActionWrapper *> listaIncompatibles(const ActionWrapper *acc,InputIterator begin,InputIterator end)
  {
    std::deque<const ActionWrapper *> retval;
    for(InputIterator i=begin;i!=end;i++)
      {
        const ActionWrapper &data= *(*i).get();
        if(acc->Incompatible(data)) retval.push_back(&data);
      }
    return retval;
  }
  
std::ostream &operator<<(std::ostream &,const ActionWrapper &);

} //fin namespace nmb_acc.

#endif
