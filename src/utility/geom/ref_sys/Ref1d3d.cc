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
//Ref1d3d.cc
#include "Ref1d3d.h"
#include "Ref2d3d.h"
#include "Ref3d3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Dir3d.h"
#include "utility/geom/d1/Line3d.h"



Ref1d3d::Ref1d3d(void): BaseRef() {}
Ref1d3d::Ref1d3d(const Pos3d &o): BaseRef(o) {}
Ref1d3d::Ref1d3d(const Pos3d &o,const Rect1d3dCooSys &sc): BaseRef(o,sc) {}
Ref1d3d::Ref1d3d(const Pos3d &o,const Vector3d &vX): BaseRef(o,vX) {}
Ref1d3d::Ref1d3d(const Pos3d &o,const Dir3d &dirX): BaseRef(o,dirX) {}
Ref1d3d::Ref1d3d(const Pos3d &o,const Pos3d &p) : BaseRef(o,p) {}

//! @brief Constructor.
Ref1d3d::Ref1d3d(const Ref2d3d &r2d3d)
  : BaseRef(r2d3d.Org(),Rect1d3dCooSys(r2d3d.getIVector())) {}

//! @brief Constructor.
Ref1d3d::Ref1d3d(const Ref3d3d &r3d3d)
  : BaseRef(r3d3d.Org(),Rect1d3dCooSys(r3d3d.getIVector())) {}

//! @brief Return the global coordinates of unit vector I.
Vector3d Ref1d3d::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Returns the line defined by X axis.
Line3d Ref1d3d::getXAxis(void) const
  {
    const Pos3d dest(org+1000.0*getIVector());
    return Line3d(org,dest);
  }

