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
//LoadCombinationGenerator.cc

#include "LoadCombinationGenerator.h"

#include "LoadCombinations.h"

//! @brief Constructor.
cmb_acc::LoadCombinationGenerator::LoadCombinationGenerator(CommandEntity *owr)
  : CommandEntity(owr), combinations(nullptr) {}

//! @brief Insert the action being passed as parameter.
//! @param weighting: name of the weighting factors repository.
//! @param family: family to which the action belongs ("permanent", "variable", "seismic",...)
//! @param act: the action object itself.
//! @param combination_factors_name: name of the combination factors container.
//! @param partial_safety_factors_name: name of the partial safety factor container.
cmb_acc::ActionRValue &cmb_acc::LoadCombinationGenerator::insert(const std::string &weighting,const std::string &family,const Action &act,const std::string &combination_factors_name, const std::string &partial_safety_factors_name)
  { return action_weighting.insert(weighting,family,act,combination_factors_name,partial_safety_factors_name); }

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
