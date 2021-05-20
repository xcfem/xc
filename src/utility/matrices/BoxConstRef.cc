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
//BoxConstRef.cc

#include "BoxConstRef.h"

//! @brief Constructor por defecto.
BoxBaseRef::BoxBaseRef(const ProtoMatrix &mat,const size_t &f1,const size_t &c1)
  : ProtoMatrix(std::max(mat.getNumberOfRows()-f1+1,size_t(0)),std::max(mat.getNumberOfColumns()-c1+1,size_t(0))), offset_f(f1-1),offset_c(c1-1)
  {}

//! @brief Constructor.
BoxBaseRef::BoxBaseRef(const ProtoMatrix &mat,const size_t &f1,const size_t &c1,const size_t &f2,const size_t &c2)
  : ProtoMatrix(std::max(f2-f1+1,size_t(0)),std::max(c2-c1+1,size_t(0))), offset_f(f1-1),offset_c(c1-1)
  {}

//! @brief Constructor.
BoxBaseRef::BoxBaseRef(const ProtoMatrix &mat,const RangoIndice &row_range,const RangoIndice &column_range)
  : ProtoMatrix(row_range.Size(),column_range.Size()), offset_f(row_range.Offset()),offset_c(column_range.Offset())
  {}

//! @brief Column alone constructor.
BoxBaseRef::BoxBaseRef(const ProtoMatrix &mat,const RangoIndice &row_range,const size_t &col)
  : ProtoMatrix(row_range.Size(),1), offset_f(row_range.Offset()),offset_c(col-1)
  {}

//! @brief Row alone constructor.
BoxBaseRef::BoxBaseRef(const ProtoMatrix &mat,const size_t &iRow,const RangoIndice &column_range)
  : ProtoMatrix(1,column_range.Size()), offset_f(iRow-1),offset_c(column_range.Offset())
  {}

//! @brief Return the row range.
RangoIndice BoxBaseRef::getRowRange(void) const
  { return RangoIndice(offset_f+1,offset_f+getNumberOfRows()); }

//! @brief Return the column range.
RangoIndice BoxBaseRef::getColumnRange(void) const
  { return RangoIndice(offset_c+1,offset_c+getNumberOfColumns()); }

