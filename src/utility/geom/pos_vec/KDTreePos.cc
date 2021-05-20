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
//KDTreePos.cc

#include "KDTreePos.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
KDTreePos::KDTreePos(const Pos3d &p)
  { d[0] = p.x(); d[1] = p.y(); d[2] = p.z(); }
  
//! @brief Distance between positions.
double KDTreePos::distance_to(const KDTreePos &other) const
  {
    double dist = sqr(d[0]-other.d[0]);
    dist+= sqr(d[1]-other.d[1]);
    dist+= sqr(d[2]-other.d[2]);
    return sqrt(dist);
  }
