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
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//VectorPos3d

#ifndef VECTORPOS3D_H
#define VECTORPOS3D_H

#include "Pos3d.h"
#include "VectorPos.h"

//! @ingroup GEOM
//
//! @brief Vector de posiciones en tres dimensiones [(x1,y1,z1),(x1,y2,z2),...].
class VectorPos3d: public VectorPos<Pos3d>
  {
  public:
    VectorPos3d(const size_t &f=1): VectorPos<Pos3d>(f) {}
    VectorPos3d(const Pos3d &p1,const Pos3d &p2,const size_t &num,const GEOM_FT &ratio)
      : VectorPos<Pos3d>(p1,p2,num,ratio) {}
    VectorPos3d(const Pos3d &p1,const Pos3d &p2,const size_t &ndiv)
      : VectorPos<Pos3d>(p1,p2,ndiv) {}
  };

#endif
