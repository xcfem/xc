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
//Rect1d2dCooSys.cc

#include "Rect1d2dCooSys.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Dir2d.h"

#include "utility/utils/text/text_string.h"
#include "utility/functions/algebra/ExprAlgebra.h"


Rect1d2dCooSys::Rect1d2dCooSys(const VGlobal &vX)
  : Xd2dCooSys(1,vX) {} //Axis 1 parallel to Vx.
Rect1d2dCooSys::Rect1d2dCooSys(const PGlobal &p1,const PGlobal &p2)
  : Xd2dCooSys(1,p1,p2) {} //Axis 1 desde p1 a p2.

//! @brief Return unary vector I in global coordinates.
Rect1d2dCooSys::VGlobal Rect1d2dCooSys::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Return the global coordinates components of the vector from
//! the local ones.
//!
//! @param v: local coordinates of the vector.
Rect1d2dCooSys::VGlobal Rect1d2dCooSys::GetCooGlobales(const VLocal &v) const
  { return Xd2dCooSys::GetCooGlobales(FT_matrix(1,1,v)); }

//! @brief Return the local coordinates of the vector from the
//! global ones.
//!
//! @param v: local coordinates of the vector.
Rect1d2dCooSys::VLocal Rect1d2dCooSys::GetCooLocales(const Rect1d2dCooSys::VGlobal &v) const
  {
    const FT_matrix tmp= Xd2dCooSys::GetCooLocales(v);
    return tmp(1,1);
  }

