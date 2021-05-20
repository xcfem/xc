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
//Pos2dArray3d

#ifndef POS2DARRAY3D_H
#define POS2DARRAY3D_H

#include "Pos2d.h"
#include "PosArray3d.h"

class Pos2dArray;


//! @ingroup GEOM
//
//! @brief Array of positions ina two-dimensional space.
class Pos2dArray3d: public PosArray3d<Pos2d>
  {
  public:
    Pos2dArray3d(const size_t iLayers= 1);
    Pos2d getCenter(void) const;
  };

class BND2d;

Pos2dArray3d create_uniform_grid(const BND2d &bnd,const size_t &ndiv_x,const size_t &ndiv_y,const size_t &ndiv_z= 0);

#endif
