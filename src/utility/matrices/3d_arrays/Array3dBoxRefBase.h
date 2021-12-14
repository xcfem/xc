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
//Array3dBoxRefBase.h 

#ifndef ARRAY3DBOXREFBASE_H
#define ARRAY3DBOXREFBASE_H

#include <ostream>

class RangoIndice;
class Array3dRange;

//! @ingroup arrays3d
//
//! @brief Indices that define a block of an array.
class Array3dBoxRefBase
  {
  protected:
    size_t n_layers; //!< number of layers.
    size_t n_rows; //!< number of rows.
    size_t n_columns; //!< number of columns.
    size_t offset_cp; //!< Offset layer.
    size_t offset_f; //!< Offset row.
    size_t offset_c; //!< Offset column.
  public:
    Array3dBoxRefBase(const size_t &,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &);
    Array3dBoxRefBase(const Array3dRange &rango);
    Array3dBoxRefBase(const size_t &iLayer,const RangoIndice &row_range,const RangoIndice &column_range);
    Array3dBoxRefBase(const RangoIndice &layer_range,const size_t &iRow,const RangoIndice &column_range);
    Array3dBoxRefBase(const RangoIndice &layer_range,const RangoIndice &row_range,const size_t &col);
    Array3dBoxRefBase(const size_t &iLayer,const size_t &iRow,const RangoIndice &column_range);
    Array3dBoxRefBase(const size_t iLayer,const RangoIndice &row_range,const size_t &col);
    Array3dBoxRefBase(const RangoIndice &layer_range,const size_t &iRow,const size_t &col);
    inline const size_t &getNumberOfLayers(void) const
      { return n_layers; }
    inline const size_t &getNumberOfRows(void) const
      { return n_rows; }
    inline const size_t &getNumberOfColumns(void) const
      { return n_columns; }
    inline const size_t Size(void) const
      { return n_layers*n_rows*n_columns; }
    bool Empty(void) const;
    RangoIndice LayerRange(void) const;
    RangoIndice RowRange(void) const;
    RangoIndice RangoCols(void) const;
  };

template <class ARRAY_3D>
inline void PrintArray3d(const ARRAY_3D &t,std::ostream &os)
  {
    os << '[';
    const size_t n_layers= t.getNumberOfLayers();
    for(size_t i=1;i<=n_layers;i++)
      {
        os << '[';
        const size_t n_rows= t.getNumberOfRows();
        for(size_t j= 1;j<=n_rows;j++)
          {
            os << '[';
            const size_t n_columns= t.getNumberOfColumns();
            if(n_columns > 0) os << t(i,j,1);
	    for(size_t k= 2;k<=n_columns;k++)
	      os << ',' << t(i,j,k);
            os << ']';
          }
        os << ']';
      }
    os << ']';
  }

#endif
