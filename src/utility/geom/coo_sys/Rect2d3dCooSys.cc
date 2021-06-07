//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Rect2d3dCooSys.cc

#include "Rect2d3dCooSys.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Vector2d.h"

#include "utility/utils/text/text_string.h"



//! @brief Constructor.
//!
//! @param v: rotation angles about the axis 3, 2 and 1 that
//! applied sequentially align the global system with this one.
Rect2d3dCooSys::Rect2d3dCooSys(const VGlobal &v)
  : Xd3dCooSys(2,v) {}
//! @brief Define a coordinate system with dimension i
//! the x axis will have the orientation and sense of the v1 argument.
Rect2d3dCooSys::Rect2d3dCooSys(const VGlobal &v1,const VGlobal &v2)
  : Xd3dCooSys(2,v1,v2) {}
//! @brief Build the 2D coordinate system defined in a 3D space, so 
//! the axis 1 goes from p1 to p2 and p3 defines the plane that contains the
//! axis 1 and 2.
Rect2d3dCooSys::Rect2d3dCooSys(const PGlobal &p1,const PGlobal &p2, const PGlobal &p3)
  : Xd3dCooSys(2,p1,p2,p3) {}

//! @brief Return the unary vector I in global coordinates.
Rect2d3dCooSys::VGlobal Rect2d3dCooSys::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Return the unary vector J in global coordinates.
Rect2d3dCooSys::VGlobal Rect2d3dCooSys::getJVector(void) const
  { return getAxisVDir(2); }

//! @brief Return the unary vector K in global coordinates.
Rect2d3dCooSys::VGlobal Rect2d3dCooSys::getKVector(void) const
  { return cross(getIVector(),getJVector()); }

//! @brief Return the global coordinates of the vector.
//!
//! @param v: local coordinates of the vector.
Rect2d3dCooSys::VGlobal Rect2d3dCooSys::getGlobalCoordinates(const VLocal &v) const
  { return Xd3dCooSys::getGlobalCoordinates(v.getMatrix()); }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: global coordinates of the vector.
Rect2d3dCooSys::VLocal Rect2d3dCooSys::getLocalCoordinates(const VGlobal &v) const
  {
    const FT_matrix tmp= Xd3dCooSys::getLocalCoordinates(v);
    return VLocal(tmp(1),tmp(2));
  }

