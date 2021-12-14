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
//BoolArray3d.cc

#include "BoolArray3d.h"

//! @brief Generate the bool arrays from the arguments.
//!
//! @param n_layers: number of layers.
//! @param n_rows: number of rows for each layer.
//! @param n_columns: number of columns for each layer.
//! @param def_value: default value.
BoolArray3d::BoolArray3d(const size_t &n_layers, const size_t &n_rows, const size_t &n_columns, const bool &def_value)
: Array3dBase<m_bool>(n_layers,n_rows,n_columns,def_value)
  {}
