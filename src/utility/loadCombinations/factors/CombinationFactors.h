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
//CombinationFactors.hxx
//Combination factors for an action.

#ifndef COMBINATIONFACTORS_H
#define COMBINATIONFACTORS_H

#include "utility/kernel/CommandEntity.h"

namespace cmb_acc{

//! @ingroup CMBACC
//
//! @brief Coeficientes de simultaneidad de una acción.
class CombinationFactors: public CommandEntity
  {
  private:
    float psi_0; //!< To compute the combination value.
    float psi_1; //!< To compute the frequent value.
    float psi_2; //!< To compute the quasi-permanent value.
  public:
    //! @brief Default constructor.
    CombinationFactors(const float &p0= 1.0, const float &p1= 1.0, const float &p2= 1.0)
      : CommandEntity(),psi_0(p0), psi_1(p1), psi_2(p2) {}
    //! @brief Copy constructor
    const float &getCombinationFactor(short int r) const;
    void Print(std::ostream &os) const;
  };

 inline std::ostream &operator<<(std::ostream &os, const CombinationFactors &cf)
  {
    cf.Print(os);
    return os;
  }

} //fin namespace nmb_acc.

#endif
