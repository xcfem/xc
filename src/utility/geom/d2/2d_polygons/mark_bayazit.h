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
//Mark_Bayazit.h

#ifndef MARK_BAYAZIT_H
#define MARK_BAYAZIT_H

//Credits: The algorithm for decomposing concave polygons to convex can be found here: https://mpen.ca/406/bayazit (Mark Bayazit).

#include "../../cgal_types.h"

class Pos2d;
class Polygon2d;

GEOM_FT area(const Pos2d &a, const Pos2d &b, const Pos2d &c);
bool left(const Pos2d &a, const Pos2d &b, const Pos2d &c);
bool leftOn(const Pos2d &a, const Pos2d &b, const Pos2d &c);
bool right(const Pos2d &a, const Pos2d &b, const Pos2d &c);
bool rightOn(const Pos2d &a, const Pos2d &b, const Pos2d &c);
bool collinear(const Pos2d &a, const Pos2d &b, const Pos2d &c);
GEOM_FT sqdist(const Pos2d &a, const Pos2d &b);
void decompose_poly(const Polygon2d &poly, std::list<Polygon2d> &result);
bool isReflex(const Polygon2d &p, const int &i);

#endif







