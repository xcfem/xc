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
//RowConstRef.h 

#ifndef ROWCONSTREF_H
#define ROWCONSTREF_H

#include "BoxConstRef.h"

//! @brief Reference to a matrix row.
template <class MAT>
class RowConstRef: public BoxConstRef<MAT>
  {
  public:
    typedef typename BoxConstRef<MAT>::const_reference const_reference;

    RowConstRef(const MAT &m,const size_t &f= 1,const size_t &c= 1);
    RowConstRef(const MAT &m,const size_t &,const size_t &,const size_t &);
    RowConstRef(const MAT &mat,const size_t &,const RangoIndice &);
    virtual const_reference operator()(size_t col) const
      { return BoxConstRef<MAT>::operator()(1,col); }
  };

template<class MAT>
RowConstRef<MAT>::RowConstRef(const MAT &mat,const size_t &f,const size_t &c)
  : BoxConstRef<MAT>(mat,f,c,f,mat.getNumberOfColumns())
  {}

template<class MAT>
RowConstRef<MAT>::RowConstRef(const MAT &mat,const size_t &f,const size_t &c1,const size_t &c2)
  : BoxConstRef<MAT>(mat,f,RangoIndice(c1,c2))
  {}

template<class MAT>
RowConstRef<MAT>::RowConstRef(const MAT &mat,const size_t &f,const RangoIndice &column_range)
  : BoxConstRef<MAT>(mat,f,column_range)
  {}

#endif
