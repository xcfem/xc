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
//op_tensor.cc

#include "op_tensor.h"

//Producto tensorial de dos tensores de primer orden.
m_double prod_tensor(const m_double &u,const m_double &v)
//u: row vector.
//v: column vector.
  {
    const size_t sz_u= u.size();
    const size_t sz_v= v.size();
    m_double retval(sz_u,sz_v);
    for(size_t i=1;i<=sz_u;i++)
      for(size_t j=1;j<=sz_v;j++)
        retval(i,j)= u[i-1]*v[j-1];
    return retval;
  }
