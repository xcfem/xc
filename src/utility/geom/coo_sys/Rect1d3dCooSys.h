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
//Rect1d3dCooSys.h
//One-dimensional coordinate system defined in a three-dimensional space.

#ifndef SISCOORECT1D3D_H
#define SISCOORECT1D3D_H

#include "Xd3dCooSys.h"

//! @ingroup CooSys
//
//! @brief One-dimensional coordinate system defined in a
//! three-dimensional space.
class Rect1d3dCooSys: public Xd3dCooSys
  {
  public:
    typedef GEOM_FT VLocal; //Dimension of the vector in local coordinates.
    typedef GEOM_FT PLocal; //Dimension of the point in local coordinates.

  public:
    Rect1d3dCooSys(void): Xd3dCooSys(1) {}
    Rect1d3dCooSys(const PGlobal &o,const PGlobal &p);
    Rect1d3dCooSys(const VGlobal &vX);
    virtual CooSys *Copia(void) const
      { return new Rect1d3dCooSys(*this); }
    virtual Xd3dCooSys *CopiaXd3d(void) const
      { return new Rect1d3dCooSys(*this); }
    void XAxisVector(const VGlobal &vX);
    VGlobal getIVector(void) const; //Return the global coordinates of unit vector I.
    VGlobal getGlobalCoordinates(const VLocal &v) const;
    VLocal getLocalCoordinates(const VGlobal &v) const;
    virtual ~Rect1d3dCooSys(void)
      {}
  };

#endif
