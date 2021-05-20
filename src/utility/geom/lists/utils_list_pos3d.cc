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
//utils_list_pos3d.cc

#include "utils_list_pos3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/trf/Trf3d.h"
#include "utility/geom/d3/BND3d.h"


//! @brief Moves the points of the list.
void move(GeomObj::list_Pos3d &l,const Vector3d &v)
  {
    for(GeomObj::list_Pos3d::iterator j=l.begin();j != l.end();j++)
      (*j)= (*j) + v;
  }

//! @brief Applies the transformation to the positions.
void transform(GeomObj::list_Pos3d &l,const Trf3d &t)
  { t.Transform(l.begin(),l.end()); }

//! @brief Return the upper right corner of the bounding rectangle.
Pos3d getPMax(const GeomObj::list_Pos3d &l)
  { return Pos3d(l.GetMax(1),l.GetMax(2),l.GetMax(3)); }

//! @brief Return the lower left cornerof the bounding rectangle.
Pos3d getPMin(const GeomObj::list_Pos3d &l)
  { return Pos3d(l.GetMin(1),l.GetMin(2),l.GetMin(3)); }

//! @brief Return the bounding rectangle. 
BND3d getBnd(const GeomObj::list_Pos3d &l)
  { return BND3d(getPMin(l),getPMax(l)); }

