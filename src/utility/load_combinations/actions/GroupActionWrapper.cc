//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GroupActionWrapper.cxx

#include "utility/load_combinations/actions/GroupActionWrapper.h"

//! @brief Default constructor.
cmb_acc::GroupActionWrapper::GroupActionWrapper(const std::string &n, const std::string &descrip, ActionWrapperList *list)
  : ActionWrapper(list),
    actions(1, ActionRepresentativeValues(n, descrip, this))
   {}

//! @brief Default constructor.
cmb_acc::GroupActionWrapper::GroupActionWrapper(const Action &a, ActionWrapperList *list,const std::string &nmb_comb_factors, const std::string &nmb_partial_safety_factors)
  : ActionWrapper(list, nmb_partial_safety_factors),
    actions(1, ActionRepresentativeValues(a, this, nmb_comb_factors))
  {}

//! @brief Return the action name.
std::string cmb_acc::GroupActionWrapper::getName(void) const
  {
    std::string retval= "";
    for(const_iterator i= actions.begin(); i!=actions.end(); i++)
      retval+= (*i).getName();
    return retval;
  }

//! @brief Return pointers to the actions wrapped in this object.
std::vector<const cmb_acc::Action *> cmb_acc::GroupActionWrapper::getWrappedActions(void) const
  {
    const size_t sz= actions.size();
    std::vector<const Action *> retval(sz, nullptr);
    size_t count= 0;
    for(const_iterator i= actions.begin(); i!=actions.end(); i++, count++)
      retval[count]= &(*i);
    return retval;
  }

//! @brief Get the representative value of the action in the contest being
//! pased as parameter.
//! @param LeadingActionInfo: information about the combination leading action.
cmb_acc::Action cmb_acc::GroupActionWrapper::getRepresentativeValue(const LeadingActionInfo &linfo) const
  {
    const int index= getIndex(); //Get the index of the action wrapper.
    Action tmp;
    for(const_iterator i= actions.begin(); i!=actions.end(); i++)
      {
	const ActionRepresentativeValues &a= *i;
	tmp+= a.getRepresentativeValue(linfo, index);
      }
    return tmp;
  }

//! @brief Return the relatioships with the other actions.
const cmb_acc::ActionRelationships &cmb_acc::GroupActionWrapper::getRelaciones(void) const
  {
    assert(!actions.empty());
    return actions[0].getRelaciones();
  }
