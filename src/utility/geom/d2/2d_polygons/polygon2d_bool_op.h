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
//polygon2d_bool_op.h

#ifndef POLYGON2D_BOOL_OP_H
#define POLYGON2D_BOOL_OP_H

#include "utility/geom/d2/2d_polygons/PolygonalSurface2d.h"
#include <list>

class Polygon2d;
class HalfPlane2d;
class Line2d;

Polygon2d prueba_conversion(const Polygon2d &p);

std::list<Polygon2d> join(const Polygon2d &p1,const Polygon2d &p2);
std::list<Polygon2d> join(const std::list<Polygon2d> &l);
std::list<Polygon2d> join(const std::list<Polygon2d> &,const Polygon2d &);

bool overlap(const Polygon2d &p1,const Polygon2d &p2);
bool overlap(const std::list<Polygon2d> &,const std::list<Polygon2d> &);

std::list<Polygon2d> clip(const std::list<Polygon2d> &,const Polygon2d &);

std::list<Polygon2d> intersection(const Polygon2d &p1,const Polygon2d &p2);
std::list<Polygon2d> intersection(const Polygon2d &p,const HalfPlane2d &r);
std::list<Polygon2d> intersection(const std::list<Polygon2d> &,const HalfPlane2d &);
std::list<Polygon2d> intersection(const std::list<Polygon2d> &,const Polygon2d &);
std::list<Polygon2d> intersection(const std::list<Polygon2d> &,const std::list<Polygon2d> &);

void particiona(const Pos2d &c1,Polygon2d &p1,const Pos2d &c2,Polygon2d &p2);
void particiona(Polygon2d &p1,Polygon2d &p2);
void particiona(const Pos2d &,std::list<Polygon2d> &,const Pos2d &,std::list<Polygon2d> &);
void particiona(const std::list<Pos2d> &,std::list<Polygon2d> &);
void particiona(std::list<Polygon2d> &);

#endif
