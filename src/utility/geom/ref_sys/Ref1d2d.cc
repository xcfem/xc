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

//Ref1d2d.cc
#include "Ref1d2d.h"
#include "Ref2d2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/d1/Line2d.h"

Ref1d2d::Ref1d2d(void): BaseRef() {}
Ref1d2d::Ref1d2d(const Pos2d &o) : BaseRef(o) {}
Ref1d2d::Ref1d2d(const Pos2d &o,const Rect1d2dCooSys &sc): BaseRef(o,sc) {}
Ref1d2d::Ref1d2d(const Pos2d &o,const Vector2d &vX): BaseRef(o,vX) {}
Ref1d2d::Ref1d2d(const Pos2d &o,const Dir2d &dirX): BaseRef(o,dirX) {}
Ref1d2d::Ref1d2d(const Pos2d &o,const Pos2d &p): BaseRef(o,p) {}

//! @brief Constructor.
Ref1d2d::Ref1d2d(const Ref2d2d &r2d2d)
  : BaseRef(r2d2d.Org(),Rect1d2dCooSys(r2d2d.getIVector())) {}

//! @brief Return the unary vector I in the global system.
Vector2d Ref1d2d::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Return the line defining the x axis.
Line2d Ref1d2d::getXAxis(void) const
  {
    const Pos2d dest(org+1000.0*getIVector());
    return Line2d(org,dest);
  }

