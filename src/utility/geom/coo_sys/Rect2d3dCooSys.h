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
//Rect2d3dCooSys.h
//Two-dimensional rectangular coordinate system in a three-dimensional space.

#ifndef RECT2D_COO_SYS_H
#define RECT2D_COO_SYS_H

#include "Xd3dCooSys.h"

class Vector2d;
class Pos2d;

//! @ingroup CooSys
//
//! @brief Two-dimensional rectangular coordinate system
//! definde in a three-dimensional space.
class Rect2d3dCooSys: public Xd3dCooSys
  {
  public:
    typedef Vector2d VLocal; //Vector dimension in local coordinates.
    typedef Pos2d PLocal; //Point dimension in local coordinates.

    Rect2d3dCooSys(void)
      : Xd3dCooSys(2) {}
    Rect2d3dCooSys(const VGlobal &v);
    Rect2d3dCooSys(const VGlobal &v1,const VGlobal &v2);
    Rect2d3dCooSys(const PGlobal &p1,const PGlobal &p2, const PGlobal &p3);

    virtual CooSys *Copia(void) const
      { return new Rect2d3dCooSys(*this); }
    virtual Xd3dCooSys *CopiaXd3d(void) const
      { return new Rect2d3dCooSys(*this); }

    //Access to members.
    VGlobal getIVector(void) const;
    VGlobal getJVector(void) const;
    VGlobal getKVector(void) const;

    VGlobal getGlobalCoordinates(const VLocal &v) const;
    VLocal getLocalCoordinates(const VGlobal &v) const;

    virtual ~Rect2d3dCooSys(void)
      {}
  };

#endif
