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
//Line3d.h

#ifndef LINE3D_H
#define LINE3D_H

#include <cstdlib>
#include <cmath>
#include "utility/utils/misc_utils/matem.h"
#include "Linear3d.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"



//! @ingroup GEOM
//
//! @brief Line in a three-dimensional space (parametric form).
class Line3dParametricForm
  {
    Pos3d org; //!< Line origin.
    Vector3d dir; //!< Direction vector.
  public:
    Line3dParametricForm(const Pos3d &o,const Vector3d &d)
      : org(o), dir(d) {}
    inline const Pos3d &GetOrg(void) const
      { return org; }
    inline const Vector3d &GetDir(void) const
      { return dir; }
    //! @brief Return a point at a distance lambda from its origin.
    inline Pos3d getPoint(const GEOM_FT &lambda) const
      { return org+lambda*dir; }
  };

class Line2d;
class Plane;
class Dir3d;


//! @ingroup GEOM
//
//! @brief Line in a three-dimensional space.
class Line3d : public Linear3d
  {
    CGLine_3 cgr;
    static const Pos3d defaultOrg;
    static const Pos3d defaultDest;
  public:
    Line3d(void);
    explicit Line3d(const CGLine_3 &r);
    Line3d(const Pos3d &p1,const Pos3d &p2);
    Line3d(const Pos3d &p,const Dir3d &dir);
    Line3d(const Plane &p1,const Plane &p2);
    explicit Line3d(const Line3dParametricForm &param);
    virtual GeomObj *clon(void) const
      { return new Line3d(*this); }
    void swap(void);
    void TwoPoints(const Pos3d &p1,const Pos3d &p2);
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int i) const
      { return NAN; }
    const CGLine_3 &ToCGAL(void) const
      { return cgr; }
    inline Pos3d Point(const int &i=0) const
      { return Pos3d(cgr.point(i)); }

    double getLambda(unsigned short int i,const double &d,const Vector3d &i_) const;

    Pos3d Projection(const Pos3d &) const;
    Vector3d Projection(const Vector3d &) const;
    Line3d XY3DProjection(void) const;
    Line3d XZ3DProjection(void) const;
    Line3d YZ3DProjection(void) const;

    Line2d XY2DProjection(void) const;
    Line2d XZ2DProjection(void) const;
    Line2d YZ2DProjection(void) const;

    GEOM_FT getSlope(void) const;
    //! @brief Returns the parametric equations of the line as:
    //!   v[0]: point in the line.
    //!   v[1]: dir vector for the line.
    Line3dParametricForm GetParametricas(void) const
      { return Line3dParametricForm(Point(0),VDir()); }
    //! @brief Return a point at a distance lambda from its origin.
    Pos3d PtoParametricas(const GEOM_FT &lambda) const
      { return Point(0)+lambda*VDir(); }
    //! @brief Defines the line from its parametric equation.
    void Parametricas(const Line3dParametricForm &param)
      { TwoPoints(param.getPoint(0.0),param.getPoint(100.0)); }
    //! @brief Return true if the point is on the line.
    virtual bool In(const Pos3d &p, const double &tol= 0.0) const
      { return cgr.has_on(p.ToCGAL()); }
    //! @brief Return the squared distance from the point to the line.
    virtual GEOM_FT dist2(const Pos3d &p) const
      { return p.dist2(Projection(p)); }
    virtual GEOM_FT dist(const Pos3d &p) const;

    Plane Perpendicular(const Pos3d &p) const;
    //! @brief Set the points that define the line.
    void Put(const Pos3d &p1,const Pos3d &p2)
      { TwoPoints(p1,p2); }
    bool isParallel(const Line3d &r) const;

    bool intersects(const Line3d &r2) const;
    GeomObj3d::list_Pos3d getIntersection(unsigned short int, const double &) const;
    GeomObj3d::list_Pos3d getIntersection(const Line3d &r2, const double &tol= sqrt_mchne_eps_dbl) const;

    //! @brief Return the length of the object.
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
    inline friend double angle(const Line3d &r,const Vector3d &v)
      { return r.getAngle(v); }
    inline friend double angle(const Line3d &r1,const Line3d &r2)
      { return r1.getAngle(r2.VDir()); }

    GEOM_FT linearLeastSquaresFitting(const GeomObj3d::list_Pos3d &lp);

    //! @brief Moment of inertia with respect to the local axis x
    //! passing through the center of mass.
    inline virtual GEOM_FT Ix(void) const
      { return 0.0; }
    //! @brief Moment of inertia with respect to the local axis y
    //! passing through the center of mass.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //! @brief Moment of inertia with respect to the local axis z
    //! passing through the center of mass.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    void Print(std::ostream &os) const;
  };

const Line3d XAxis3d(Pos3d(0,0,0),Pos3d(1,0,0));
const Line3d YAxis3d(Pos3d(0,0,0),Pos3d(0,1,0));
const Line3d ZAxis3d(Pos3d(0,0,0),Pos3d(0,0,1));


inline GEOM_FT dist2(const Pos3d &p,const Line3d &r)
  { return r.dist2(p); }
inline GEOM_FT dist2(const Line3d &r,const Pos3d &p)
  { return dist2(p,r); }
GEOM_FT dist(const Pos3d &p,const Line3d &r);
inline GEOM_FT dist(const Line3d &r,const Pos3d &p)
  { return dist(p,r); }

inline bool parallel(const Line3d &r1,const Line3d &r2)
  { return r1.isParallel(r2); }
bool colineales(const Line3d &r1,const Line3d &r2);
bool coplanarias(const Line3d &r1,const Line3d &r2);

inline bool intersecan(const Line3d &r1,const Line3d &r2)
  { return r1.intersects(r2); }
inline GeomObj3d::list_Pos3d intersection(const Line3d &r1,const Line3d &r2)
  { return r1.getIntersection(r2); }


#endif
