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
//op_tensor.h
//Operaciones con tensores.

#ifndef OP_TENSOR_H
#define OP_TENSOR_H

#include "m_double.h"

//Producto tensorial de dos tensores de primer orden.
m_double prod_tensor(const m_double &u,const m_double &v);

//Operador producto tensorial.
inline m_double operator&(const m_double &u,const m_double &v)
  { return prod_tensor(u,v); }
#endif
