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
//Ref3d3d.cc
#include "Ref3d3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d2/Plane.h"

//! @brief Constructor.
Ref3d3d::Ref3d3d(void): BaseRef() {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &o) : BaseRef(o) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &o,const Rect3d3dCooSys &sc)
  : BaseRef(o,sc) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &o,const Pos3d &p) : BaseRef(o,p) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : BaseRef(p1,Rect3d3dCooSys(p1,p2,p3)) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2)
  : BaseRef(o,Rect3d3dCooSys(v1,v2)) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2,const Vector3d &v3)
  : BaseRef(o,Rect3d3dCooSys(v1,v2,v3)) {}

//! @brief Constructor.
Ref3d3d::Ref3d3d(const Line3d &r,const Pos3d &p)
  : BaseRef(r.Point(0),Rect3d3dCooSys(r.Point(0),r.Point(100),p)) {}

Ref3d3d::~Ref3d3d(void)
  {}

//! @brief Return the unary vector I expressed in global coordinates.
Vector3d Ref3d3d::getIVector(void) const
  { return getAxisVDir(1); }
//! @brief Return the unary vector J expressed in global coordinates.
Vector3d Ref3d3d::getJVector(void) const
  { return getAxisVDir(2); }
//! @brief Return the unary vector K expressed in global coordinates.
Vector3d Ref3d3d::getKVector(void) const
  { return getAxisVDir(3); }

//! @brief Return the line defining the x axis.
Line3d Ref3d3d::getXAxis(void) const
  {
    const Pos3d dest(org+1000.0*getIVector());
    return Line3d(org,dest);
  }

//! @brief Return the line defining the y axis.
Line3d Ref3d3d::getYAxis(void) const
  {
    const Pos3d dest(org+1000.0*getJVector());
    return Line3d(org,dest);
  }

//! @brief Return the line defining the z axis.
Line3d Ref3d3d::getZAxis(void) const
  {
    const Pos3d dest(org+1000.0*getKVector());
    return Line3d(org,dest);
  }

Plane Ref3d3d::getXYPlane(void) const
  { return Plane(org,1000.0*getIVector(),1000.0*getJVector()); }
Plane Ref3d3d::getXZPlane(void) const
  { return Plane(org,1000.0*getIVector(),1000.0*getKVector()); }
Plane Ref3d3d::getYZPlane(void) const
  { return Plane(org,1000.0*getJVector(),1000.0*getKVector()); }

// GEOM_FT Ref3d3d::GetZLocal(const Pos3d &p) const
// //Return the local Z of the point p expressed in local coordinates
//   {
//     m_double inv(trf.getColumn(3));
//     inv.Trn();
//     return dot(inv,(p - org));
//   }

