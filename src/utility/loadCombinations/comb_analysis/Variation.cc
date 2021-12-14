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
//Variation.cxx

#include "Variation.h"
#include "utility/loadCombinations/actions/ActionRValueList.h"

//! \fn void cmb_acc::Variation::print(std::ostream &os) const
//! @brief Imprime la variación.
void cmb_acc::Variation::print(std::ostream &os) const
  {
    if(size()<1) return;
    const_iterator i= begin();
    os << '[';
    os << *i; i++;
    for(;i!=end();i++)
      os << ',' << *i;
    os << ']';
  }

//! \fn cmb_acc::Variation cmb_acc::Variation::concat(const cmb_acc::Variation &a,const cmb_acc::Variation &b)
//! @brief Concatena a la variación a la variación b
cmb_acc::Variation cmb_acc::Variation::concat(const cmb_acc::Variation &a,const cmb_acc::Variation &b)
  {
    const size_t sz_a= a.size();
    const size_t sz_b= b.size();
    const size_t sz= sz_a+sz_b;
    Variation retval(sz);
    for(size_t i=0;i<sz_a;i++)
      retval[i]= a[i];
    for(size_t i=0;i<sz_b;i++)
      retval[i+sz_a]= b[i];
    return retval;
  }


//! @brief Return verdadero si las acciones que contiene la variaciónes son compatibles
bool cmb_acc::Variation::compatible(const ActionRValueList &lvr)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      for(size_t j=i+1;j<sz;j++)
        if( ((*this)[i]!= 0.0) && ((*this)[j]!= 0.0) )
          if(incompatibles(lvr[i],lvr[j]))
            return false;
    return true;
  }

//! \fn std::ostream &cmb_acc::operator<<(std::ostream &os,const cmb_acc::Variation &v)
//! @brief Operador salida.
std::ostream &cmb_acc::operator<<(std::ostream &os,const cmb_acc::Variation &v)
  {
    v.print(os);
    return os;
  }
