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
//Variation.hxx

#ifndef VARIATION_HXX
#define VARIATION_HXX

#include <vector>
#include <iostream>

namespace cmb_acc{

class ActionRValueList;

//! @ingroup CMBACC
//
//! @brief Vector that stores the factors of the linear combination of actions.
class Variation: public std::vector<double>
  {
    void print(std::ostream &os) const;
  public:
    //! @brief Constructor.
    Variation(const size_t &sz=0,const double &valor=0.0)
      : std::vector<double>(sz,valor) {}
    static Variation concat(const Variation &a,const Variation &b);
    bool compatible(const ActionRValueList &lvr);
    friend std::ostream &operator<<(std::ostream &os,const Variation &v);
  };

std::ostream &operator<<(std::ostream &os,const Variation &v);
} //fin namespace nmb_acc.

#endif
