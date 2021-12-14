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
//Rect2d2dCooSys.h

#ifndef RECT_2D2D_COO_SYS_H
#define RECT_2D2D_COO_SYS_H

#include "Xd2dCooSys.h"


//! @ingroup CooSys
//
//! @brief Twp-dimensional rectangular coordinate system
//! defined in a two-dimensional space.
class Rect2d2dCooSys: public Xd2dCooSys
  {
  public:
    typedef Vector2d VLocal; //!< Dimension of the vector in local coordinates.
    typedef Pos2d PLocal; //!< Dimension of the point in local coordinates.


    Rect2d2dCooSys(void);
    Rect2d2dCooSys(const Pos2d &p1,const Pos2d &p2);
    Rect2d2dCooSys(const VGlobal &vX);
    virtual CooSys *Copia(void) const;

    //Acceso a miembros.
    VGlobal getIVector(void) const;
    VGlobal getJVector(void) const;
    void PutI(const VGlobal &);
    void PutJ(const VGlobal &);

    //Conversión vectores.
    VGlobal getGlobalCoordinates(const VLocal &v) const;
    VLocal getLocalCoordinates(const VGlobal &v) const;

    virtual ~Rect2d2dCooSys(void) {}
  };

#endif
