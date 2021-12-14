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
//Rect3d3dCooSys.h
//Three-dimensional coordinate system defined in a three-dimensional space.

#ifndef RECT3D3D_COO_SYS_H
#define RECT3D3D_COO_SYS_H

#include "Xd3dCooSys.h"

//! @ingroup CooSys
//
//! @brief Three-dimensional coordinate system defined in a
//! three-dimensional space.
class Rect3d3dCooSys: public Xd3dCooSys
  {
  public:
    typedef Vector3d VLocal; //Dimensión del vector en locales.
    typedef Pos3d PLocal; //Dimensión del vector en locales.
  public:
    Rect3d3dCooSys(void): Xd3dCooSys(3) {}
    Rect3d3dCooSys(const PGlobal &o,const PGlobal &p);
    Rect3d3dCooSys(const PGlobal &p1,const PGlobal &p2, const PGlobal &p3);
    Rect3d3dCooSys(const VGlobal &v1,const VGlobal &v2,const VGlobal &v3);
    Rect3d3dCooSys(const VGlobal &v1,const VGlobal &v2);
    virtual CooSys *Copia(void) const
      { return new Rect3d3dCooSys(*this); }
    virtual Xd3dCooSys *CopiaXd3d(void) const
      { return new Rect3d3dCooSys(*this); }

    VGlobal getIVector(void) const; //Return unary vector I in global coordinates.
    VGlobal getJVector(void) const; //Return unary vector J in global coordinates.
    VGlobal getKVector(void) const; //Return unary vector K in global coordinates.

    VGlobal getGlobalCoordinates(const VLocal &v) const;
    VLocal getLocalCoordinates(const VGlobal &v) const;
    virtual ~Rect3d3dCooSys(void)
      {}
  };

#endif
