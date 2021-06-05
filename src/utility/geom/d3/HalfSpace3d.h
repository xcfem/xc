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
//HalfSpace3d.h

#ifndef HALFSPACE3D_H
#define HALFSPACE3D_H


#include "utility/geom/d2/Plane.h"
#include "utility/utils/stl/TripletMap.h"



//! @ingroup GEOM
//
//! @brief The points of the half space are those that line
//! at the back of the plane, this being understood as those that
//! return a zero or negative value when evaluating on them the
//! general equation of the plane, i. e.:
//!
//! A*x_i+B*y_i+C*z_i+D <= 0
//!
//! with the point coordinates: (x_i,y_i,z_i).
class HalfSpace3d : public GeomObj3d
  {
    Plane lim; 
  public:
    HalfSpace3d(const Plane &p= Plane());
    HalfSpace3d(const Plane &, const Pos3d &);
    virtual bool operator==(const HalfSpace3d &) const;
    
    const Plane &getBoundaryPlane(void) const;
    void setBoundaryPlane(const Plane &);
    
    virtual GeomObj *clon(void) const
      { return new HalfSpace3d(*this); }
    inline void swap(void)
      { lim.swap(); }
    HalfSpace3d getSwap(void) const;
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int) const
      { return NAN; }
    Vector3d NormalExterior(void) const;
    Vector3d NormalInterior(void) const;
    inline const Plane &getPlane(void) const
      { return lim; }

    virtual bool In(const Pos3d &, const double &tol= 0.0) const;
    virtual bool In(const Line3d &, const double &tol= 0.0) const;
    virtual bool In(const Ray3d &, const double &tol= 0.0) const;
    virtual bool In(const Segment3d &, const double &tol= 0.0) const;
    bool intersects(const Line3d &) const;
    bool intersects(const Ray3d &) const;
    bool intersects(const Segment3d &) const;
    GEOM_FT distSigno(const Pos3d &) const;
    GEOM_FT distSigno2(const Pos3d &p) const;
    virtual GEOM_FT dist(const Pos3d &p) const;
    virtual GEOM_FT dist2(const Pos3d &p) const;

    GEOM_FT getAngle(const HalfSpace3d &) const;
    GEOM_FT getAngle(const Plane &) const;
    GEOM_FT getAngle(const Vector3d &) const;

    Line3d getLima(const HalfSpace3d &) const;

    virtual unsigned short int Dimension(void) const;
    virtual GEOM_FT getLength(void) const;
    virtual GEOM_FT getArea(void) const;
    virtual GEOM_FT getVolume(void) const;
    virtual Pos3d getCenterOfMass(void) const;
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Pxy(void) const;
    virtual GEOM_FT Iz(void) const;

    Ray3d clip(const Line3d &) const;
    Ray3d clip(const Ray3d &) const;
    Segment3d clip(const Segment3d &) const;
    
    void Print(std::ostream &os) const;
  };

inline GEOM_FT dist(const Pos3d &p,const HalfSpace3d &r)
  { return r.dist(p); }
inline GEOM_FT dist(const HalfSpace3d &r,const Pos3d &p)
  { return dist(p,r); }
inline bool operator!=(const HalfSpace3d &r1,const HalfSpace3d &r2)
  { return !(r1==r2); }

TripletMap<Pos3d> intersection_points(const std::deque<HalfSpace3d> &);
TripletMap<Pos3d> polyhedron_vertices(const std::deque<HalfSpace3d> &, const double &tol= 1e-10);
std::deque<Line3d> intersection_lines(const std::deque<HalfSpace3d> &);
std::deque<Vector3d> vectores_normales(const std::deque<HalfSpace3d> &);

GEOM_FT angle(const HalfSpace3d &,const HalfSpace3d &);
GEOM_FT angle(const Vector3d &,const HalfSpace3d &);
GEOM_FT angle(const HalfSpace3d &,const Vector3d &);

#endif
