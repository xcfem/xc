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
//JRowConstRef.h 

#ifndef JROWCONSTREF_H
#define JROWCONSTREF_H


//! @ingroup arrays3d
//
//! @brief Reference to a layer of the array which points have constant
//! J and K indexes.
template <class ARRAY_3D>
class JRowConstRef: public Array3dBoxConstRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxConstRef<ARRAY_3D>::const_reference const_reference;

    explicit JRowConstRef(const ARRAY_3D &m,const size_t &iLayer= 1,const size_t &c= 1);
    JRowConstRef(const ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &);
    JRowConstRef(const ARRAY_3D &t,const size_t &iLayer,const RangoIndice &row_range,const size_t &c);
    inline virtual ~JRowConstRef(void) {}
    inline virtual const_reference operator()(size_t f=1) const
      { return Array3dBoxConstRef<ARRAY_3D>::operator()(1,f,1); }
    inline virtual const_reference operator()(size_t iLayer,size_t iRow,size_t col) const
      { return Array3dBoxConstRef<ARRAY_3D>::operator()(iLayer,iRow,col); }
  };

//! @brief Default constructor.
template<class ARRAY_3D>
JRowConstRef<ARRAY_3D>::JRowConstRef(const ARRAY_3D &t,const size_t &iLayer,const size_t &c)
  : Array3dBoxConstRef<ARRAY_3D>(t,iLayer,RangoIndice(1,t.getNumberOfRows()),c)
  {}

template<class ARRAY_3D>
JRowConstRef<ARRAY_3D>::JRowConstRef(const ARRAY_3D &t,const size_t &iLayer,const size_t &f1,const size_t &f2,const size_t &c)
  : Array3dBoxConstRef<ARRAY_3D>(t,iLayer,RangoIndice(f1,f2),c) {}

template<class ARRAY_3D>
JRowConstRef<ARRAY_3D>::JRowConstRef(const ARRAY_3D &t,const size_t &iLayer,const RangoIndice &row_range,const size_t &c)
  : Array3dBoxConstRef<ARRAY_3D>(t,iLayer,row_range,c) {}

#endif
