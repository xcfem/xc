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
//BoolArray3d.h
//Array of point matrices.

#ifndef BOOLARRAY3D_H
#define BOOLARRAY3D_H

#include "utility/matrices/m_bool.h"
#include "Array3dBase.h"

//! @ingroup arrays3d
//
//! @brief Base class for grids of bool in 3D (used to express if
//! something exists or not in a (layer,row,column).
class BoolArray3d: public Array3dBase<m_bool>
  {
  public:
    BoolArray3d(const size_t &n_layers, const size_t &n_rows, const size_t &n_columns, const bool &def_value= true); 
  };



#endif
