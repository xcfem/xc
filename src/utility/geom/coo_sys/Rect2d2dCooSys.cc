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
//Rect2d2dCooSys.cc

#include "Rect2d2dCooSys.h"
#include "../pos_vec/Pos2d.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Vector3d.h"
#include "utility/matrices/normalize.h"
#include "utility/utils/misc_utils/matem.h"



//! @brief Default constructor.
Rect2d2dCooSys::Rect2d2dCooSys(void): Xd2dCooSys(2) {}
//! @brief Constructor.
Rect2d2dCooSys::Rect2d2dCooSys(const Pos2d &p1,const Pos2d &p2)
  : Xd2dCooSys(2,p1,p2) {} //Axis 1 from p1 to p2.
//! @brief Constructor.
Rect2d2dCooSys::Rect2d2dCooSys(const VGlobal &vX)
  : Xd2dCooSys(2,vX) {} //x axis parallel to vX.

//! @brief Constructor virtual.
CooSys *Rect2d2dCooSys::Copia(void) const
  { return new Rect2d2dCooSys(*this); }

//! @brief Return the unary vector I in global coordinates.
Rect2d2dCooSys::VGlobal Rect2d2dCooSys::getIVector(void) const
  { return getAxisVDir(1); }
//! @brief Return the unary vector J in global coordinates.
Rect2d2dCooSys::VGlobal Rect2d2dCooSys::getJVector(void) const
  { return getAxisVDir(2); }

//! @brief Set the unary vector I.
void Rect2d2dCooSys::PutI(const VGlobal &i)
  {
    put(1,1,i(1));
    put(1,2,i(2));
    put(2,1,-i(2)); //Para que sea ortogonal y dextrógiro.
    put(2,2,i(1)); //Para que sea ortogonal y dextrógiro.
  }
//! @brief Asigna el vector unitario I.
void Rect2d2dCooSys::PutJ(const VGlobal &j)
  {
    put(2,1,j(1));
    put(2,2,j(2));
    put(1,1,j(2)); //Para que sea ortogonal y dextrógiro.
    put(1,2,-j(1)); //Para que sea ortogonal y dextrógiro.
  }

//! @brief Return the global coordinates of the vector.
//!
//! @param v: local coordinates of the vector.
Rect2d2dCooSys::VGlobal Rect2d2dCooSys::getGlobalCoordinates(const VLocal &v) const
  { return Xd2dCooSys::getGlobalCoordinates(v.getMatrix()); }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: global coordinates of the vector.
Rect2d2dCooSys::VLocal Rect2d2dCooSys::getLocalCoordinates(const VGlobal &v) const
  {
    const FT_matrix tmp= Xd2dCooSys::getLocalCoordinates(v);
    return VLocal(tmp(1),tmp(2)); 
  }

