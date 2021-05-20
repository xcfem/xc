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
//Rect1d2dCooSys.h
//One dimensional coordinate system defined in a three-dimensional space.

#ifndef SISCOORECT1D2D_H
#define SISCOORECT1D2D_H

#include "Xd2dCooSys.h"

//! @ingroup CooSys
//
//! @brief One dimensional coordinate system defined in a three-dimensional
//! space.
class Rect1d2dCooSys: public Xd2dCooSys
  {
  public:
    typedef GEOM_FT VLocal; //Vector type in local system.
    typedef GEOM_FT PLocal; //Point type in local system.

    Rect1d2dCooSys(void): Xd2dCooSys(1) {}
    Rect1d2dCooSys(const PGlobal &o,const PGlobal &p);
    Rect1d2dCooSys(const VGlobal &vX);
    virtual CooSys *Copia(void) const
      { return new Rect1d2dCooSys(*this); }

    VGlobal getIVector(void) const; //Return global coordinates of unit vector I.

    VGlobal GetCooGlobales(const VLocal &v) const;
    VLocal GetCooLocales(const VGlobal &v) const;
    virtual ~Rect1d2dCooSys(void)
      {}
  };

#endif
