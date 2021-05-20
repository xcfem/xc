// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//!
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//utils_list_pos2d.h

#ifndef UTILS_LIST_POS2D_H
#define UTILS_LIST_POS2D_H

#include "utility/geom/GeomObj.h"

class BND2d;
class Polygon2d;

//Auxiliary functions for manipulation of position lists.

//! @ingroup GEOM
//
void move(GeomObj::list_Pos2d &l,const Vector2d &);
void transform(GeomObj::list_Pos2d &l,const Trf2d &);
void plot(Plotter &,const GeomObj::list_Pos2d &l);
Pos2d getPMax(const GeomObj::list_Pos2d &l);
Pos2d getPMin(const GeomObj::list_Pos2d &l);
BND2d getBnd(const GeomObj::list_Pos2d &l);
std::deque<GEOM_FT> &getCovers(const GeomObj::list_Pos2d &l,const Polygon2d &);
#endif

