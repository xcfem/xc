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
//Ref2d3d.cc
#include "Ref2d3d.h"
#include "Ref3d3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Dir3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d2/Plane.h"

//! @brief Constructor.
Ref2d3d::Ref2d3d(void)
  : BaseRef() {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Pos3d &o)
  : BaseRef(o) {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Pos3d &o,const Rect2d3dCooSys &sc)
  : BaseRef(o,sc) {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Pos3d &o,const Pos3d &p,const Pos3d &q)
  : BaseRef(o,Rect2d3dCooSys(o,p,q)) {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Pos3d &o,const Vector3d &v)
  : BaseRef(o,Rect2d3dCooSys(v)) {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2)
  : BaseRef(o,Rect2d3dCooSys(v1,v2)) {}

//! @brief Constructor.
Ref2d3d::Ref2d3d(const Ref3d3d &r3d3d)
  : BaseRef(r3d3d.Org(),Rect2d3dCooSys(r3d3d.getIVector(),r3d3d.getJVector())) {}

//! @brief Return the unary vector I expressed in global coordinates.
Vector3d Ref2d3d::getIVector(void) const
  { return getAxisVDir(1); }

//! @brief Return the unary vector J expressed in global coordinates.
Vector3d Ref2d3d::getJVector(void) const
  { return getAxisVDir(2); }

//! @brief Return the unary vector K expressed in global coordinates.
Vector3d Ref2d3d::getKVector(void) const
  { return cross(getIVector(), getJVector()); }

//! @brief Return the line defining the x axis.
Line3d Ref2d3d::getXAxis(void) const
  {
    const Pos3d dest(org+1000.0*getIVector());
    return Line3d(org,dest);
  }

//! @brief Return the line defining the y axis.
Line3d Ref2d3d::getYAxis(void) const
  {
    const Pos3d dest(org+1000.0*getJVector());
    return Line3d(org,dest);
  }

Plane Ref2d3d::getXYPlane(void) const
  { return Plane(org,1000.0*getIVector(),1000.0*getJVector()); }

