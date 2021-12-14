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
//CacheExpresiones.h

#ifndef CACHEEXPRESIONES_H
#define CACHEEXPRESIONES_H

#include "ExprAlgebra.h"
#include <ext/hash_map>

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

class CacheExpresiones
  {
  private:

    typedef __gnu_cxx::hash_map<const char *,ExprAlgebra,__gnu_cxx::hash<const char*>,eqstr> tabla_expresiones;
    typedef tabla_expresiones::iterator iterator;
    typedef tabla_expresiones::const_iterator const_iterator;
    typedef tabla_expresiones::value_type value_type;
    tabla_expresiones expresiones;
  public:
    CacheExpresiones(void)
      : expresiones() {}
    void Nueva(const std::string &,const ExprAlgebra &d);
    const ExprAlgebra *Busca(const std::string &) const;
    inline size_t size(void) const
      { return expresiones.size(); }
 };

#endif
