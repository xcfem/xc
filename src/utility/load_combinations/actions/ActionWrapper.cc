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
//ActionWrapper.cxx

#include "utility/load_combinations/actions/ActionWrapper.h"
#include "utility/load_combinations/actions/ActionWrapperList.h"
#include "utility/load_combinations/actions/containers/ActionsFamily.h"
#include "utility/load_combinations/actions/factors/PartialSafetyFactors.h"
#include "utility/load_combinations/actions/factors/PartialSafetyFactorsMap.h"
#include "utility/load_combinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
cmb_acc::ActionWrapper::ActionWrapper(ActionWrapperList *list, const std::string &nmb_partial_safety_factors)
  : container(list),
    partial_safety_factors(nullptr)
  {
    if(not nmb_partial_safety_factors.empty())
      setPartialSafetyFactors(nmb_partial_safety_factors);
  }

//! @brief Return the partial safety factors.
const cmb_acc::PartialSafetyFactors *cmb_acc::ActionWrapper::getPartialSafetyFactors(void) const
  {
    if(!partial_safety_factors)
       std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: action " << getName()
	        << ": partial safety factors not set."
	        << Color::def << std::endl;
    return partial_safety_factors;
  }

//! @brief Set the partial safety factors.
void cmb_acc::ActionWrapper::setPartialSafetyFactors(const std::string &nmb_factors)
  {
    if(!nmb_factors.empty())
      {
        const PartialSafetyFactors *tmp=nullptr;
        if(container)
          tmp= container->getPtrPartialSafetyFactors()->getPtrCoefs(nmb_factors);
        else
           std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; ERROR: pointer to actions container not set."
		    << Color::def << std::endl;
        if(tmp)
           partial_safety_factors= tmp;
	else
	  {
             std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; ERROR: combination factors with name: '"
	  	      << nmb_factors
	              << "' not found."
		      << Color::def << std::endl;
	    exit(-1);
	  }
      }
  }

//! @brief Return the index of the action in its container.
int cmb_acc::ActionWrapper::getIndex(void) const
  {
    int retval= -1;
    if(container)
      retval= container->getIndex(this);
    return retval;
  }
//! @brief Return the family that owns this object.
const cmb_acc::ActionsFamily *cmb_acc::ActionWrapper::getFamily(void) const
  {
    const ActionsFamily *retval= nullptr;
    if(container)
      retval= container->getFamily();
    return retval;
  }

//! @brief When the wrapped actions decomposition.
cmb_acc::Action::map_descomp cmb_acc::ActionWrapper::getComponents(void) const
  {
    Action::map_descomp retval;
    std::vector<const Action *> this_actions= this->getWrappedActions();
    for(std::vector<const Action *>::const_iterator i= this_actions.begin(); i!=this_actions.end(); i++)
      {
        const Action *this_action= *i;
	Action::map_descomp tmp= this_action->getComponents();
	retval.merge(tmp);
      }
    return retval;
  }

//! @brief Return the group components in a Python dictionary.
boost::python::dict cmb_acc::ActionWrapper::getComponentsPy(void) const
  {
    boost::python::dict retval; 
    const Action::map_descomp descomp= getComponents();
    for(Action::map_descomp::const_iterator i= descomp.begin(); i!= descomp.end(); i++)
      {
	const std::string key= (*i).first;
	const double factor= (*i).second;
	retval[key]= factor;
      }
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict cmb_acc::ActionWrapper::getPyDict(void) const
  {
    boost::python::dict retval= EntityWithOwner::getPyDict();
    std::vector<const Action *> this_actions= this->getWrappedActions();
    // Populate action dictionary.
    boost::python::dict actionDict;
    for(std::vector<const Action *>::const_iterator i= this_actions.begin(); i!=this_actions.end(); i++)
      {
	const Action *this_action= *i;
	const std::string &actionName= this_action->getName();
	actionDict[actionName]= this_action->getPyDict();
      }
    retval["actions"]= actionDict;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void cmb_acc::ActionWrapper::setPyDict(const boost::python::dict &d)
  {
     std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
	      << Color::def << std::endl;
    // EntityWithOwner::setPyDict(d);
    // boost::python::dict actionDict= boost::python::extract<std::string>(d["actions"]);
    // auto items= actionDict.items();
    // for (auto it = stl_input_iterator<tuple>(items); it != stl_input_iterator<tuple>(); ++it)
    //   {
    //     tuple kv = *it;
    // 	std::string key= kv[0];
    // 	boost::python::dict values= kv[1];
    // 	Action newAction;
    // 	newAction.setPyDict(values);
        
    //   }
  }

//! @brief Compute the variations that can be formed with this action.
//!
//! @param uls: True if it's an ultimate limit state.
//! @param sit_accidental: true if it's an accidental or seismic situation.
cmb_acc::Variations cmb_acc::ActionWrapper::getVariations(const bool &uls,const bool &sit_accidental) const
  {
    Variation v= getPartialSafetyFactors()->getVariation(uls,sit_accidental);
    return Variations::first_combination(v);
  }

//! @brief Return the combination value for the action.
//! @param leadingActioInfo: Information about the leading action.
//! @param psf: partial safety factor.
cmb_acc::Action cmb_acc::ActionWrapper::getCombinationValue(const LeadingActionInfo &lai, const double &psf) const
  {
    return psf*getRepresentativeValue(lai);
  }

//! @brief Print stuff.
void cmb_acc::ActionWrapper::Print(std::ostream &os) const
  {
    const ActionsFamily *family= this->getFamily();
    if(family)
      os << "actions family: " << family->getName();
    std::vector<const Action *> this_actions= this->getWrappedActions();
    for(std::vector<const Action *>::const_iterator i= this_actions.begin(); i!=this_actions.end(); i++)
      {
        const Action *this_action= *i;
        os << " " << *this_action;
      }
  }

bool cmb_acc::incompatibles(const ActionWrapper &acc_i,const ActionWrapper &acc_j)
  {
    bool retval= false;
    if(&acc_i!=&acc_j) //Una acción nunca es incompatible consigo misma.
      {
        if(acc_i.Incompatible(acc_j))
          retval= true;
        else
          if(acc_j.Incompatible(acc_i)) retval= true;
      }
    return retval;
  }

//! @brief Return true if this actions are not compatible with
//! those in the argument wrapper.
bool cmb_acc::ActionWrapper::Incompatible(const ActionWrapper &other) const
  {
    bool retval= false;
    std::vector<const Action *> this_actions= this->getWrappedActions();
    std::vector<const Action *> other_actions= other.getWrappedActions();
    for(std::vector<const Action *>::const_iterator i= this_actions.begin(); i!=this_actions.end(); i++)
      {
        const Action *this_action= *i;
	for(std::vector<const Action *>::const_iterator j= other_actions.begin(); j!=other_actions.end(); j++)
	  {
	    const Action *other_action= *j;
	    if(this_action->Incompatible(*other_action))
	      {
		retval= true;
		break; //No need to continue.
	      }
	  }
      }
    return retval;
  }

std::string cmb_acc::ActionWrapper::incompatibleStringList(ActionWrapperList *awl) const
  {
    std::string retval;
    if(awl)
      {
        std::deque<const cmb_acc::ActionWrapper *> incomp= listaIncompatibles(this,awl->begin(),awl->end());
        if(!incomp.empty())
          retval= actionPtrsNames(incomp.begin(),incomp.end());
      }
    else
       std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; null pointer to the wrapper list."
		<< Color::def << std::endl;
    return retval; 
  }
//! @brief Output operator.
std::ostream &cmb_acc::operator<<(std::ostream &os,const ActionWrapper &aw)
  {
    aw.Print(os);
    return os;
  }
