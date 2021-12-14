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
//KRowVarRef.h 

#ifndef KROWVARREF_H
#define KROWVARREF_H


//! @ingroup arrays3d
//
//! @brief Reference to a row of the array which points have constant
//! K indexes.
template <class ARRAY_3D>
class KRowVarRef: public Array3dBoxVarRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxVarRef<ARRAY_3D>::const_reference const_reference;
    typedef typename Array3dBoxVarRef<ARRAY_3D>::reference reference;

    explicit KRowVarRef(ARRAY_3D &m,const size_t &iLayer= 1,const size_t &f= 1);
    KRowVarRef(ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &);
    KRowVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f,const RangoIndice &rango_col);
    inline virtual ~KRowVarRef(void) {}
    size_t getLayerIndex(void) const
      { return this->offset_cp+1; }
    size_t getRowIndex(void) const
      { return this->offset_f+1; }
    inline virtual const_reference operator()(size_t c=1) const
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(1,1,c); }
    inline virtual reference operator()(size_t c=1)
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(1,1,c); }
    inline virtual const_reference operator()(size_t iLayer,size_t iRow,size_t col) const
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,iRow,col); }
    inline virtual reference operator()(size_t iLayer,size_t iRow,size_t col)
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,iRow,col); }
  };

//! @brief Default constructor.
template<class ARRAY_3D>
KRowVarRef<ARRAY_3D>::KRowVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f)
  : Array3dBoxVarRef<ARRAY_3D>(t,iLayer,f,RangoIndice(1,t.getNumberOfColumns())) {}

template<class ARRAY_3D>
KRowVarRef<ARRAY_3D>::KRowVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f,const size_t &c1,const size_t &c2)
  : Array3dBoxVarRef<ARRAY_3D>(t,iLayer,f,RangoIndice(c1,c2)) {}

template<class ARRAY_3D>
KRowVarRef<ARRAY_3D>::KRowVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f,const RangoIndice &rango_col)
  : Array3dBoxVarRef<ARRAY_3D>(t,iLayer,f,rango_col) {}

#endif
