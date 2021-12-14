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
//CombinationFactorsMap.h
//Contenedor de coeficientes de simultaneidad de acciones.

#ifndef COMBINATIONFACTORSMAP_H
#define COMBINATIONFACTORSMAP_H

#include "utility/kernel/CommandEntity.h"
#include "CombinationFactors.h"
#include "FactorsMap.h"


namespace cmb_acc {

class LoadCombinationVector;

//! @ingroup CMBACC
//
//! @brief Contenedor de coeficientes de simultaneidad de acciones.
class CombinationFactorsMap: public FactorsMap<CombinationFactors>
  {
  private:
    friend class ActionsAndFactors;
  public:
    CombinationFactorsMap(void);
  };
} // fin namespace cmb_acc

#endif
