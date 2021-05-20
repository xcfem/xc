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
//CacheExpresiones.cc

#include "CacheExpresiones.h"

void CacheExpresiones::Nueva(const std::string &str_expr,const ExprAlgebra &expr)
  {
    const ExprAlgebra *i= Busca(str_expr);
    if(!i)
      expresiones[str_expr.c_str()]= expr;
    if(expresiones.size()>50000)
      std::clog << "Tamaño del caché de expresiones muy grande:" << expresiones.size() << std::endl;
  }
const ExprAlgebra *CacheExpresiones::Busca(const std::string &str_expr) const
  {
    const_iterator i= expresiones.find(str_expr.c_str());
    if(i!=expresiones.end())
      return &((*i).second);
    else
      return NULL;
  }
