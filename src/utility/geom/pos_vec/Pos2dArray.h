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
//Pos2dArray

#ifndef POS2DARRAY_H
#define POS2DARRAY_H

#include "Pos2d.h"
#include "PosArray.h"

class Triangle2d;
class Quadrilateral2d;
class FT_matrix;
class Trf2d;


//! @ingroup GEOM
//
//! @brief Array of positions in a two-dimensional space.
class Pos2dArray: public PosArray<Pos2d>
  {
    friend class Pos2dArray3d;
    typedef PosArray<Pos2d> m_pos;
    Pos2d pos_lagrangiana(const size_t &i,const size_t &j) const;
    GEOM_FT dist_lagrange(void) const;
    GEOM_FT ciclo_lagrange(void);
    
  public:
    Pos2dArray(const size_t &f=1,const size_t &c=1,const Pos2d &p=Pos2d()): m_pos(f,c,p) {}
    Pos2dArray(const Pos2d &p1,const Pos2d &p2,const size_t &num,const GEOM_FT &ratio);
    Pos2dArray(const Pos2d &p1,const Pos2d &p2,const size_t &ndiv);
    Pos2dArray(const Pos2d &p1,const Pos2d &p2,const std::vector<GEOM_FT> &longs);
    Pos2dArray(const Pos2d &p0,const Pos2d &p1,const Pos2d &p2,const size_t &ndiv1,const size_t &ndiv2);
    Pos2dArray(const Pos2dArray &,const Pos2dArray &,const Pos2dArray &,const Pos2dArray &);
 
    size_t GetNumQuads(void) const;
    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    GEOM_FT getIx(void) const;
    GEOM_FT getIy(void) const;
    GEOM_FT GetPxy(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Pxy(void) const;
    Pos2d getCenter(void) const;
    Pos2d getCenterOfMass(void) const;
    GEOM_FT GetArea(void) const;
    Triangle2d getTriangle1(const size_t &i,const size_t &j) const;
    Triangle2d getTriangle2(const size_t &i,const size_t &j) const;
    GEOM_FT GetX(const size_t &,const size_t &) const;
    GEOM_FT GetY(const size_t &,const size_t &) const;
    const FT_matrix &GetVertCoords(const size_t &,const size_t &) const;
    Pos2d getPoint(const size_t &,const size_t &) const;
    Quadrilateral2d GetQuad(const size_t &i,const size_t &j) const;
    double GetAreaQuad(const size_t &i,const size_t &j) const;
    Pos2d getQuadCentroid(const size_t &i,const size_t &j) const;
    bool In(const Pos2d &, const double &) const;

    void Transform(const Trf2d &trf2d);

    GEOM_FT Lagrange(const GEOM_FT &tol);
  };

#endif
