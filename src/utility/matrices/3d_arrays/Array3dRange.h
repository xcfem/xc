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
//Array3dRange.h

#ifndef RANGOARRAY3D_H
#define RANGOARRAY3D_H

#include "utility/matrices/RangoIndice.h"


//! @ingroup arrays3d
//
//! @brief Rangos de variación de tres índices, se emplea en BoxConstRef.
class Array3dRange
  {
    RangoIndice layer_range; //!< layers range.
    RangoIndice row_range; //!< row range.
    RangoIndice column_range; //!< columns range.
  public:
    Array3dRange(const RangoIndice &rcp= RangoIndice(),const RangoIndice &rf= RangoIndice(),const RangoIndice &rc= RangoIndice())
      : layer_range(rcp), row_range(rf),column_range(rc) {}
    Array3dRange(const size_t &iLayer,const RangoIndice &rf,const RangoIndice &rc)
      : layer_range(iLayer,iLayer), row_range(rf),column_range(rc) {}
    Array3dRange(const RangoIndice &rcp,const size_t &iRow,const RangoIndice &rc)
      : layer_range(rcp), row_range(iRow, iRow),column_range(rc) {}
    Array3dRange(const RangoIndice &rcp,const RangoIndice &rf,const size_t &col)
      : layer_range(rcp), row_range(rf),column_range(col,col) {}
    template <class TTZ>
    explicit Array3dRange(const TTZ &);

    size_t getNumberOfLayers(void) const
      { return row_range.Size(); }
    size_t getNumberOfRows(void) const
      { return row_range.Size(); }
    size_t getNumberOfColumns(void) const
      { return column_range.Size(); }
    size_t Size(void) const
      { return getNumberOfLayers()*getNumberOfRows()*getNumberOfColumns(); }
    bool Vacio(void) const;
    const RangoIndice &getLayerRange(void) const
      { return layer_range; }
    RangoIndice &getLayerRange(void)
      { return layer_range; }
    const RangoIndice &getRowRange(void) const
      { return row_range; }
    RangoIndice &getRowRange(void)
      { return row_range; }
    const RangoIndice &getColumnRange(void) const
      { return column_range; }
    RangoIndice &getColumnRange(void)
      { return column_range; }

    size_t Dimension(void) const;
    bool isIRow(void) const;
    bool isJRow(void) const;
    bool isKRow(void) const;
    bool isConstantILayer(void) const;
    bool isConstantJLayer(void) const;
    bool isConstantKLayer(void) const;

    inline static const char &Separador(void)
      { return RangoIndice::Separador(); }
    void Clip(const size_t &cpmax,const size_t &fmax,const size_t &cmax);
    void Intersec(const Array3dRange &);
    Array3dRange Intersec(const Array3dRange &other) const;
    void Print(std::ostream &os) const;
  };

Array3dRange clip(const Array3dRange &r,const size_t &cpmax,const size_t &fmax,const size_t &cmax);
Array3dRange intersec(const Array3dRange &,const Array3dRange &);
std::ostream &operator<<(std::ostream &os,const Array3dRange &rango);

//! @brief Constructor from an array.
template <class TTZ>
Array3dRange::Array3dRange(const TTZ &ttz)
  : layer_range(1,ttz.getNumberOfLayers()), row_range(1,ttz.getNumberOfRows()),column_range(1,ttz.getNumberOfColumns()) {}

//! @brief Clip the range intervals from the array argument.
template <class TTZ>
Array3dRange clip(const Array3dRange &rttz,const TTZ &ttz)
  { return rttz.Intersec(Array3dRange(ttz)); }

#endif
