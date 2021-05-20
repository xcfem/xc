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
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles DE LA LICENCIA PÚBLICA GENERAL GNU PARA OBTENER 
//  UNA INFORMACIÓN MÁS DETALLADA. 
//
// DEBERÍA HABER RECIBIDO UNA COPIA DE LA LICENCIA PÚBLICA GENERAL GNU 
// JUNTO A ESTE PROGRAMA. 
// EN CASO CONTRARIO, CONSULTE <HTTP://WWW.GNU.ORG/LICENSES/>.
//----------------------------------------------------------------------------
//CONSTANTKLAYERCONSTREF.H 

#ifndef CONSTANTKLAYERCONSTREF_H
#define CONSTANTKLAYERCONSTREF_H

#include "Array3dRange.h"


//! @ingroup arrays3d
//
//! @brief Reference to a layer of the array which points
//! have constant K index.
template <class ARRAY_3D>
class ConstantKLayerConstRef: public Array3dBoxConstRef<ARRAY_3D>
  {
  public:
    typedef typename Array3dBoxConstRef<ARRAY_3D>::const_reference const_reference;

    explicit ConstantKLayerConstRef(const ARRAY_3D &m,const size_t &c=1,const size_t &iLayer= 1,const size_t &f= 1);
    ConstantKLayerConstRef(const ARRAY_3D &m,const size_t &,const size_t &,const size_t &,const size_t &,const size_t &);
    ConstantKLayerConstRef(const ARRAY_3D &t,const RangoIndice &column_range,const RangoIndice &row_range,const size_t &c);
    inline virtual ~ConstantKLayerConstRef(void) {}
    virtual const_reference operator()(size_t iLayer=1,size_t iRow=1) const
      { return Array3dBoxConstRef<ARRAY_3D>::operator()(iLayer,iRow,1); }
  };

template<class ARRAY_3D>
ConstantKLayerConstRef<ARRAY_3D>::ConstantKLayerConstRef(const ARRAY_3D &t,const size_t &c,const size_t &iLayer,const size_t &f)
  : Array3dBoxConstRef<ARRAY_3D>(t,Array3dRange(RangoIndice(iLayer,t.getNumberOfLayers()),RangoIndice(f,t.getNumberOfRows()),c)) {}

template<class ARRAY_3D>
ConstantKLayerConstRef<ARRAY_3D>::ConstantKLayerConstRef(const ARRAY_3D &t,const size_t &c,const size_t &iLayer1,const size_t &f1,const size_t &iLayer2,const size_t &f2)
  : Array3dBoxConstRef<ARRAY_3D>(t,Array3dRange(RangoIndice(iLayer1,iLayer2),RangoIndice(f1,f2),c)) {}

template<class ARRAY_3D>
ConstantKLayerConstRef<ARRAY_3D>::ConstantKLayerConstRef(const ARRAY_3D &t,const RangoIndice &column_range,const RangoIndice &row_range,const size_t &c)
  : Array3dBoxConstRef<ARRAY_3D>(t,column_range,row_range,c)
  {}

#endif
