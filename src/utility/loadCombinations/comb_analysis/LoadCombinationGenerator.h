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
//LoadCombinationGenerator.h

#ifndef LOADCOMBINATIONGENERATOR_H
#define LOADCOMBINATIONGENERATOR_H

#include "ActionWeightingMap.h"

namespace cmb_acc{

class LoadCombinations;

//! @ingroup CMBACC
//
//! @brief Objeto that generates and manages the combinations.
class LoadCombinationGenerator: public CommandEntity
  {
  private:
    ActionWeightingMap action_weighting;
    LoadCombinations *combinations;
  public:
    LoadCombinationGenerator(CommandEntity *owr= nullptr);

    ActionRValue &insert(const std::string &,const std::string &,const Action &,const std::string &combination_factors_name="",const std::string &partial_safety_factors_name="");
    void genera(void);
    inline ActionWeightingMap &getActionWeighting(void)
      { return action_weighting; }
    inline void setActionWeighting(const ActionWeightingMap &pa)
      { action_weighting= pa; }
    inline LoadCombinations *getLoadCombinations(void)
      { return combinations; }
  };
} //fin namespace nmb_acc.


#endif
