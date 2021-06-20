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
//HalfPlane2d.h

#ifndef HALFPLANE2D_H
#define HALFPLANE2D_H


#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/Surface2d.h"

class GeomGroup2d;
class Ray2d;
class Segment2d;


//! @ingroup GEOM
//
//! @brief Half plane in a two-dimensional space.
class HalfPlane2d : public Surface2d
  {
    Line2d lim; //The points of the half-plane are those we lie to the right of the line.
  public:
    HalfPlane2d(void);
    explicit HalfPlane2d(const Line2d &r);
    HalfPlane2d(const Line2d &r, const Pos2d &p);
    HalfPlane2d(const Pos2d &p1,const Pos2d &p2);

    virtual bool operator==(const HalfPlane2d &) const;
    virtual GeomObj *clon(void) const;
    inline void swap(void);
    HalfPlane2d getSwap(void) const;

    //! @brief Return the value of a that corresponds to the
    //! equation of the line: a*x + b*y + c= 0
    inline GEOM_RT a(void) const
      { return lim.a(); }
    //! @brief Return the value of b that corresponds to the
    //! equation of the line: a*x + b*y + c= 0
    inline GEOM_RT b(void) const
      { return lim.b(); }
    //! @brief Return the value of c that corresponds to the
    //! equation of the line: a*x + b*y + c= 0
    inline GEOM_RT c(void) const
      { return lim.c(); } 
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int) const
      { return NAN; }
    inline bool isDegenerated(void) const
      { return lim.isDegenerated(); }
    inline const Line2d &getBorde(void) const
      { return lim; }
    
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;
    virtual bool In(const Line2d &, const double &tol= 0.0) const;
    virtual bool In(const Ray2d &, const double &tol= 0.0) const;
    virtual bool In(const Segment2d &, const double &tol= 0.0) const;

    //! @brief Return the signed distance from the point to the half-plane.
    GEOM_FT DistSigno(const Pos2d &p) const
      {
        GEOM_FT retval= lim.dist(p);
        if(In(p)) retval= -retval;
        return retval;
      }
    //! @brief Return the distance from the point to the half-plane.
    inline virtual GEOM_FT Dist(const Pos2d &p) const
      { return fabs(DistSigno(p)); }

    HalfPlane2d getNormalized(void) const;

    inline bool intersects(const HalfPlane2d &r2) const
      { return lim.intersects(r2.lim); }
    inline bool intersects(const Line2d &r) const
      { return lim.intersects(r); }
    bool intersects(const Ray2d &sr) const;
    bool intersects(const Segment2d &sg) const;
    GeomGroup2d getIntersection(const Line2d &r) const;
    GeomGroup2d getIntersection(const Ray2d &sr) const;
    GeomGroup2d getIntersection(const Segment2d &sg) const;

    //! @brief Return the length of the object.
    inline virtual GEOM_FT getLength(void) const
      { return NAN; }
    inline virtual Pos2d getCenterOfMass(void) const
      { return lim.getCenterOfMass(); }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Ix(void) const
      { return NAN; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //product of inertia.
    inline virtual GEOM_FT Pxy(void) const
      { return NAN; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }
    
    Ray2d clip(const Line2d &) const;
    Ray2d clip(const Ray2d &) const;
    Segment2d clip(const Segment2d &) const;

    void Transform(const Trf2d &trf2d);

    inline void Print(std::ostream &os) const
      { os << lim; }
  };

inline GEOM_FT dist(const Pos2d &p,const HalfPlane2d &r)
  { return r.Dist(p); }
inline GEOM_FT dist(const HalfPlane2d &r,const Pos2d &p)
  { return dist(p,r); }
inline bool operator!=(const HalfPlane2d &r1,const HalfPlane2d &r2)
  { return !(r1==r2); }

inline bool intersecan(const HalfPlane2d &sp1,const HalfPlane2d &sp2)
  { return sp1.intersects(sp2); }
inline bool intersecan(const HalfPlane2d &sp,const Line2d &r)
  { return sp.intersects(r); }
inline bool intersecan(const HalfPlane2d &sp,const Ray2d &sr)
  { return sp.intersects(sr); }
inline bool intersecan(const HalfPlane2d &sp,const Segment2d &sg)
  { return sp.intersects(sg); }
inline bool intersecan(const Line2d &r,const HalfPlane2d &sp)
  { return sp.intersects(r); }
inline bool intersecan(const Ray2d &sr,const HalfPlane2d &sp)
  { return sp.intersects(sr); }
inline bool intersecan(const Segment2d &sg,const HalfPlane2d &sp)
  { return sp.intersects(sg); }

GeomGroup2d intersection(const HalfPlane2d &sp,const Line2d &r);
GeomGroup2d intersection(const HalfPlane2d &sp,const Ray2d &sr);
GeomGroup2d intersection(const HalfPlane2d &sp,const Segment2d &sg);
GeomGroup2d intersection(const Line2d &r,const HalfPlane2d &sp);
GeomGroup2d intersection(const Ray2d &sr,const HalfPlane2d &sp);
GeomGroup2d intersection(const Segment2d &sg,const HalfPlane2d &sp);

#endif
