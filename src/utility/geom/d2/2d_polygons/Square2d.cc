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
//Square2d.cc

#include "Square2d.h"

#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d1/Ray2d.h"
#include "utility/geom/d1/Segment2d.h"


Square2d::Square2d(const Pos2d &o,const GEOM_FT &l)
  : Quadrilateral2d(o,o+Vector2d(l,0),o+Vector2d(l,l),o+Vector2d(0,l)) {}

