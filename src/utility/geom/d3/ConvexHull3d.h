// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//ConvexHull3d.h

#ifndef CONVEXHULL3D_H
#define CONVEXHULL3D_H

#include "utility/geom/d3/GeomObj3d.h"

class Polyhedron3d;


//! @ingroup GEOM
//
//! @brief Return el «convex hull» del conjunto de posiciones
//! que se pasa como parámetro.
Polyhedron3d get_convex_hull(const GeomObj::list_Pos3d &lp);


#endif
