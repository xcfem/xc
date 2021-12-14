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
//Triangle3d.cc

#include "Triangle3d.h"
#include "utility/geom/d2/2d_polygons/Triangle2d.h"
#include "../Wm3/Wm3DistVector3Triangle3.h"


Triangle3d::Triangle3d(const Ref2d3d &rf,const Triangle2d &t)
  : Polygon3d(rf,t.getPolygon()) {}
Triangle3d::Triangle3d(void): Polygon3d() {}
Triangle3d::Triangle3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : Polygon3d(p1,p2,p3) {}

//! @brief Return the squared distance from the point to the triangle.
GEOM_FT Triangle3d::dist2(const Pos3d &p) const
  {
    Wm3::DistVector3Triangle3ft d(p.VectorPos(),*this);
    return d.GetSquared();
  }

//! @brief Return the distance from the point to the triangle.
GEOM_FT Triangle3d::dist(const Pos3d &p) const
  {
    Wm3::DistVector3Triangle3ft d(p.VectorPos(),*this);
    return d.Get();
  }
