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
//LoadCombinationGenerator.cc

#include "LoadCombinationGenerator.h"
#include "LoadCombinations.h"
#include "utility/load_combinations/actions/ActionWrapperList.h"

//! @brief Constructor.
cmb_acc::LoadCombinationGenerator::LoadCombinationGenerator(CommandEntity *owr)
  : CommandEntity(owr), combinations(nullptr) {}

//! @brief Insert the action being passed as parameter.
//! @param weighting: name of the weighting factors repository.
//! @param family: family to which the action belongs ("permanent", "variable", "seismic",...)
//! @param act: the action object itself.
//! @param combination_factors_name: name of the combination factors container.
//! @param partial_safety_factors_name: name of the partial safety factor container.
cmb_acc::ActionWrapper &cmb_acc::LoadCombinationGenerator::insert(const std::string &weighting,const std::string &family,const Action &act,const std::string &combination_factors_name, const std::string &partial_safety_factors_name)
  { return action_weighting.insert(weighting,family,act,combination_factors_name,partial_safety_factors_name); }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param weighting: name of the weighting factors repository.
//! @param familyName: name of the family to which the action belongs.
//! @param actions: Vector of action objects to insert.
//! @param combination_factors_names: vector of names of the combination factors
//!                                  that correspond to the action.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::LoadCombinationGenerator::insertGroup(const std::string &weighting, const std::string &familyName, const std::vector<Action> &actions, const std::vector<std::string> &combination_factors_names, const std::string &partial_safety_factors_name)
  {
    return action_weighting.insertGroup(weighting,familyName,actions,combination_factors_names,partial_safety_factors_name);
  }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param weighting: name of the weighting factors repository.
//! @param familyName: name of the family to which the action belongs.
//! @param actionTuples: list of (action, combination_factors_name) tuples.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::LoadCombinationGenerator::insertGroupPy(const std::string &weighting, const std::string &familyName, const boost::python::list &actionTuples, const std::string &partial_safety_factors_name)
  {
    // Extract values from Python list.
    auto [actionLst, combFactorsNameLst] = extract_action_tuples(actionTuples);
    // Call C++ counterpart.
    return insertGroup(weighting,familyName,actionLst,combFactorsNameLst,partial_safety_factors_name);
  }

//! @brief Generation of combinations
void cmb_acc::LoadCombinationGenerator::genera(void)
  {
    if(combinations) //Already generated.
      {
        delete combinations;
        combinations= nullptr;
      }
    else
      combinations= new LoadCombinations(action_weighting.getLoadCombinations());
  }
