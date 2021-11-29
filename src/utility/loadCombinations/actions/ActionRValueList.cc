//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//ActionRValueList.cxx

#include "ActionRValueList.h"
#include "ActionsFamily.h"
#include "utility/loadCombinations/comb_analysis/Variation.h"
#include "utility/loadCombinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"
#include "utility/loadCombinations/factors/PartialSafetyFactors.h"
#include "utility/loadCombinations/comb_analysis/LoadCombinationVector.h"

cmb_acc::ActionRValue &cmb_acc::ActionRValueList::push_back(const ActionRValue &a)
  {
    std::deque<ActionRValue>::push_back(a);
    ActionRValue &retval= back();
    return retval;
  }

//! @brief Insert the action being passed as parameter.
cmb_acc::ActionRValue &cmb_acc::ActionRValueList::insert(const Action &a,const std::string &combination_factors_name,const std::string &partial_safety_factors_name)
  {
    ActionRValue acc(a,this,combination_factors_name,partial_safety_factors_name);
    acc.set_owner(this);
    return push_back(acc);
  }

//! @brief Build the combination of actions defined by the variation argument.
//!
//! @param v: Variation to build.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::Action cmb_acc::ActionRValueList::buildCombination(const Variation &var, const LeadingActionInfo &lai) const
  {
    const size_t num_acciones= size();
    Action retval= Action::NULA(); //Initialize to zero.
    for(size_t j=0;j<num_acciones;j++)
      {
        retval+= (*this)[j].getCombinationValue(lai,var[j]);
      }
    return retval;
  }

//! @brief Return the index of the argument in the list.
int cmb_acc::ActionRValueList::getIndex(const ActionRValue *ptr) const
  {
    const size_t num_acciones= size();
    int retval= -1;
    for(size_t j=0;j<num_acciones;j++)
      {
	const ActionRValue *tmp= &(*this)[j];
	if(ptr==tmp)
	  {
	    retval= j;
	    break;
	  }
      }
    return retval;
  }

//! @brief Return a pointer to the table of coeficientes de simultaneidad.
const cmb_acc::CombinationFactorsMap *cmb_acc::ActionRValueList::getPtrCombinationFactors(void) const
  {
    const ActionsFamily *tmp= dynamic_cast<const ActionsFamily *>(Owner());
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
const cmb_acc::PartialSafetyFactorsMap *cmb_acc::ActionRValueList::getPtrPartialSafetyFactors(void) const
  {
    const ActionsFamily *tmp= dynamic_cast<const ActionsFamily *>(Owner());
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
cmb_acc::Variations cmb_acc::ActionRValueList::computeVariations(const bool &uls,const bool &sit_accidental,const int &leadingActionIndex) const
  {
    
    const_iterator i= begin();
    Variations v= i->getVariations(uls,sit_accidental);
    Variations retval(v);
    i++;
    for(;i!=end();i++)
      {
	v= i->getVariations(uls,sit_accidental);
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
cmb_acc::LoadCombinationVector cmb_acc::ActionRValueList::getCombinations(const bool &uls,const bool &sit_accidental, const LeadingActionInfo &leadingActionInfo) const
  {
    const int leadingActionIndex= leadingActionInfo.getLeadingActionIndex();
    Variations var= computeVariations(uls,sit_accidental,leadingActionIndex);
    const size_t num_variations= var.size();
    LoadCombinationVector retval(num_variations);
    ActionRValueList *this_no_const= const_cast<ActionRValueList *>(this);
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
cmb_acc::LoadCombinationVector cmb_acc::ActionRValueList::getCombinationsWhenLeading(const bool &uls,const bool &sit_accidental, const bool &sit_sismica,const short int &v) const
  {
    const size_t nq= size();
    LoadCombinationVector retval;
    for(size_t i=0;i<nq;i++) //i: index of the leading action.
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
	if(verbosity>1)
	  std::clog << "done." << std::endl;
      }
    return retval;
  }

//! @brief Print stuff.
void cmb_acc::ActionRValueList::Print(std::ostream &os) const
  {
    for(const_iterator i=begin();i!=end();i++)
      os << *i << ' ';
  }

//! @brief Operador salida.
std::ostream &cmb_acc::operator<<(std::ostream &os,const ActionRValueList &lvr)
  {
    lvr.Print(os);
    return os;
  }
