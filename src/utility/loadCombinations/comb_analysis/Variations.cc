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
//Variations.cxx

#include "Variations.h"
#include "utility/loadCombinations/actions/ActionRValueList.h"

//! @brief Constructor.
cmb_acc::Variations::Variations(const size_t &sz,const Variation &v)
  : std::vector<Variation>(sz,v) {}

//! \fn void cmb_acc::Variations::print(std::ostream &os) const
//! @brief Imprime las variations.
void cmb_acc::Variations::print(std::ostream &os) const
  {
    if(size()<1) return;
    const_iterator i= begin();
    os << '[';
    os << *i; i++;
    for(;i!=end();i++)
      os << ' ' <<*i;
    os << ']';
  }

//! @brief Return the first combination.
cmb_acc::Variations cmb_acc::Variations::first_combination(const Variation &v)
  {
    const size_t sz= v.size();
    Variations retval(sz);
    for(size_t i=0;i<sz;i++)
      retval[i]= Variation(1,v[i]);
    return retval;
  }

//! @brief Return el producto cartesiano de las variations que se pasan como parámetro.
cmb_acc::Variations cmb_acc::Variations::prod_cartesiano(const Variations &a,const Variations &b)
  {
    const size_t sz_a= a.size();
    const size_t sz_b= b.size();
    std::deque<Variation> tmp;
    for(size_t i=0;i<sz_a;i++)
      for(size_t j=0;j<sz_b;j++)
        {
          Variation tmp_var= Variation::concat(a[i],b[j]);
          tmp.push_back(tmp_var);
        }
    const size_t sz= tmp.size();
    Variations retval(sz);
    for(size_t i= 0;i<sz;i++)
      retval[i]= tmp[i];
    return retval;
  }

//! @brief Operador salida.
std::ostream &cmb_acc::operator<<(std::ostream &os,const Variations &vs)
  {
    vs.print(os);
    return os;
  }
