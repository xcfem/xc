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
//ColumnConstRef.h 

#ifndef COLUMNCONSTREF_H
#define COLUMNCONSTREF_H

#include "BoxConstRef.h"

//! @brief Constant reference to a column of a matrix.
template <class MAT>
class ColumnConstRef: public BoxConstRef<MAT>
  {
  public:
    typedef typename BoxConstRef<MAT>::const_reference const_reference;

    ColumnConstRef(const MAT &m,const size_t &c= 1,const size_t &f= 1);
    ColumnConstRef(const MAT &m,const size_t &,const size_t &,const size_t &);
    ColumnConstRef(const MAT &mat,const RangoIndice &,const size_t &);
    virtual const_reference operator()(size_t iRow) const
      { return BoxConstRef<MAT>::operator()(iRow,1); }
  };

template<class MAT>
ColumnConstRef<MAT>::ColumnConstRef(const MAT &mat,const size_t &c,const size_t &f)
  : BoxConstRef<MAT>(mat,f,c,mat.getNumberOfRows(),c)
  {}

template<class MAT>
ColumnConstRef<MAT>::ColumnConstRef(const MAT &mat,const size_t &c,const size_t &f1,const size_t &f2)
  : BoxConstRef<MAT>(mat,f1,c,f2,c)
  {}

template<class MAT>
ColumnConstRef<MAT>::ColumnConstRef(const MAT &mat,const RangoIndice &row_range,const size_t &col)
  : BoxConstRef<MAT>(mat,row_range,col)
  {}

#endif
