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
//SingleActionWrapper.cxx

#include "utility/load_combinations/actions/SingleActionWrapper.h"

//! @brief Default constructor.
cmb_acc::SingleActionWrapper::SingleActionWrapper(const std::string &n, const std::string &descrip, ActionWrapperList *list)
  : ActionWrapper(list),
    action(n, descrip, this)
   {}

//! @brief Default constructor.
cmb_acc::SingleActionWrapper::SingleActionWrapper(const Action &a, ActionWrapperList *list,const std::string &nmb_comb_factors, const std::string &nmb_partial_safety_factors)
  : ActionWrapper(list, nmb_partial_safety_factors),
    action(a, this, nmb_comb_factors)
  {}

//! @brief Return the action name.
std::string cmb_acc::SingleActionWrapper::getName(void) const
  { return action.getName(); }

//! @brief Return pointers to the actions wrapped in this object.
std::vector<const cmb_acc::Action *> cmb_acc::SingleActionWrapper::getWrappedActions(void) const
  {
    std::vector<const Action *> retval(1, &action);
    return retval;
  }
//! @brief Return true if this actions is not compatible with
//! the argument one.
bool cmb_acc::SingleActionWrapper::Incompatible(const ActionWrapper &other) const
  {
    bool retval= false;
    std::vector<const Action *> other_actions= other.getWrappedActions();
    for(std::vector<const Action *>::const_iterator i= other_actions.begin(); i!=other_actions.end(); i++)
      {
	const Action *other_action= *i;
	if(this->action.Incompatible(*other_action))
	  {
	    retval= true;
	    break; //No need to continue.
	  }
      }
    return retval;
  }

//! @brief Get the representative value of the action in the contest being
//! pased as parameter.
//! @param LeadingActionInfo: information about the combination leading action.
cmb_acc::Action cmb_acc::SingleActionWrapper::getRepresentativeValue(const LeadingActionInfo &linfo) const
  {
    const int index= getIndex(); //Get the index of the action wrapper.
    return action.getRepresentativeValue(linfo, index);
  }

//! @brief Return the relatioships with the other actions.
const cmb_acc::ActionRelationships &cmb_acc::SingleActionWrapper::getRelaciones(void) const
  { return action.getRelaciones(); }
