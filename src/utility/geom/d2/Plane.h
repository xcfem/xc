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
//Plane.h

#ifndef PLANO3D_H
#define PLANO3D_H

#include <cstdlib>
#include <cmath>
#include "utility/utils/misc_utils/matem.h"
#include "Surface3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Pos3dList.h"
#include "../cgal_types.h"
#include <list>

class Line3d;
class Ray3d;
class Segment3d;
class Polyline3d;
class Polygon3d;
class GeomGroup3d;
class GeneralEquationOfPlane;
class Rect2d3dCooSys;
class Ref2d3d;


//! @ingroup GEOM
//
//! @brief Plane in a three-dimensional space.
class Plane : public Surface3d
  {
    CGPlane_3 cgp;
  public:

    typedef enum{AHEAD,BEHIND,INSIDE,CROSSES} polygon_classification;

  protected:
    static polygon_classification clfpnt2clfpol(const CGAL::Oriented_side os);
  public:
    Plane(void);
    explicit Plane(const CGPlane_3 &cgp);
    Plane(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    Plane(const Pos3d &o,const Vector3d &v);
    Plane(const Pos3d &p,const Vector3d &v1,const Vector3d &v2);
    Plane(const Line3d &r,const Pos3d &p);
    Plane(const Segment3d &s,const Vector3d &v);
    explicit Plane(const Polygon3d &trg);
    explicit Plane(const GeneralEquationOfPlane &eg);
    explicit Plane(const GeomObj3d::list_Pos3d &lp);
    template <typename InputIterator>
    Plane(InputIterator begin,InputIterator end);
    
    virtual bool operator==(const Plane &) const;

    virtual GeomObj *clon(void) const;
    const CGPlane_3 &ToCGAL(void) const
      { return cgp; }
    void ThreePoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    virtual GEOM_FT GetMax(unsigned short int) const
      { return NAN; }
    virtual GEOM_FT GetMin(unsigned short int i) const
      { return NAN; }
    Pos3d Projection(const Pos3d &) const;
    Vector3d Projection(const Vector3d &) const;
    Line3d Projection(const Line3d &) const;
    GeomObj3d::list_Pos3d Projection(const GeomObj3d::list_Pos3d &) const;
    //Polygon3d Projection(const Polygon3d &) const;

    virtual Vector3d Normal(void) const;
    Vector3d Base1(void) const;
    Vector3d Base2(void) const;
    Rect2d3dCooSys getCooSys(void) const;
    Ref2d3d getRef(void) const;

    void swap(void);
    Plane getSwap(void) const;

    bool positiveSide(const Pos3d &p) const;
    bool negativeSide(const Pos3d &p) const;
    virtual bool In(const Pos3d &p, const GEOM_FT &tol= 0.0) const;

    CGAL::Oriented_side ClassifyPoint(const Pos3d &p) const;

    template <typename InputIterator>
    polygon_classification ClassifyPoints(InputIterator first,InputIterator last) const;
    polygon_classification classifyPolygon(const Polygon3d &pol) const;

    GEOM_FT PseudoDist(const Pos3d &p) const;
    GEOM_FT PseudoDist2(const Pos3d &p) const;
    virtual GEOM_FT dist2(const Pos3d &p) const;
    virtual GEOM_FT dist(const Pos3d &p) const;
    //! @brief Set the points that define the plane.
    void Put(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
      { ThreePoints(p1,p2,p3); }

    GeneralEquationOfPlane getGeneralEquation(void) const;
    void GeneralEquation(const GeneralEquationOfPlane &eq);
    GEOM_FT x(const Pos2d &p) const;
    GEOM_FT y(const Pos2d &p) const;
    GEOM_FT z(const Pos2d &p) const;

    virtual Pos3d getCenterOfMass(void) const;
    Pos3d Point(void) const;
    //! @brief Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Ix(void) const
      { return NAN; }
    //! @brief Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iy(void) const
      { return NAN; }
    //! @brief Moment of inertia with respect to the center of mass in local coordinates.
    inline virtual GEOM_FT Iz(void) const
      { return NAN; }

    Line3d XYTrace(void) const;
    Line3d XZTrace(void) const;
    Line3d YZTrace(void) const;
    Line3d getMaximumSlopeLineXY(void) const;
    Line3d getMaximumSlopeLineXZ(void) const;
    Line3d getMaximumSlopeLineYZ(void) const;

    bool intersects(const Plane &p) const;
    Line3d getIntersection(const Plane &p) const;
    bool intersects(const Line3d &p) const;
    Pos3d getIntersection(const Line3d &p) const;
    bool intersects(const Ray3d &p) const;
    Pos3d getIntersection(const Ray3d &p) const;
    bool intersects(const Segment3d &p) const;
    Pos3d getIntersection(const Segment3d &p) const;
    bool intersects(const Polygon3d &p) const;
    Segment3d getIntersection(const Polygon3d &p) const;

    GEOM_FT getAngle(const Plane &p) const;
    GEOM_FT getAngle(const Vector3d &) const;
    GEOM_FT getSlopeAngleXY(void) const;
    GEOM_FT getSlopeAngleXZ(void) const;
    GEOM_FT getSlopeAngleYZ(void) const;

    GEOM_FT linearLeastSquaresFitting(const GeomObj3d::list_Pos3d &lp);
    template <typename InputIterator>
    GEOM_FT linearLeastSquaresFitting(InputIterator begin,InputIterator end);

    friend Plane FromCGAL(const CGPlane_3 &p);
    void Print(std::ostream &os) const;
  };

Plane FromCGAL(const CGPlane_3 &p);

const Plane XYPlane3d(Pos3d(0,0,0),Pos3d(1,0,0),Pos3d(0,1,0));
const Plane XZPlane3d(Pos3d(0,0,0),Pos3d(1,0,0),Pos3d(0,0,1));
const Plane YZPlane3d(Pos3d(0,0,0),Pos3d(0,1,0),Pos3d(0,0,1));

GEOM_FT angle(const Line3d &r,const Plane &p);
GEOM_FT angle(const Plane &p,const Line3d &r);
GEOM_FT angle(const Vector3d &,const Plane &);
GEOM_FT angle(const Plane &,const Vector3d &);
GEOM_FT angle(const Plane &,const Plane &);


Plane perpendicular(const Line3d &r, const Pos3d &p);

bool parallel(const Plane &p, const Line3d &r);
bool parallel(const Plane &p1, const Plane &p2);


Line3d intersection_line(const Plane &, const Plane &);
Pos3d intersection_point(const Plane &, const Plane &,const Plane &);
GeomGroup3d intersection(const Plane &p1, const Plane &p2);
GeomObj3d::list_Pos3d intersection(const Plane &p, const Line3d &r);
GeomObj3d::list_Pos3d intersection(const Line3d &r, const Plane &p);
GeomObj3d::list_Pos3d intersection(const Plane &p, const Ray3d &r);
GeomObj3d::list_Pos3d intersection(const Ray3d &r, const Plane &p);
GeomObj3d::list_Pos3d intersection(const Plane &p, const Segment3d &);
GeomObj3d::list_Pos3d intersection(const Segment3d &, const Plane &p);
GeomObj3d::list_Pos3d intersection(const Plane &p, const Polyline3d &r);
GeomObj3d::list_Pos3d intersection(const Polyline3d &r, const Plane &p);
Pos3d intersection_point(const Plane &p, const Line3d &r);
Pos3d intersection_point(const Line3d &r, const Plane &p);
Pos3d intersection_point(const Plane &p, const Ray3d &r);
Pos3d intersection_point(const Ray3d &r, const Plane &p);
Pos3d intersection_point(const Plane &p, const Segment3d &r);
Pos3d intersection_point(const Segment3d &r, const Plane &p);

GeomObj3d::list_Pos3d intersection_points(const std::deque<Plane> &);

template <typename InputIterator>
Plane::polygon_classification Plane::ClassifyPoints(InputIterator first,InputIterator last) const
  {
    InputIterator i= first;
    CGAL::Oriented_side cf_pinic= ClassifyPoint(*i); i++;
    for(;i!=last;i++)
      {
        const CGAL::Oriented_side cf_point= ClassifyPoint(*i);
        if(cf_point!=cf_pinic)
          {
            if(cf_pinic!=CGAL::ON_ORIENTED_BOUNDARY) //Start point out of plane.
              if(cf_point!=CGAL::ON_ORIENTED_BOUNDARY) //Point *i out of plane.
                return CROSSES;
              else //Point *i inside the plane.
                continue;
            else //Start point inside the plane.
              if(cf_point!=CGAL::ON_ORIENTED_BOUNDARY) //Point *i out of plane.
                {
                  cf_pinic= cf_point;
                  continue;
                }
              else //Point *i inside the plane.
                continue;
          }
      }
    //Si llegamos hasta aquí es que todos tienen la
    //misma clasificación.
    return clfpnt2clfpol(cf_pinic);
  }

//! @brief Compute the plane that best suits the point cloud.
template <typename InputIterator>
GEOM_FT Plane::linearLeastSquaresFitting(InputIterator begin,InputIterator end)
  {
    Pos3dList pointList(begin,end);
    GEOM_FT quality(0.0);
    cgp= pointList.linearLeastSquaresFittingPlane(quality);
    return quality;
  }
template <typename InputIterator>
Plane::Plane(InputIterator begin,InputIterator end)
  : Surface3d(), cgp()
  {
    const size_t sz= std::distance(begin,end); // number of elements.
    if(sz<3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the list must contain at least three points." 
             << std::endl;
      }
    else if(sz==3)
      {
        const InputIterator i= begin;
	const Pos3d &pi= *i;
	InputIterator j= i;j++;
	const Pos3d &pj= *j;
	InputIterator k= j; k++;
	const Pos3d &pk= *k;
        ThreePoints(pi,pj,pk);
      }
    else
      linearLeastSquaresFitting(begin,end);
  }

/* class Ray; */

/* GeomObj3d::list_Pos3d intersection(const Plane &p, const Ray3d &sr); */
/* inline GeomObj3d::list_Pos3d intersection(const Ray3d &sr, const Plane &p) */
/*   { return intersection(p,sr); }  */


#endif

