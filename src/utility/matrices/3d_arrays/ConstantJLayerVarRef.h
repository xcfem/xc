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
//ConstantJLayerVarRef.h 

#ifndef CONSTANTJLAYERVARREF_H
#define CONSTANTJLAYERVARREF_H

#include "Array3dRange.h"


//! @ingroup arrays3d
//
//! @brief Reference to a layer of the array which points have
//! constant J index.
template <class ARRAY_3D>
class ConstantJLayerVarRef: public Array3dBoxVarRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxVarRef<ARRAY_3D>::const_reference const_reference;
    typedef typename Array3dBoxVarRef<ARRAY_3D>::reference reference;

    explicit ConstantJLayerVarRef(ARRAY_3D &m,const size_t &iLayer=1,const size_t &f= 1,const size_t &c= 1);
    ConstantJLayerVarRef(ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &);
    ConstantJLayerVarRef(ARRAY_3D &t,const RangoIndice &layer_range,const size_t &f,const RangoIndice &column_range);
    inline virtual ~ConstantJLayerVarRef(void) {}
    size_t getRowIndex(void) const
      { return this->offset_f+1; }
    virtual const_reference operator()(size_t iLayer=1,size_t col=1) const
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,1,col); }
    virtual reference operator()(size_t iLayer=1,size_t col=1)
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,1,col); }
  };

template<class ARRAY_3D>
ConstantJLayerVarRef<ARRAY_3D>::ConstantJLayerVarRef(ARRAY_3D &t,const size_t &iLayer,const size_t &f,const size_t &c)
  : Array3dBoxVarRef<ARRAY_3D>(t,Array3dRange(RangoIndice(iLayer,t.getNumberOfLayers()),f,RangoIndice(c,t.getNumberOfColumns()))) {}

template<class ARRAY_3D>
ConstantJLayerVarRef<ARRAY_3D>::ConstantJLayerVarRef(ARRAY_3D &t,const size_t &f,const size_t &iLayer1,const size_t &c1,const size_t &iLayer2,const size_t &c2)
  : Array3dBoxVarRef<ARRAY_3D>(t,RangoIndice(iLayer1,iLayer2),f,RangoIndice(c1,c2)) {}

template<class ARRAY_3D>
ConstantJLayerVarRef<ARRAY_3D>::ConstantJLayerVarRef(ARRAY_3D &t,const RangoIndice &layer_range,const size_t &f,const RangoIndice &column_range)
  : Array3dBoxVarRef<ARRAY_3D>(t,layer_range,f,column_range) {}

#endif
