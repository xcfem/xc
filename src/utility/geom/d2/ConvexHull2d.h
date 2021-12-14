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
//ConvexHull2d.h

#ifndef CONVEXHULL2D_H
#define CONVEXHULL2D_H

#include "utility/geom/d2/GeomObj2d.h"

class Polygon2d;

//! @ingroup GEOM
//
//! @brief Return the «convex hull» of the point set argument.
Polygon2d get_convex_hull2d(const GeomObj::list_Pos2d &);

//! @ingroup GEOM
//
//! @brief Return the «convex hull» of the point set argument.
Polygon2d get_convex_hull2d(const boost::python::list &);

#endif
