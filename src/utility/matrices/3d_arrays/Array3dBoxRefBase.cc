//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Array3dBoxRefBase.cc

#include "Array3dBoxRefBase.h"
#include "utility/matrices/RangoIndice.h"
#include "Array3dRange.h"
#include <iostream>

Array3dBoxRefBase::Array3dBoxRefBase(const size_t &iLayer1,const size_t &f1,const size_t &c1,const size_t &iLayer2,const size_t &f2,const size_t &c2)
  : n_layers(std::max(iLayer2-iLayer1+1,size_t(0))), n_rows(std::max(f2-f1+1,size_t(0))), n_columns(std::max(c2-c1+1,size_t(0))),
    offset_cp(iLayer1-1),offset_f(f1-1),offset_c(c1-1) {}

Array3dBoxRefBase::Array3dBoxRefBase(const Array3dRange &rango)
  : n_layers(rango.getLayerRange().Size()), n_rows(rango.getRowRange().Size()), n_columns(rango.getColumnRange().Size()),
    offset_cp(rango.getLayerRange().Offset()),offset_f(rango.getRowRange().Offset()),offset_c(rango.getColumnRange().Offset()) 
  {
    if(rango.Vacio())
      std::cerr << "Error!. El rango: '" << rango << "', is empty." << std::endl;
  }

Array3dBoxRefBase::Array3dBoxRefBase(const size_t &iLayer,const RangoIndice &row_range,const RangoIndice &column_range)
  : n_layers(1), n_rows(row_range.Size()), n_columns(column_range.Size()),
    offset_cp(iLayer-1),offset_f(row_range.Offset()),offset_c(column_range.Offset()) 
  {
    if(row_range.Vacio())
      std::cerr << "Error!. The row range: '" << row_range
		<< "', is empty." << std::endl;
    if(column_range.Vacio())
      std::cerr << "Error!. The column range: '" << column_range
		<< "', is empty." << std::endl;
  }

Array3dBoxRefBase::Array3dBoxRefBase(const RangoIndice &layer_range,const size_t &row,const RangoIndice &column_range)
  : n_layers(layer_range.Size()), n_rows(1), n_columns(column_range.Size()),
    offset_cp(layer_range.Offset()),offset_f(row-1),offset_c(column_range.Offset())
  {
    if(layer_range.Vacio())
      std::cerr << "Error!. the layer range: '" << layer_range << "', is empty." << std::endl;
    if(column_range.Vacio())
      std::cerr << "Error!. the column range: '" << column_range << "', is empty." << std::endl;
  }

Array3dBoxRefBase::Array3dBoxRefBase(const RangoIndice &layer_range,const RangoIndice &row_range,const size_t &col)
  : n_layers(layer_range.Size()), n_rows(row_range.Size()), n_columns(1),
    offset_cp(layer_range.Offset()),offset_f(row_range.Offset()),offset_c(col-1)
  {
    if(layer_range.Vacio())
      std::cerr << "Error!. the layer range: '" << layer_range << "', is empty." << std::endl;
    if(row_range.Vacio())
      std::cerr << "Error!. Row range: '" << row_range << "', is empty." << std::endl;
  }

Array3dBoxRefBase::Array3dBoxRefBase(const size_t &iLayer,const size_t &row,const RangoIndice &column_range)
  : n_layers(1), n_rows(1), n_columns(column_range.Size()),
    offset_cp(iLayer-1),offset_f(row-1),offset_c(column_range.Offset()) {}

Array3dBoxRefBase::Array3dBoxRefBase(const size_t iLayer,const RangoIndice &row_range,const size_t &col)
  : n_layers(1), n_rows(row_range.Size()), n_columns(1),
    offset_cp(iLayer-1),offset_f(row_range.Offset()),offset_c(col-1) {}

Array3dBoxRefBase::Array3dBoxRefBase(const RangoIndice &layer_range,const size_t &row,const size_t &col)
  : n_layers(layer_range.Size()), n_rows(1), n_columns(1),
    offset_cp(layer_range.Offset()),offset_f(row-1),offset_c(col-1) {}

//! @brief Return el rango de iLayers.
RangoIndice Array3dBoxRefBase::LayerRange(void) const
  { return RangoIndice(offset_cp+1,offset_cp+getNumberOfLayers()); }

//! @brief Return the row range.
RangoIndice Array3dBoxRefBase::RowRange(void) const
  { return RangoIndice(offset_f+1,offset_f+getNumberOfRows()); }

//! @brief Return el column range.
RangoIndice Array3dBoxRefBase::RangoCols(void) const
  { return RangoIndice(offset_c+1,offset_c+getNumberOfColumns()); }

//! @brief Return verdadero si está vacía
bool  Array3dBoxRefBase::Empty(void) const
  { return (Size() == 0); }
