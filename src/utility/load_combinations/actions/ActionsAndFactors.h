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
//ActionsAndFactors.h
//Store all the families of actions for the structure of structural element.

#ifndef ACTIONSANDFACTORS_H
#define ACTIONSANDFACTORS_H

#include "ActionContainer.h"
#include "utility/load_combinations/factors/Factors.h"

namespace cmb_acc{
//! @ingroup CMBACC
//
//! @brief Container for all the action families.
class ActionsAndFactors: public ActionContainer
  {
  private:
    Factors factors; //!< Factors to use to build combinations.

  protected:
    friend class ActionWeightingMap;
  public:
    ActionsAndFactors(const Factors &factors= Factors());

    const Factors &getFactors(void) const
      { return factors; }
  };
} //end namespace cmb_acc.

#endif
