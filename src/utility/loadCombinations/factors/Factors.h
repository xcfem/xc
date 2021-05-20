// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//Factors.h
//Container for the partial safety factors and the combination factors.

#ifndef FACTORS_H
#define FACTORS_H

#include "CombinationFactorsMap.h"
#include "PartialSafetyFactorsMap.h"

namespace cmb_acc{
//! @ingroup CMBACC
//
//! @brief Almacena todas las familias de acciones.
class Factors: public CommandEntity
  {
  private:
    PartialSafetyFactorsMap partial_safety_factors; //!< Partial safety factors.
    CombinationFactorsMap combination_factors; //!< Combination factors.

  public:
    Factors(const PartialSafetyFactorsMap &psf= PartialSafetyFactorsMap(), const CombinationFactorsMap &cf= CombinationFactorsMap());

    const CombinationFactorsMap *getPtrCombinationFactors(void) const
      { return &combination_factors; }
    const CombinationFactorsMap &getCombinationFactors(void) const
      { return combination_factors; }
    const PartialSafetyFactorsMap *getPtrPartialSafetyFactors(void) const
      { return &partial_safety_factors; }
    const PartialSafetyFactorsMap &getPartialSafetyFactors(void) const
      { return partial_safety_factors; }
  };
} //fin namespace nmb_acc.

#endif
