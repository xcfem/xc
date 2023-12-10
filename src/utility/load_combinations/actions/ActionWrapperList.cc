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
//ActionWrapperList.cxx

#include "ActionWrapperList.h"
#include "containers/ActionsFamily.h"
#include "utility/load_combinations/comb_analysis/Variation.h"
#include "utility/load_combinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"
// #include "utility/load_combinations/actions/factors/PartialSafetyFactors.h"
#include "utility/load_combinations/comb_analysis/LoadCombinationVector.h"

//! @brief Back inserter.
cmb_acc::ActionWrapper &cmb_acc::ActionWrapperList::push_back(const SingleActionWrapper &a)
  {
    base_pointer ptr= std::make_shared<SingleActionWrapper>(a);
    base_container::push_back(ptr);
    ActionWrapper &retval= *back().get();
    retval.set_owner(this);
    return retval;
  }

//! @brief Back inserter.
cmb_acc::ActionWrapper &cmb_acc::ActionWrapperList::push_back(const GroupActionWrapper &a)
  {
    base_pointer ptr= std::make_shared<GroupActionWrapper>(a);
    base_container::push_back(ptr);
    ActionWrapper &retval= *back().get();
    retval.set_owner(this);
    return retval;
  }

//! @brief Insert the action being passed as parameter and sets its combination
//! and partial safety factors.
//! @param a: Action object to insert.
//! @param combination_factors_name: name of the combination factors that
//!                                  correspond to the action.
//! @param partial_safety_factors_name: name of the partial safety factors
//!                                     that correspond to the action.
cmb_acc::ActionWrapper &cmb_acc::ActionWrapperList::insert(const Action &a,const std::string &combination_factors_name,const std::string &partial_safety_factors_name)
  {
    SingleActionWrapper acc(a,this,combination_factors_name,partial_safety_factors_name);
    return push_back(acc);
  }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param actions: Vector of action objects to insert.
//! @param combination_factors_names: vector of names of the combination factors
//!                                  that correspond to the action.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::ActionWrapperList::insertGroup(const std::vector<Action> &actions, const std::vector<std::string> &combination_factors_names, const std::string &partial_safety_factors_name)
  {
    GroupActionWrapper accGrp(actions,this,combination_factors_names,partial_safety_factors_name);
    return push_back(accGrp);
  }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param actionTuples: list of (action, combination_factors_name) tuples.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::ActionWrapperList::insertGroupPy(const boost::python::list &actionTuples, const std::string &partial_safety_factors_name)
  {
    // Extract values from Python list.
    auto [actionLst, combFactorsNameLst] = extract_action_tuples(actionTuples);
    // Call C++ counterpart.
    return insertGroup(actionLst,combFactorsNameLst,partial_safety_factors_name);
  }

//! @brief Build the combination of actions defined by the variation argument.
//!
//! @param v: Variation to build.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::Action cmb_acc::ActionWrapperList::buildCombination(const Variation &var, const LeadingActionInfo &lai) const
  {
    const size_t num_acciones= size();
    Action retval= Action::NULA(); //Initialize to zero.
    for(size_t j=0;j<num_acciones;j++)
      {
        retval+= (*this)[j].get()->getCombinationValue(lai,var[j]);
      }
    return retval;
  }

//! @brief Return the index of the argument in the list.
int cmb_acc::ActionWrapperList::getIndex(const ActionWrapper *ptr) const
  {
    const size_t num_acciones= size();
    int retval= -1;
    for(size_t j=0;j<num_acciones;j++)
      {
	const ActionWrapper *tmp= (*this)[j].get();
	if(ptr==tmp)
	  {
	    retval= j;
	    break;
	  }
      }
    return retval;
  }

//! @brief Return the family that owns this list.
const cmb_acc::ActionsFamily *cmb_acc::ActionWrapperList::getFamily(void) const
  { return dynamic_cast<const ActionsFamily *>(Owner()); }

//! @brief Return a pointer to the table of combination factors.
const cmb_acc::CombinationFactorsMap *cmb_acc::ActionWrapperList::getPtrCombinationFactors(void) const
  {
    const ActionsFamily *tmp= this->getFamily();
    if(tmp)
      return tmp->getPtrCombinationFactors();
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; owner not found." << std::endl;
        return nullptr;
      }
  }

//! @brief Return a pointer to the table of coeficientes de simultaneidad.
const cmb_acc::PartialSafetyFactorsMap *cmb_acc::ActionWrapperList::getPtrPartialSafetyFactors(void) const
  {
    const ActionsFamily *tmp= this->getFamily();
    if(tmp)
      return tmp->getPtrPartialSafetyFactors();
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; owner not found." << std::endl;
        return nullptr;
      }
  }

//! @brief Compute the variations that can be formed with the actions of the list.
//!
//! @param uls: True if it's an ultimate limit state.
//! @param sit_accidental: true if it's an accidental or seismic situation.
//! @param leadingActionIndex: index of the leading action (-1 if no one is).
cmb_acc::Variations cmb_acc::ActionWrapperList::computeVariations(const bool &uls,const bool &sit_accidental,const int &leadingActionIndex) const
  {
    
    const_iterator i= begin();
    Variations v= (*i).get()->getVariations(uls,sit_accidental);
    Variations retval(v);
    i++;
    for(;i!=end();i++)
      {
	v= (*i).get()->getVariations(uls,sit_accidental);
	retval= Variations::prod_cartesiano(retval,v); //Order is important (LCPT 4/08/2018) 
      }
    if(retval.empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; Warning! no combinations computed." << std::endl;
    return retval;
  }

//! @brief Return the load combinations for a leading action.
//!
//! @param uls: Verdadero si se trata de un estado límite último.
//! @param sit_accidental: Verdadero si estamos en situación accidental.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::LoadCombinationVector cmb_acc::ActionWrapperList::getCombinations(const bool &uls,const bool &sit_accidental, const LeadingActionInfo &leadingActionInfo) const
  {
    const int leadingActionIndex= leadingActionInfo.getLeadingActionIndex();
    Variations var= computeVariations(uls,sit_accidental,leadingActionIndex);
    const size_t num_variations= var.size();
    LoadCombinationVector retval(num_variations);
    ActionWrapperList *this_no_const= const_cast<ActionWrapperList *>(this);
    for(size_t i=0;i<num_variations;i++)
      {
        const Variation &v_i= var[i];
        if(leadingActionIndex>=0)//There is a leading action.
          {
	    if(v_i.at(leadingActionIndex)>0.0)
              retval[i]= buildCombination(v_i,leadingActionInfo);
	    else
	      retval[i]= Action::NULA();
          }
	else
          retval[i]= buildCombination(v_i,leadingActionInfo);
        retval[i].set_owner(this_no_const);
      }
    retval= retval.GetNoNulas(1e-3);
    retval= get_compatibles(retval); //Filtramos las que contienen incompatible actions.
    retval= retval.GetDistintas(); //Eliminamos las repetidas.
    return retval;
  }

//! @brief Return the load combinations for a leading action.
//!
//! @param uls: Verdadero si se trata de un estado límite último.
//! @param sit_accidental: Verdadero si estamos en situación accidental.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::LoadCombinationVector cmb_acc::ActionWrapperList::getCombinationsWhenLeading(const bool &uls,const bool &sit_accidental, const bool &sit_sismica,const short int &v) const
  {
    const size_t nq= size();
    LoadCombinationVector retval;
    for(size_t i=0;i<nq;i++) //i: index of the leading action.
      {
	const ActionWrapper *leadingAction= (*this)[i].get();
	if(leadingAction->notDeterminant()) // Can't be leading action.
	  {
	    if(verbosity>1)
	      std::clog << std::endl
			<< "    Action: "
			<< i << " cannot lead. Skipping...";
	  }
	else
	  {
	    if(verbosity>1)
	      std::clog << std::endl
			<< "    Computing combinations of variable actions with leading action: "
			<< i << " ...";
	    LeadingActionInfo lci(i,v-1,v); //Leading action with representative value v
					    //rest of actions with representative value v-1.
	    if(sit_accidental)
	      lci= LeadingActionInfo(i,1,2); //Leading action with frequent value
					     //rest of actions with quasi-permanent value.
	    if(sit_sismica)
	      lci= LeadingActionInfo(-1,2,2); //All actions with quasi-permanent value.
	    LoadCombinationVector temp= getCombinations(uls,sit_accidental,lci); 
	    retval= LoadCombinationVector::Concat(retval,temp,Action::zero);
	  }
	if(verbosity>1)
	  std::clog << "done." << std::endl;
      }
    return retval;
  }

//! @brief Print stuff.
void cmb_acc::ActionWrapperList::Print(std::ostream &os) const
  {
    for(const_iterator i=begin();i!=end();i++)
      os << *i << ' ';
  }

//! @brief Output operator.
std::ostream &cmb_acc::operator<<(std::ostream &os,const ActionWrapperList &lvr)
  {
    lvr.Print(os);
    return os;
  }

//! @brief Extract the (action, combination_factors_name) pairs from the
//! argument list.
//! @param actionTuples: list of (action, combination_factors_name) tuples.
std::tuple<std::vector<cmb_acc::Action>, std::vector<std::string> > cmb_acc::extract_action_tuples(const boost::python::list &actionTuples)
  {
    const size_t sz= len(actionTuples);
    std::vector<Action> actionLst(sz);
    std::vector<std::string> combFactorsNameLst(sz);
    for(size_t i= 0;i<sz;++i)
      {
	boost::python::tuple tuple= boost::python::extract<boost::python::tuple>(actionTuples[i]);
        const size_t tuple_size= boost::python::len(tuple);
	if(tuple_size != 2)
	  {
	    std::cerr << __FUNCTION__
	              << "; expected a tuple of 2 values get one with: "
	              << tuple_size << " values." << std::endl;
	    exit(-1);
	  } 
        const Action extractedAction= boost::python::extract<Action>(tuple[0]);
	actionLst[i]= extractedAction;
        const std::string extractedString= boost::python::extract<std::string>(tuple[1]);
	combFactorsNameLst[i]= extractedString;
	
      }
    return {actionLst,combFactorsNameLst};
  }
