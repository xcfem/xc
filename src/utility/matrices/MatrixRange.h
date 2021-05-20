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
//MatrixRange.h

#ifndef MATRIXRANGE_H
#define MATRIXRANGE_H

#include "RangoIndice.h"

//! @brief Rango de variación de un índice, se emplea en BoxConstRef.
class MatrixRange
  {
    RangoIndice row_range; //!< row range.
    RangoIndice column_range; //!< column range.
  public:
    MatrixRange(const RangoIndice &rf,const RangoIndice &rc)
      :row_range(rf),column_range(rc) {}
    template <class M>
    MatrixRange(const M &);
    size_t getNumberOfRows(void) const
      { return row_range.Size(); }
    size_t getNumberOfColumns(void) const
      { return column_range.Size(); }
    size_t Size(void) const
      { return getNumberOfRows()*getNumberOfColumns(); }
    const RangoIndice &getRowRange(void) const
      { return row_range; }
    RangoIndice &getRowRange(void)
      { return row_range; }
    const RangoIndice &getColumnRange(void) const
      { return column_range; }
    RangoIndice &GetgetColumnRange(void)
      { return column_range; }
    bool isRow(void) const
      { return (row_range.Size()==1); }
    bool isColumn(void) const
      { return (column_range.Size()==1); }
    bool Vacio(void) const;
    inline static const char &Separador(void)
      { return RangoIndice::Separador(); }
    void Clip(const size_t &fmax,const size_t &cmax);
    void Intersec(const MatrixRange &otro);
    MatrixRange Intersec(const MatrixRange &otro) const;
    void Print(std::ostream &os) const;
  };

MatrixRange clip(const MatrixRange &r,const size_t &fmax,const size_t &cmax);
MatrixRange intersec(const MatrixRange &,const MatrixRange &);
template <class M>
MatrixRange clip(const MatrixRange &,const M &);
std::ostream &operator<<(std::ostream &os,const MatrixRange &rango);

//! @brief Constructor from a matrix.
template <class M>
MatrixRange::MatrixRange(const M &m)
  : row_range(1,m.getNumberOfRows()),column_range(1,m.getNumberOfColumns()) {}

//! @brief Clipping of the range intervals from a matrix.
template <class M>
MatrixRange clip(const MatrixRange &rm,const M &m)
  { return rm.Intersec(MatrixRange(m)); }
#endif
