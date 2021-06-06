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
//Ray2d.h

#ifndef RAY2D_H
#define RAY2D_H

#include "Linear2d.h"
#include "Line2d.h"
#include "../pos_vec/Pos2d.h"

class Dir2d;

//! @ingroup GEOM
//
//! @brief Ray in a two-dimensional space.
class Ray2d : public Linear2d
  {
    CGRay_2 cgsr;

    friend class Segment2d;
  public:
    Ray2d(void): Linear2d(),cgsr(CGPoint_2(0,0),CGPoint_2(1,0)) {}
    explicit Ray2d(const CGRay_2 &r)
      : Linear2d(), cgsr(r) {}
    Ray2d(const Pos2d &p1,const Pos2d &p2);
    Ray2d(const Pos2d &p1,const Vector2d &vdir);

    virtual bool operator==(const Ray2d &) const;
    const CGRay_2 &ToCGAL(void) const
      { return cgsr; }
    virtual GeomObj *clon(void) const
      { return new Ray2d(*this); }
    void swap(void);
    void TwoPoints(const Pos2d &p1,const Pos2d &p2);
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int) const
      { return NAN; }
    Line2d getSupportLine(void) const
      { return Line2d(cgsr.supporting_line()); }
    Pos2d getFromPoint(void) const
      { return Pos2d(cgsr.source()); }
    inline GEOM_FT getSlope(void) const
      { return getSupportLine().getSlope(); }
    const Pos2d Point(const int &i) const
      { return Pos2d(cgsr.point(i)); }
    //@brief Return a point of the line at a distance lambda from its origin.
    Pos2d PtoParametricas(const GEOM_FT &lambda) const
      { return Point(0)+lambda*VDir(); }
    inline bool EsVertical(void) const
      { return cgsr.is_vertical(); }
    inline bool isDegenerated(void) const
      { return cgsr.is_degenerate(); }
    //@brief Return true if the points is on the ray.
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const
      { return cgsr.has_on(p.ToCGAL()); }
    GEOM_FT dist2(const Pos2d &p) const;
    GEOM_FT dist(const Pos2d &p) const;
    //! @brief Set the points that define the ray.
    void Put(const Pos2d &p1,const Pos2d &p2)
      { TwoPoints(p1,p2); }

    //! @brief Returns the points ordered by the value of the 
    //! parameter of its projection onto the line from lowest
    //! to highest. Origin is at Line2d::Point(0).
    GeomObj::list_Pos2d Ordena(const GeomObj::list_Pos2d &ptos) const
      { return getSupportLine().Ordena(ptos); }

    bool intersects(const Ray2d &sr2) const
      { return do_intersect(cgsr,sr2.cgsr); }
    bool intersects(const Line2d &sr) const;
    GeomObj2d::list_Pos2d getIntersection(unsigned short int, const double &) const;
    GeomObj2d::list_Pos2d getIntersection(const Line2d &r) const;
    GeomObj2d::list_Pos2d getIntersection(const Ray2d &sr) const;
    
    Pos2d Projection(const Pos2d &) const;
    Vector2d Projection(const Vector2d &) const;

    bool isParallel(const Line2d &r) const
      { return parallel(getSupportLine(),r); }
    bool isParallel(const Ray2d &sr) const
      { return isParallel(sr.getSupportLine()); }

    //! @brief Return the length of the object.
    inline virtual GEOM_FT getLength(void) const
      { return NAN; }
    inline virtual Pos2d getCenterOfMass(void) const
      { return Pos2d(NAN,NAN); }
    Dir2d GetDir(void) const;
    Vector2d VDir(void) const;
    Vector2d Normal(void) const;
    virtual Vector2d getIVector(void) const;
    virtual Vector2d getJVector(void) const;
    inline GEOM_FT getAngle(const Vector2d &v) const
      { return angle(VDir(),v); }
    inline friend GEOM_FT angle(const Ray2d &r,const Vector2d &v)
      { return r.getAngle(v); }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Ix(void) const
      { return 0.0; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //product of inertia.
    inline virtual GEOM_FT Pxy(void) const
      { return NAN; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }

    void Transform(const Trf2d &trf2d);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);

    void Print(std::ostream &os) const;
    void Plot(Plotter &psos) const;
  };

inline GEOM_FT dist2(const Pos2d &p,const Ray2d &r)
  { return r.dist2(p); }
inline GEOM_FT dist2(const Ray2d &r,const Pos2d &p)
  { return dist2(p,r); }
inline GEOM_FT dist(const Pos2d &p,const Ray2d &r)
  { return r.dist(p); }
inline GEOM_FT dist(const Ray2d &r,const Pos2d &p)
  { return dist(p,r); }

inline bool parallel(const Ray2d &sr,const Line2d &r)
  { return sr.isParallel(r); }
inline bool parallel(const Line2d &r,const Ray2d &sr)
  { return parallel(sr,r); }
inline bool parallel(const Ray2d &r1,const Ray2d &r2)
  { return r1.isParallel(r2); }

inline bool intersecan(const Ray2d &sr1,const Ray2d &sr2)
  { return sr1.intersects(sr2); }
inline bool intersecan(const Ray2d &sr,const Line2d &r)
  { return sr.intersects(r); }
inline bool intersecan(const Line2d &r,const Ray2d &sr)
  { return sr.intersects(r); }
inline GeomObj2d::list_Pos2d intersection(const Ray2d &sr,const Line2d &r)
  { return sr.getIntersection(r); }
inline GeomObj2d::list_Pos2d intersection(const Line2d &r, const Ray2d &sr)
  { return sr.getIntersection(r); }

//! @brief Return the intersection point of both lines, if it does not exists
//! return an empty list.
inline GeomObj2d::list_Pos2d intersection(const Ray2d &sr1,const Ray2d &sr2)
  { return sr1.getIntersection(sr2); }


inline bool colineales(const Ray2d &sr,const Line2d &r)
  { return colineales(sr.getSupportLine(),r); }
inline bool colineales(const Line2d &r,const Ray2d &sr)
  { return colineales(sr,r); }
inline bool colineales(const Ray2d &sr1,const Ray2d &sr2)
  { return colineales(sr1,sr2.getSupportLine()); }


#endif
