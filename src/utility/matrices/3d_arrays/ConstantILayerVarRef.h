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
//ConstantILayerVarRef.h 

#ifndef CONSTANTILAYERVARREF_H
#define CONSTANTILAYERVARREF_H

#include "Array3dRange.h"


//! @ingroup arrays3d
//
//! @brief Reference to a layer of the array which points have constant I index.
template <class ARRAY_3D>
class ConstantILayerVarRef: public Array3dBoxVarRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxVarRef<ARRAY_3D>::const_reference const_reference;
    typedef typename Array3dBoxVarRef<ARRAY_3D>::reference reference;

    explicit ConstantILayerVarRef(ARRAY_3D &m,const size_t &iLayer=1,const size_t &f= 1,const size_t &c= 1);
    ConstantILayerVarRef(ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &);
    ConstantILayerVarRef(ARRAY_3D &t,const size_t &iLayer,const RangoIndice &row_range,const RangoIndice &column_range);
    inline virtual ~ConstantILayerVarRef(void) {}
    size_t getLayerIndex(void) const
      { return this->offset_cp+1; }
    virtual const_reference operator()(size_t iRow=1,size_t col=1) const
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(1,iRow,col); }
    virtual reference operator()(size_t iRow=1,size_t col=1)
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(1,iRow,col); }
  };

template<class ARRAY_3D>
ConstantILayerVarRef<ARRAY_3D>::ConstantILayerVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f,const size_t &c)
  : Array3dBoxVarRef<ARRAY_3D>(t,Array3dRange(iLayer,RangoIndice(f,t.getNumberOfRows()),RangoIndice(c,t.getNumberOfColumns()))) {}

template<class ARRAY_3D>
ConstantILayerVarRef<ARRAY_3D>::ConstantILayerVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f1,const size_t &c1,const size_t &f2,const size_t &c2)
  : Array3dBoxVarRef<ARRAY_3D>(t,iLayer,RangoIndice(f1,f2),RangoIndice(c1,c2)) {}

template<class ARRAY_3D>
ConstantILayerVarRef<ARRAY_3D>::ConstantILayerVarRef(ARRAY_3D &t,const size_t &iLayer,const RangoIndice &row_range,const RangoIndice &column_range)
  : Array3dBoxVarRef<ARRAY_3D>(t,iLayer,row_range,column_range)
  {}

#endif
