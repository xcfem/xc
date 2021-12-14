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
//Pos3dArray

#ifndef POS3DARRAY_H
#define POS3DARRAY_H

#include "Pos3d.h"
#include "PosArray.h"

class Triangle3d;
class BND3d;
class Trf3d;
class Revolution3d;

//! @ingroup GEOM
//
//! @brief Array of positions in a three-dimensional space.
class Pos3dArray: public PosArray<Pos3d>
  {
    Pos3dArray(const PosArray<Pos3d> &mp3d);
    friend class Pos3dArray3d;
    Pos3d pos_lagrangiana(const size_t &i,const size_t &j) const;
    GEOM_FT dist_lagrange(void) const;
    GEOM_FT ciclo_lagrange(void);
  public:
    Pos3dArray(const size_t &f=1,const size_t &c=1,const Pos3d &p= Pos3d());
    Pos3dArray(const Pos3d &p1,const Pos3d &p2,const size_t &num,const GEOM_FT &ratio);
    Pos3dArray(const Pos3d &p1,const Pos3d &p2,const size_t &ndiv);
    Pos3dArray(const Pos3d &p1,const Pos3d &p2,const std::vector<GEOM_FT> &longs);
    Pos3dArray(const Pos3d &p0,const Pos3d &p1,const Pos3d &p2,const size_t &ndiv1,const size_t &ndiv2);
    Pos3dArray(const Pos3dArray &l1_points,const Pos3dArray &l2_points,const Pos3dArray &l3_points,const Pos3dArray &l4_points);
    Pos3dArray(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4,const size_t &ndiv1,const size_t &ndiv2);
    Pos3d getCenter(void) const;
    Triangle3d getTriangle1(const size_t &i,const size_t &j) const;
    Triangle3d getTriangle2(const size_t &i,const size_t &j) const;
    GEOM_FT Lagrange(const GEOM_FT &tol);
    void Transform(const Trf3d &trf);
  };

GEOM_FT dist2(const Pos3dArray &ptos,const Pos3d &pt);
bool dist_menor(const Pos3dArray &ptos,const Pos3d &pt,const GEOM_FT &d_max);
GEOM_FT dist(const Pos3dArray &ptos,const Pos3d &pt);
GEOM_FT pseudo_dist2(const Pos3dArray &ptos,const Pos3d &pt);
GEOM_FT pseudo_dist(const Pos3dArray &ptos,const Pos3d &pt);

BND3d get_bnd(const Pos3dArray &ptos);

Pos3dArray create_revolution_surface(const Revolution3d &r,const Pos3dArray &m);
Pos3dArray Quadrilateral(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4,const size_t &ndiv1,const size_t &ndiv2);

#endif
