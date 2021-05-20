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
//MatrixRange.cc

#include "MatrixRange.h"

//! @brief Return verdadero si el rango no contiene ningún índice.
bool MatrixRange::Vacio(void) const
{ return (row_range.Vacio() || column_range.Vacio()); }

//! @brief Imprime el rango.
void MatrixRange::Print(std::ostream &os) const
  { os << row_range << ',' << column_range; }

//! @brief Reduce los valores del rango de manera que ambos sean menores que
//! los being passed as parameter.
void MatrixRange::Clip(const size_t &fmax,const size_t &cmax)
  {
    row_range.Clip(fmax);
    column_range.Clip(cmax);
  }

//! @brief Asigna a ESTE la intersección de ambos rangos.
void MatrixRange::Intersec(const MatrixRange &otro)
  {
    row_range.Intersec(otro.row_range);
    column_range.Intersec(otro.column_range);
  }

//! @brief Return la intersección de ambos rangos.
MatrixRange MatrixRange::Intersec(const MatrixRange &otro) const
  {
    MatrixRange retval(*this);
    retval.Intersec(otro);
    return retval;
  }

std::ostream &operator<<(std::ostream &os,const MatrixRange &rango)
  {
    rango.Print(os);
    return os;
  }

MatrixRange clip(const MatrixRange &r,const size_t &fmax,const size_t &cmax)
  {
    MatrixRange retval(r);
    retval.Clip(fmax,cmax);
    return retval;
  }

//! @brief Return la intersección de ambos rangos.
MatrixRange intersec(const MatrixRange &r1,const MatrixRange &r2)
  { return r1.Intersec(r2); }
