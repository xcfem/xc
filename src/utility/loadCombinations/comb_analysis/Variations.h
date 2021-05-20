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
//Variations.hxx
//Conjunto de las posibles variations de dos elementos
//(partial_safety_factors_fav y partial_safety_factors_desfav) tomados de n en n.

#ifndef VARIATIONS_HXX
#define VARIATIONS_HXX

#include "Variation.h"

namespace cmb_acc{

class ActionRValueList;

//! @ingroup CMBACC
//
//! @brief Conjunto de las posibles variations de dos elementos (partial_safety_factors_fav y partial_safety_factors_desfav) tomados de n en n.
class Variations: public std::vector<Variation>
  {
    void print(std::ostream &os) const;
  public:
    static Variations first_combination(const Variation &v);
    static Variations prod_cartesiano(const Variations &a,const Variations &b);

    Variations(const size_t &sz=0,const Variation &v=Variation());
    friend std::ostream &operator<<(std::ostream &os,const Variations &vs);
  };

    std::ostream &operator<<(std::ostream &os,const Variations &vs);
} //fin namespace nmb_acc.

#endif
