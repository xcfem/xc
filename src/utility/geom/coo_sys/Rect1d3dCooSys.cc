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
//Rect1DCooSys.cc

#include "Rect1d3dCooSys.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Dir3d.h"
#include "../pos_vec/Pos3d.h"



Rect1d3dCooSys::Rect1d3dCooSys(const VGlobal &vX)
  : Xd3dCooSys(1,vX)  //Axis 1 parallelo to Vx.
  {}
Rect1d3dCooSys::Rect1d3dCooSys(const PGlobal &o,const PGlobal &p)
  : Xd3dCooSys(1,o,p) {}

//! @brief Return the unary vector I in global coordinates.
Rect1d3dCooSys::VGlobal Rect1d3dCooSys::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Return the global coordinates of the vector.
//!
//! @param v: local coordinates of the vector.
Rect1d3dCooSys::VGlobal Rect1d3dCooSys::GetCooGlobales(const VLocal &v) const
  { return Xd3dCooSys::GetCooGlobales(FT_matrix(1,1,v)); }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: global coordinates of the vector.
Rect1d3dCooSys::VLocal Rect1d3dCooSys::GetCooLocales(const Rect1d3dCooSys::VGlobal &v) const
  {
    const FT_matrix tmp= Xd3dCooSys::GetCooLocales(v);
    return VLocal(tmp(1));
  }

