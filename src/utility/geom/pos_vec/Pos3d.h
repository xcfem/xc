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
//Pos3d.h

#ifndef POS3D_H
#define POS3D_H


#include <iostream>
#include <string>
#include "../cgal_types.h"
#include "../ProtoGeom.h"

class Vector3d;
class Pos2d;
class Line3d;
class Ray3d;
class Segment3d;
class Plane;
class HalfSpace3d;
class Solid3d;

//! @ingroup GEOM
//
//! @brief Posición en tres dimensiones.
class Pos3d : public ProtoGeom
  {
    CGPoint_3 cgpt;
  public:
    typedef Vector3d vector;
    typedef CGPoint_3::Cartesian_const_iterator Cartesian_const_iterator;

    Pos3d(void)
      : ProtoGeom(), cgpt(CGAL::ORIGIN) {}
    Pos3d(const CGPoint_3 &p)
      : ProtoGeom(), cgpt(p) {}
    Pos3d(const GEOM_FT &Ax,const GEOM_FT &Ay,const GEOM_FT &Az)
      : ProtoGeom(), cgpt(Ax,Ay,Az) {}
/*     Pos3d(const double &Ax,const double &Ay,const double &Az) */
/*       : ProtoGeom(), cgpt(Point_3_from_doubles(Ax,Ay,Az)) {} */

    Pos3d &operator+=(const Vector3d &);
    Pos3d &operator-=(const Vector3d &);
    Vector3d operator-(const Pos3d &) const;
    Pos3d operator-(const Vector3d &) const;
    Pos3d operator+(const Vector3d &) const;
    virtual bool operator==(const Pos3d &) const;
    inline bool operator!=(const Pos3d &other) const
      { return !(*this==other); }
    const CGPoint_3 &ToCGAL(void) const
      { return cgpt; }
    GEOM_FT at0(const size_t &i) const;
    GEOM_FT operator()(const size_t &i) const;
    inline const GEOM_FT x() const
      { return Pos3d::operator()(1); }
    inline const GEOM_FT y() const
      { return Pos3d::operator()(2); }
    inline const GEOM_FT z() const
      { return Pos3d::operator()(3); }
    inline const GEOM_RT hx() const
      { return cgpt.hx(); }
    inline const GEOM_RT hy() const
      { return cgpt.hy(); }
    inline const GEOM_RT hz() const
      { return cgpt.hz(); }
    inline const GEOM_RT hw() const
      { return cgpt.hw(); }

    void SetX(const GEOM_FT &vx);
    void SetY(const GEOM_FT &vy);
    void SetZ(const GEOM_FT &vz);
    void Set(unsigned short int i,const GEOM_FT &v);

    inline Cartesian_const_iterator cartesian_begin(void) const
      { return ToCGAL().cartesian_begin(); }
    inline Cartesian_const_iterator cartesian_end(void) const
      { return ToCGAL().cartesian_end(); }
    size_t dimension(void) const
      { return cgpt.dimension(); }
    Vector3d VectorPos(void) const;
    bool domina_a(const Pos3d &b) const;

    GEOM_FT dist(const Pos3d &) const;
    GEOM_FT dist2(const Pos3d &) const;
    GEOM_FT dist2(const Line3d &) const;
    GEOM_FT dist(const Line3d &) const;
    GEOM_FT dist2(const Ray3d &) const;
    GEOM_FT dist(const Ray3d &) const;
    GEOM_FT dist2(const Segment3d &) const;
    GEOM_FT dist(const Segment3d &) const;
    GEOM_FT dist2(const Plane &) const;
    GEOM_FT dist(const Plane &) const;
    GEOM_FT dist2(const HalfSpace3d &) const;
    GEOM_FT dist(const HalfSpace3d &) const;
    GEOM_FT dist(const Solid3d &) const;

    int Cuadrante(void) const;

    Pos3d XY3DProjection(void) const; //Return projection onto XY plane.
    Pos3d XZ3DProjection(void) const; //Return projection onto XZ plane.
    Pos3d YZ3DProjection(void) const; //Return projection onto YZ plane.

    Pos2d XY2DProjection(void) const; //Return projection onto XY plane.
    Pos2d XZ2DProjection(void) const; //Return projection onto XZ plane.
    Pos2d YZ2DProjection(void) const; //Return projection onto YZ plane.

    Pos3d getModArgX(const GEOM_FT &mod,const GEOM_FT &giroX) const;
    Pos3d getModArgY(const GEOM_FT &mod,const GEOM_FT &giroY) const;
    Pos3d getModArgZ(const GEOM_FT &mod,const GEOM_FT &giroZ) const;

    friend bool colineales(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    friend bool coplanarios(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4);
    friend std::ostream &operator<<(std::ostream &stream,const Pos3d &n);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
  };

Pos3d To3dXY2d(const Pos2d &p,const GEOM_FT &z= 0);
Pos3d To3dXZ2d(const Pos2d &p,const GEOM_FT &y= 0);
Pos3d To3dYZ2d(const Pos2d &p,const GEOM_FT &x= 0);
Pos3d To3d(const Pos2d &p,const size_t &coo,const GEOM_FT &u=0);


inline GEOM_FT dist2(const Pos3d &p1,const Pos3d &p2)
  { return p1.dist2(p2); }

inline GEOM_FT dist(const Pos3d &p1,const Pos3d &p2)
  { return p1.dist(p2); }

inline Pos3d pos_max(const Pos3d &a,const Pos3d &b)
  { return Pos3d(std::max(a.x(),b.x()),std::max(a.y(),b.y()),std::max(a.z(),b.z())); }
inline Pos3d pos_min(const Pos3d &a,const Pos3d &b)
  { return Pos3d(std::min(a.x(),b.x()),std::min(a.y(),b.y()),std::min(a.z(),b.z())); }

const Pos3d Origin3d;

#endif
