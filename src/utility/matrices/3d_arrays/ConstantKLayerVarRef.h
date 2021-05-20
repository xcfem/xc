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
//ConstantKLayerVarRef.h 

#ifndef CONSTANTKLAYERVARREF_H
#define CONSTANTKLAYERVARREF_H


//! @ingroup arrays3d
//
//! @brief Reference to a layer of the array which points
//! have constant K index.
template <class ARRAY_3D>
class ConstantKLayerVarRef: public Array3dBoxVarRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxVarRef<ARRAY_3D>::const_reference const_reference;
    typedef typename Array3dBoxVarRef<ARRAY_3D>::reference reference;

    explicit ConstantKLayerVarRef(ARRAY_3D &m,const size_t &c=1,const size_t &iLayer= 1,const size_t &f= 1);
    ConstantKLayerVarRef(ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &);
    ConstantKLayerVarRef(ARRAY_3D &t,const RangoIndice &column_range,const RangoIndice &row_range,const size_t &c);
    inline virtual ~ConstantKLayerVarRef(void) {}
    size_t IndiceCol(void) const
      { return this->offset_c+1; }
    virtual const_reference operator()(size_t iLayer=1,size_t iRow=1) const
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,iRow,1); }
    virtual reference operator()(size_t iLayer=1,size_t iRow=1)
      { return Array3dBoxVarRef<ARRAY_3D>::operator()(iLayer,iRow,1); }
  };

template<class ARRAY_3D>
ConstantKLayerVarRef<ARRAY_3D>::ConstantKLayerVarRef(ARRAY_3D &t,const size_t &c,const size_t &iLayer,const size_t &f)
  : Array3dBoxVarRef<ARRAY_3D>(t,Array3dRange(RangoIndice(iLayer,t.getNumberOfLayers()),RangoIndice(f,t.getNumberOfRows()),c)) {}

template<class ARRAY_3D>
ConstantKLayerVarRef<ARRAY_3D>::ConstantKLayerVarRef(ARRAY_3D &t,const size_t &c,const size_t &iLayer1,const size_t &f1,const size_t &iLayer2,const size_t &f2)
  : Array3dBoxVarRef<ARRAY_3D>(t,Array3dRange(RangoIndice(iLayer1,iLayer2),RangoIndice(f1,f2),c)) {}

template<class ARRAY_3D>
ConstantKLayerVarRef<ARRAY_3D>::ConstantKLayerVarRef(ARRAY_3D &t,const RangoIndice &column_range,const RangoIndice &row_range,const size_t &c)
  : Array3dBoxVarRef<ARRAY_3D>(t,column_range,row_range,c) {}

#endif
