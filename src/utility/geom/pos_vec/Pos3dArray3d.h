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
//Pos3dArray3d

#ifndef ARRAY_3DPOS3D_H
#define ARRAY_3DPOS3D_H

#include "Pos3d.h"
#include "PosArray3d.h"

class Pos3dArray;
class Trf3d;
class Revolution3d;


//! @ingroup GEOM
//
//! @brief Position array in a three-dimensional space.
class Pos3dArray3d: public PosArray3d<Pos3d>
  {
  public:
    Pos3d getCenter(void) const;
    Pos3dArray3d(const size_t &iLayers= 1);
    Pos3dArray3d(const size_t &,const PosArray<Pos3d> &);
    Pos3dArray3d(const PosArray<Pos3d> &l1_points,const PosArray<Pos3d> &l2_points,
                 const PosArray<Pos3d> &l3_points,const PosArray<Pos3d> &l4_points,
                 const size_t &ndiv_12,const size_t &ndiv_13);
    Pos3dArray3d(const Pos3dArray &l1_points,const Pos3dArray &l2_points,
                 const Pos3dArray &l3_points,const Pos3dArray &l4_points,
                 const size_t &ndiv_12,const size_t &ndiv_13);
    void Transform(const Trf3d &trf);
    Pos3dArray3d Transform(const Trf3d &trf) const;
  };

class BND3d;

Pos3dArray3d create_uniform_grid(const BND3d &bnd,const size_t &ndiv_x,const size_t &ndiv_y,const size_t &ndiv_z);
Pos3dArray3d create_revolution_volume(const Revolution3d &r,const Pos3dArray &m);

#endif
