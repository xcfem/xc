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
//Ray3d.h

#ifndef RAY3D_H
#define RAY3D_H

#include "Linear3d.h"
#include "Line3d.h"
#include "../pos_vec/Pos3d.h"

class Dir3d;


//! @ingroup GEOM
//
//! @brief Ray in a three-dimensional space.
class Ray3d : public Linear3d
  {
    CGRay_3 cgsr;
  public:
    Ray3d(void);
    explicit Ray3d(const CGRay_3 &);
    Ray3d(const Pos3d &,const Pos3d &);
    Ray3d(const Pos3d &,const Vector3d &);
    virtual GeomObj *clon(void) const
      { return new Ray3d(*this); }
    void swap(void);
    virtual bool operator==(const Ray3d &) const;
    const CGRay_3 &ToCGAL(void) const
      { return cgsr; }
    void TwoPoints(const Pos3d &p1,const Pos3d &p2);
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int) const
      { return NAN; }
    Line3d getSupportLine(void) const
      { return Line3d(cgsr.supporting_line()); }
    Pos3d getFromPoint(void) const
      { return Pos3d(cgsr.source()); }
    inline GEOM_FT getSlope(void) const
      { return getSupportLine().getSlope(); }
    const Pos3d Point(const int &i= 0) const
      { return Pos3d(cgsr.point(i)); }
    //! @brief Return a point of the line at a distance lambda from its origin.
    Pos3d PtoParametricas(const GEOM_FT &lambda) const
      { return Point(0)+lambda*VDir(); }
    inline bool isDegenerated(void) const
      { return cgsr.is_degenerate(); }
    virtual bool In(const Pos3d &p, const double &tol= 0.0) const
    //Return true if the point is on the object.
      { return cgsr.has_on(p.ToCGAL()); }
    virtual GEOM_FT dist2(const Pos3d &p) const;
    GEOM_FT dist(const Pos3d &p) const;
    //! @brief Set the points that define the line.
    void Put(const Pos3d &p1,const Pos3d &p2)
      { TwoPoints(p1,p2); }

    bool isParallel(const Ray3d &r) const;
    bool isParallel(const Line3d &r) const;

    GeomObj3d::list_Pos3d getIntersection(unsigned short int, const double &) const;
    GeomObj3d::list_Pos3d getIntersection(const Line3d &r) const;
    GeomObj3d::list_Pos3d getIntersection(const Ray3d &sr) const;

    Pos3d Projection(const Pos3d &) const;
    Vector3d Projection(const Vector3d &) const;
    
    inline virtual GEOM_FT getLength(void) const
      { return NAN; }
    inline virtual Pos3d getCenterOfMass(void) const
      { return Pos3d(NAN,NAN,NAN); }
    Dir3d GetDir(void) const;
    Vector3d VDir(void) const;
    virtual Vector3d getIVector(void) const;
    virtual Vector3d getJVector(void) const;
    virtual Vector3d getKVector(void) const;
    inline double getAngle(const Vector3d &v) const
      { return angle(VDir(),v); }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Ix(void) const
      { return 0.0; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    inline void Print(std::ostream &os) const
      { os << PtoParametricas(0.0) << " " << PtoParametricas(100.0); }
  };

inline GEOM_FT angle(const Ray3d &r,const Vector3d &v)
  { return r.getAngle(v); }
inline GEOM_FT angle(const Ray3d &sr,const Line3d &r)
  { return r.getAngle(sr.VDir()); }
inline GEOM_FT angle(const Line3d &r,const Ray3d &sr)
  { return r.getAngle(sr.VDir()); }

inline GEOM_FT dist2(const Pos3d &p,const Ray3d &r)
  { return r.dist2(p); }
inline GEOM_FT dist2(const Ray3d &r,const Pos3d &p)
  { return r.dist2(p); }
inline GEOM_FT dist(const Pos3d &p,const Ray3d &r)
  { return r.dist(p); }
inline GEOM_FT dist(const Ray3d &r,const Pos3d &p)
  { return r.dist(p); }

inline bool parallel(const Ray3d &r1,const Line3d &r2)
  { return r1.isParallel(r2); }
inline bool parallel(const Line3d &r1,const Ray3d &r2)
  { return parallel(r2,r1); }
inline bool parallel(const Ray3d &r1,const Ray3d &r2)
  { return r1.isParallel(r2); }

inline GeomObj3d::list_Pos3d intersection(const Line3d &r1,const Ray3d &r2)
  { return r2.getIntersection(r1); }
inline GeomObj3d::list_Pos3d intersection(const Ray3d &r1,const Line3d &r2)
  { return r1.getIntersection(r2); }
inline GeomObj3d::list_Pos3d intersection(const Ray3d &r1,const Ray3d &r2)
  { return r1.getIntersection(r2); }


inline bool colineales(const Ray3d &sr,const Line3d &r)
  { return colineales(sr.getSupportLine(),r); }
inline bool colineales(const Line3d &r,const Ray3d &sr)
  { return colineales(sr,r); }
inline bool colineales(const Ray3d &sr1,const Ray3d &sr2)
  { return colineales(sr1,sr2.getSupportLine()); }



#endif
