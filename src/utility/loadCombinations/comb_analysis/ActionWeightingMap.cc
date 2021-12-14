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
//ActionWeightingMap.cc

#include "ActionWeightingMap.h"
#include "utility/loadCombinations/actions/ActionsAndFactors.h"
#include "LoadCombinations.h"

//! @brief Return true if the object is found.
bool cmb_acc::ActionWeightingMap::exists(const std::string &nmb) const
  { return (actions_and_factors.find(nmb)!=actions_and_factors.end()); }

//! @brief Return a pointer to the object with argument name.
cmb_acc::ActionsAndFactors *cmb_acc::ActionWeightingMap::findByName(const std::string &name)
  {
    if(exists(name))
      return actions_and_factors[name];
    else
      return nullptr;
  }

//! @brief Return a pointer to the object with that name.
const cmb_acc::ActionsAndFactors *cmb_acc::ActionWeightingMap::findByName(const std::string &name) const
  {
    const_iterator i= actions_and_factors.find(name);
    if(i!= actions_and_factors.end())
      return (*i).second;
    else
      return nullptr;
  }

//! @brief Creates a new object with the argument name.
cmb_acc::ActionsAndFactors *cmb_acc::ActionWeightingMap::findOrCreate(const std::string &name,const Factors &factors)
  {
    ActionsAndFactors *tmp =nullptr;
    if(!exists(name)) //new object.
      {
        tmp= new ActionsAndFactors(factors);
        actions_and_factors[name]= tmp;
      }
    else //already exists
      tmp= findByName(name);
    return tmp;
  }

//! @brief Default constructor.
cmb_acc::ActionWeightingMap::ActionWeightingMap(void)
  : CommandEntity() {}

//! @brief Copy constructor(NO COPIA LAS ACTIONS_AND_FACTORS).
cmb_acc::ActionWeightingMap::ActionWeightingMap(const ActionWeightingMap &other)
  : CommandEntity(other)
  {
    copy(other.actions_and_factors);
  }

//! @brief Assignment operator (NO COPIA LAS ACTIONS_AND_FACTORS).
cmb_acc::ActionWeightingMap &cmb_acc::ActionWeightingMap::operator=(const ActionWeightingMap &other)
  {
    CommandEntity::operator=(other);
    copy(other.actions_and_factors);
    return *this;
  }

//! @brief Define una ponderación de acciones.
cmb_acc::ActionsAndFactors *cmb_acc::ActionWeightingMap::create(const std::string &nmb_ponderacion,const Factors &factors)
  {
    ActionsAndFactors *retval= findOrCreate(nmb_ponderacion,factors);
    if(retval) retval->set_owner(this);
    return retval;
  }

//! @brief Insert the action being passed as parameter.
cmb_acc::ActionRValue &cmb_acc::ActionWeightingMap::insert(const std::string &pond,const std::string &family,const Action &acc,const std::string &combination_factors_name,const std::string &partial_safety_factors_name)
  {
    ActionsAndFactors *ponderacion_ptr= findByName(pond);
    if(!ponderacion_ptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; weighting: '"
                  << pond << "' not found.\n";
      }
    return ponderacion_ptr->insert(family,acc,combination_factors_name,partial_safety_factors_name);
  }

//! @brief Borra todas las actions_and_factors definidos.
void cmb_acc::ActionWeightingMap::clear(void)
  {
    for(iterator i= actions_and_factors.begin();i!=actions_and_factors.end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    actions_and_factors.clear();
  }

//! @brief Copy the objects from the argument.
void cmb_acc::ActionWeightingMap::copy(const map_actions_and_factors &a_and_f)
  {
    clear();
    for(const_iterator i= a_and_f.begin();i!=a_and_f.end();i++)
      {
        const std::string name= (*i).first;
        const ActionsAndFactors *a= (*i).second;
        assert(a);
        actions_and_factors[name]= new ActionsAndFactors(*a);
      }
  }

cmb_acc::ActionWeightingMap::~ActionWeightingMap(void)
  {
    //actions_and_factors.clear();
    clear();
  }

//! @brief Return el número de actions_and_factors de todas las actions_and_factors.
size_t cmb_acc::ActionWeightingMap::size(void) const
  { return actions_and_factors.size(); }

//! brief Return verdadero si las actions_and_factors estan vacías.
bool cmb_acc::ActionWeightingMap::empty(void) const
  { return actions_and_factors.empty(); }

cmb_acc::ActionWeightingMap::iterator cmb_acc::ActionWeightingMap::begin(void)
  { return actions_and_factors.begin(); }
cmb_acc::ActionWeightingMap::const_iterator cmb_acc::ActionWeightingMap::begin(void) const
  { return actions_and_factors.begin(); }
cmb_acc::ActionWeightingMap::iterator cmb_acc::ActionWeightingMap::end(void)
  { return actions_and_factors.end(); }
cmb_acc::ActionWeightingMap::const_iterator cmb_acc::ActionWeightingMap::end(void) const
  { return actions_and_factors.end(); }

//! @brief Return the names of the action weightings.
boost::python::list cmb_acc::ActionWeightingMap::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;    
  }

//! @bried Return the combinations that correspond to all actions_and_factors.
cmb_acc::LoadCombinations cmb_acc::ActionWeightingMap::getLoadCombinations(void)
  {
    LoadCombinations retval;
    for(const_iterator i= actions_and_factors.begin();i!=actions_and_factors.end();i++)
      retval.Concat(LoadCombinations(*(*i).second));
    return retval;
  }

