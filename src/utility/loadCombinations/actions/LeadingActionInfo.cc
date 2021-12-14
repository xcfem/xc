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
//LeadingActionInfo.cxx

#include "LeadingActionInfo.h"
#include "Action.h"

cmb_acc::LeadingActionInfo::LeadingActionInfo(const short int &i, const short int &lrv, const short int &grv)
  : CommandEntity(),index(i), leading_representative_value_index(lrv),
    general_representative_value_index(grv) {}

//! @brief Print stuff.
void cmb_acc::LeadingActionInfo::Print(std::ostream &os) const
  {
    os << "leading action index= " << index
       << ", leading action representative value index= "
       <<  leading_representative_value_index
       << ", general action representative value index= "
       << general_representative_value_index;
  }
