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
//Pos2d.h

#ifndef POS2D_H
#define POS2D_H


#include <string>
#include "../cgal_types.h"
#include "../ProtoGeom.h"

class Vector2d;
class Plotter;
class Line2d;
class Ray2d;
class Segment2d;

//! @ingroup GEOM
//
//! @brief Posición en dos dimensiones.
class Pos2d: public ProtoGeom
  {
    CGPoint_2 cgpt;
  public:
    typedef Vector2d vector;

    Pos2d(void);
    explicit Pos2d(const CGPoint_2 &p);
    Pos2d(const GEOM_FT &Ax,const GEOM_FT &Ay);
    Pos2d(const GEOM_RT &x,const GEOM_RT &y,const GEOM_RT &h);
    
    bool notAPoint(void) const;
    Pos2d &operator+=(const Vector2d &v);
    Pos2d &operator-=(const Vector2d &v);
    Vector2d operator-(const Pos2d &) const;
    Pos2d operator-(const Vector2d &) const;
    Pos2d operator+(const Vector2d &) const;
    virtual bool operator==(const Pos2d &) const;
    inline bool operator!=(const Pos2d &other) const
      { return !(*this==other); }
    const CGPoint_2 &ToCGAL(void) const
      { return cgpt; }
    inline GEOM_FT operator()(const size_t &i) const //Base 1
      { return cgpt.cartesian(i-1); }
    inline GEOM_FT at0(const size_t &j) const //Base 0
      { return cgpt.cartesian(j); }
    inline GEOM_FT operator[](const size_t &j) const //Base 0
      { return at0(j); }
    inline GEOM_FT x() const
      { return Pos2d::operator()(1); }
    inline GEOM_FT y() const
      { return Pos2d::operator()(2); }
    inline const GEOM_RT hx() const
      { return cgpt.hx(); }
    inline const GEOM_RT hy() const
      { return cgpt.hy(); }
    inline const GEOM_RT hw() const
      { return cgpt.hw(); }

    void SetX(const GEOM_FT &vx);
    void SetY(const GEOM_FT &vy);
    void Set(unsigned short int i,const GEOM_FT &v);

    size_t dimension(void) const
      { return cgpt.dimension(); }
    Vector2d VectorPos(void) const;
    bool domina_a(const Pos2d &) const;
    GEOM_FT dist(const Pos2d &) const;
    GEOM_FT dist2(const Pos2d &) const;
    GEOM_FT dist2(const Line2d &) const;
    GEOM_FT dist(const Line2d &) const;
    GEOM_FT dist2(const Ray2d &) const;
    GEOM_FT dist(const Ray2d &) const;
    GEOM_FT dist2(const Segment2d &) const;
    GEOM_FT dist(const Segment2d &) const;

    friend bool colineales(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3);

    void Print(std::ostream &os) const;
    void Plot(Plotter &psos) const;
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
  };

std::ostream &operator << (std::ostream &stream,const Pos2d &n);

inline GEOM_FT dist2(const Pos2d &p1,const Pos2d &p2)
  { return p1.dist2(p2); }

inline GEOM_FT dist(const Pos2d &p1,const Pos2d &p2)
  { return p1.dist(p2); }

inline Pos2d pos_max(const Pos2d &a,const Pos2d &b)
{ return Pos2d(std::max(a.x(),b.x()),std::max(a.y(),b.y())); }
inline Pos2d pos_min(const Pos2d &a,const Pos2d &b)
  { return Pos2d(std::min(a.x(),b.x()),std::min(a.y(),b.y())); }

const Pos2d Origin2d;

#endif



