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
//Segment2d.h

#ifndef SEGMENT2D_H
#define SEGMENT2D_H

#include "Linear2d.h"
#include "Line2d.h"
#include "Ray2d.h"
#include "../pos_vec/Pos2d.h"

class Dir2d;
class VectorPos2d;

//! @ingroup GEOM
//
//! @brief Segment in a two-dimensional space.
class Segment2d : public Linear2d
  {
    CGSegment_2 cgseg;
  public:
    Segment2d(void): Linear2d(),cgseg(CGPoint_2(0,0),CGPoint_2(1,0)) {}
    explicit Segment2d(const CGSegment_2 &r)
      : Linear2d(), cgseg(r) {}
    Segment2d(const Pos2d &p1,const Pos2d &p2);
    
    virtual bool operator==(const Segment2d &) const;
    const CGSegment_2 &ToCGAL(void) const
      { return cgseg; }
    virtual GeomObj *clon(void) const;
    void swap(void);
    void TwoPoints(const Pos2d &p1,const Pos2d &p2);
    virtual GEOM_FT GetMax(unsigned short int i) const
      { return std::max(getFromPoint()(i),getToPoint()(i)); }
    virtual GEOM_FT GetMin(unsigned short int i) const
      { return std::min(getFromPoint()(i),getToPoint()(i)); }
    Line2d getSupportLine(void) const
      { return Line2d(cgseg.supporting_line()); }
    Pos2d getFromPoint(void) const
      { return Pos2d(cgseg.source()); }
    Pos2d getToPoint(void) const
      { return Pos2d(cgseg.target()); }
    inline GEOM_FT getSlope(void) const
      { return getSupportLine().getSlope(); }
    const Pos2d Point(const int &i) const
      { return Pos2d(cgseg.point(i)); }
    Pos2d PtoParametricas(const GEOM_FT &) const;
    double getParamNaturalCoord(const GEOM_FT &) const;
    Pos2d getPointNaturalCoord(const GEOM_FT &) const;
    inline bool EsVertical(void) const
      { return cgseg.is_vertical(); }
    inline bool isDegenerated(void) const
      { return cgseg.is_degenerate(); }
    virtual bool In(const Pos2d &, const double &tol= 0.0) const;
    virtual GEOM_FT dist2(const Pos2d &p) const;
    GEOM_FT dist(const Pos2d &p) const;
    //! @brief Set the points that define the segment.
    void Put(const Pos2d &p1,const Pos2d &p2)
      { TwoPoints(p1,p2); }

    //! @brief Returns the points ordered by the value of the 
    //! parameter of its projection onto the line from lowest
    //! to highest. Origin is at Line2d::Point(0).
    GeomObj::list_Pos2d Ordena(const GeomObj::list_Pos2d &ptos) const
      { return getSupportLine().Ordena(ptos); }

    Segment2d Offset(const Vector2d &v) const;
    Segment2d Offset(const GEOM_FT &d) const;

    inline bool intersects(const Segment2d &r2) const
      { return do_intersect(cgseg,r2.cgseg); }
    bool intersects(const Line2d &r) const;
    bool intersects(const Ray2d &sr) const;
    GeomObj2d::list_Pos2d getIntersection(unsigned short int, const double &) const;
    GeomObj2d::list_Pos2d getIntersection(const Line2d &r) const;
    GeomObj2d::list_Pos2d getIntersection(const Ray2d &sr) const;
    GeomObj2d::list_Pos2d getIntersection(const Segment2d &sg2) const;
    std::pair<int,int> connected(const Segment2d &, const GEOM_FT &tol) const;


    Pos2d Projection(const Pos2d &) const;
    Vector2d Projection(const Vector2d &) const;

    Line2d Perpendicular(const Pos2d &p) const;
    Line2d isParallel(const Pos2d &v) const;
    Line2d getPerpendicularBisector(void) const;

    bool isParallel(const Line2d &r) const
      { return parallel(getSupportLine(),r); }
    bool isParallel(const Ray2d &sr) const
      { return parallel(getSupportLine(),sr); }
    bool isParallel(const Segment2d &r) const
      { return parallel(getSupportLine(),r.getSupportLine()); }

    virtual GEOM_FT getLength(void) const;
    virtual Pos2d getCenterOfMass(void) const;
    Dir2d GetDir(void) const;
    Vector2d VDir(void) const;
    Vector2d Normal(void) const;
    Vector2d GetVector(void) const;
    virtual Vector2d getIVector(void) const;
    virtual Vector2d getJVector(void) const;
    GEOM_FT getAngle(const Vector2d &v) const;
    GEOM_FT getSignedAngle(const Vector2d &v) const;
    GEOM_FT getAngle(const Segment2d &v) const;
    GEOM_FT getSignedAngle(const Segment2d &v) const;
    GEOM_FT XAxisAngle(void) const;
    GEOM_FT YAxisAngle(void) const;
    inline GEOM_FT Azimuth(void) const
      { return getSupportLine().Azimuth(); }

    //! @brief Moment of inertia with respect to the center of mass en local axis.
    inline virtual GEOM_FT Ix(void) const
      { return 0.0; }
    //! @brief Moment of inertia with respect to the center of mass en local axis.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //! @brief Product of inertia.
    inline virtual GEOM_FT Pxy(void) const
      { return NAN; }
    //! @brief Moment of inertia with respect to the center of mass en local axis.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }
    
    VectorPos2d Divide(int num_partes) const;
    boost::python::list DividePy(int num_partes) const;
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);

    void Transform(const Trf2d &trf2d);

    void Print(std::ostream &os) const;
  };

inline GEOM_FT dist2(const Pos2d &p,const Segment2d &r)
  { return r.dist2(p); }
inline GEOM_FT dist2(const Segment2d &r,const Pos2d &p)
  { return dist2(p,r); }
GEOM_FT dist(const Pos2d &p,const Segment2d &r);
inline GEOM_FT dist(const Segment2d &r,const Pos2d &p)
  { return dist(p,r); }

GEOM_FT angle(const Segment2d &,const Vector2d &);
GEOM_FT angle(const Vector2d &,const Segment2d &);

inline bool parallel(const Segment2d &sg,const Line2d &r)
  { return sg.isParallel(r); }
inline bool parallel(const Line2d &r, const Segment2d &sg)
  { return parallel(sg,r); }
inline bool parallel(const Segment2d &sg,const Ray2d &sr)
  { return sg.isParallel(sr); }
inline bool parallel(const Ray2d &sr, const Segment2d &sg)
  { return parallel(sg,sr); }
inline bool parallel(const Segment2d &r1,const Segment2d &r2)
  { return r1.isParallel(r2); }

inline bool intersecan(const Segment2d &sg,const Line2d &r)
  { return sg.intersects(r); }
inline bool intersecan(const Line2d &r,const Segment2d &sg)
  { return sg.intersects(r); }
inline bool intersecan(const Segment2d &sg,const Ray2d &sr)
  { return sg.intersects(sr); }
inline bool intersecan(const Ray2d &sr,const Segment2d &sg)
  { return sg.intersects(sr); }
inline bool intersecan(const Segment2d &r1,const Segment2d &r2)
  { return r1.intersects(r2); }
inline GeomObj2d::list_Pos2d intersection(const Segment2d &sg,const Line2d &r)
  { return sg.getIntersection(r); }
inline GeomObj2d::list_Pos2d intersection(const Line2d &r,const Segment2d &sg)
  { return sg.getIntersection(r); }
inline GeomObj2d::list_Pos2d intersection(const Segment2d &sg,const Ray2d &sr)
  { return sg.getIntersection(sr); }
inline GeomObj2d::list_Pos2d intersection(const Ray2d &sr,const Segment2d &sg)
  { return sg.getIntersection(sr); }
inline GeomObj2d::list_Pos2d intersection(const Segment2d &sg1,const Segment2d &sg2)
  { return sg1.getIntersection(sg2); }

inline bool colineales(const Segment2d &sg,const Line2d &r)
  { return colineales(sg.getSupportLine(),r); }
inline bool colineales(const Line2d &r,const Segment2d &sg)
  { return colineales(sg,r); }
inline bool colineales(const Segment2d &sg,const Ray2d &sr)
  { return colineales(sg.getSupportLine(),sr); }
inline bool colineales(const Ray2d &sr,const Segment2d &sg)
  { return colineales(sg,sr); }
inline bool colineales(const Segment2d &sg1,const Segment2d &sg2)
  { return colineales(sg1,sg2.getSupportLine()); }

Pos2d intersection_point(const Segment2d &, const Line2d &);
Pos2d intersection_point(const Line2d &, const Segment2d &);
Pos2d intersection_point(const Segment2d &, const Segment2d &);

inline Line2d perpendicular_bisector(const Segment2d &sg)
  { return sg.getPerpendicularBisector(); }

std::list<Segment2d> without_degenerated(const std::list<Segment2d> &);


#endif
