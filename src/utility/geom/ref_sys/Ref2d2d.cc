//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Ref2d2d.cc
#include "Ref2d2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/d1/Line2d.h"



Ref2d2d::Ref2d2d(void): BaseRef() {}
Ref2d2d::Ref2d2d(const Pos2d &o) : BaseRef(o) {}
Ref2d2d::Ref2d2d(const Pos2d &o,const Rect2d2dCooSys &sc): BaseRef(o,sc) {}
Ref2d2d::Ref2d2d(const Pos2d &o,const Vector2d &vX): BaseRef(o,vX) {}
Ref2d2d::Ref2d2d(const Pos2d &o,const Dir2d &dirX): BaseRef(o,dirX) {}
Ref2d2d::Ref2d2d(const Pos2d &o,const Pos2d &p): BaseRef(o,p) {}

//! @brief Return the unary vector I expressed in global coordinates.
Vector2d Ref2d2d::getIVector(void) const
  { return getAxisVDir(1); }
//! @brief Return the unary vector J expressed in global coordinates.
Vector2d Ref2d2d::getJVector(void) const
  { return getAxisVDir(2); }

//! @brief Return la line defining the x axis.
Line2d Ref2d2d::getXAxis(void) const
  {
    const Pos2d dest(org+1000.0*getIVector());
    return Line2d(org,dest);
  }
//! @brief Return la line defining the y axis.
Line2d Ref2d2d::getYAxis(void) const
  {
    const Pos2d dest(org+1000.0*getJVector());
    return Line2d(org,dest);
  }

