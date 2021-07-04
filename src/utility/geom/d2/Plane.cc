//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Plane.cc

#include "Plane.h"
#include "../lists/ThreePoints3d.h"
#include "GeneralEquationOfPlane.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d1/Ray3d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d1/Polyline3d.h"
#include "utility/geom/d2/Polygon3d.h"
#include "utility/geom/d2/Triangle3d.h"
#include "utility/geom/d3/GeomGroup3d.h"
#include "utility/utils/misc_utils/mchne_eps.h"

const double quiet_nan= std::numeric_limits<double>::quiet_NaN();

Plane::Plane(void)
  : Surface3d(), cgp(CGPoint_3(0,0,0), CGPoint_3(1,0,0), CGPoint_3(0,1,0)) {}

Plane::Plane(const CGPlane_3 &cgp)
  : Surface3d(), cgp(cgp) {}

//! @brief Constructor: plane defined by three points.
Plane::Plane(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : Surface3d(), cgp(p1.ToCGAL(),p2.ToCGAL(),p3.ToCGAL()) 
  {
    if(cgp.is_degenerate())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; degenerate plane "
		<< " p1= " << p1 << " p2= " << p2 << " p3= " << p3
	        << std::endl;
  }

//! @brief Constructor: plane defined by the point and the normal vector.
Plane::Plane(const Pos3d &o,const Vector3d &v)
  : Surface3d(), cgp(o.ToCGAL(),v.ToCGAL()) {}
//! @brief Constructor
Plane::Plane(const Line3d &r,const Pos3d &p)
  : Surface3d(), cgp(r.ToCGAL(),p.ToCGAL()) {}

//! @brief Constructor
Plane::Plane(const Segment3d &s,const Vector3d &v)
  : Surface3d(), cgp(s.getSupportLine().ToCGAL(),(s.getFromPoint()+v).ToCGAL()) {}

//! @brief Constructor
Plane::Plane(const Pos3d &p,const Vector3d &v1,const Vector3d &v2)
  : Surface3d(), cgp(p.ToCGAL(),(p+v1).ToCGAL(),(p+v2).ToCGAL()) {}

//! @brief Constructor
Plane::Plane(const GeomObj3d::list_Pos3d &lp)
  : Surface3d(), cgp()
  {
    Plane tmp(lp.begin(),lp.end());
    (*this)= tmp;
  }

//! @brief Constructor
Plane::Plane(const Polygon3d &pg3d)
  : Surface3d(), cgp()
  { *this= pg3d.getPlane(); }

//! @brief Constructor
Plane::Plane(const GeneralEquationOfPlane &eg)
  : Surface3d(), cgp(eg.a(),eg.b(),eg.c(),eg.d()) {}

GeomObj *Plane::getCopy(void) const
  { return new Plane(*this); }

//! @brief Comparison operator.
bool Plane::operator==(const Plane &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Surface3d::operator==(other);
        if(retval)
          retval= (cgp==other.cgp); 
       }
    return retval;
  }

//! @brief Converts the point classification to the polygon one.
Plane::polygon_classification Plane::clfpnt2clfpol(const CGAL::Oriented_side os)
  {
    polygon_classification retval= BEHIND;
    switch(os)
      {
        case CGAL::ON_NEGATIVE_SIDE:
          retval= BEHIND;
          break;
        case CGAL::ON_POSITIVE_SIDE:
          retval= AHEAD;
          break;
        case CGAL::ON_ORIENTED_BOUNDARY:
          retval= INSIDE;
          break;
      }
    return retval;
  }

void Plane::ThreePoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  { operator=(Plane(p1,p2,p3)); }

//! @brief Return el normal vector oriented to the "positive" side.
Vector3d Plane::Normal(void) const
  {
    Vector3d retval(cgp.orthogonal_vector());
    retval.Normalize();
    return retval;    
  }
//! @brief Return un vector ortogonal al devuelto por Normal().
Vector3d Plane::Base1(void) const
  {
    Vector3d retval(cgp.base1());
    retval.Normalize();
    return retval;
  }
//! @brief Return un vector ortogonal al devuelto por Normal() y al devuelto por Base1().
Vector3d Plane::Base2(void) const
  {
    Vector3d retval(cgp.base2());
    retval.Normalize();
    return retval;
  }

//! @brief Return a coordinate system whose XY plane corresponds to
//! this one.
Rect2d3dCooSys Plane::getCooSys(void) const
  { return Rect2d3dCooSys(Base1(),Base2()); }

//! @brief Return a reference frame whose XY plane corresponds to
//! this one.
Ref2d3d Plane::getRef(void) const
  { return Ref2d3d(Point(),Base1(),Base2()); }

void Plane::swap(void)
  { operator=(getSwap()); }

Plane Plane::getSwap(void) const
  { return Plane(cgp.opposite()); }

//! @brief Return the projection of the point onto this plane.
Pos3d Plane::Projection(const Pos3d &p) const
  { return Pos3d(cgp.projection(p.ToCGAL())); }

//! @brief Return the projection of the vector onto this plane.
Vector3d Plane::Projection(const Vector3d &v) const
  {
    const Vector3d n= Normal().getNormalized();
    return cross(cross(n,v),n);
  }

//! @brief Return the projection of the line onto this plane.
Line3d Plane::Projection(const Line3d &r) const
  {
    Line3d retval;
    const Pos3d p0= Projection(r.Point(0));
    const Pos3d p1= Projection(r.Point(100));
    const double d= p0.dist(p1);
    if(d>mchne_eps_dbl)
      retval= Line3d(p0,p1);
    else
      retval.setExists(false);
    return retval;
  }

GeomObj3d::list_Pos3d Plane::Projection(const GeomObj3d::list_Pos3d &ptos) const
  {
    GeomObj3d::list_Pos3d retval;
    for(GeomObj3d::list_Pos3d::const_iterator i=ptos.begin();i!=ptos.end();i++)
      retval.push_back(Projection(*i));
    return retval;
  }
// Polygono Plane::Projection(const Polygono &pg) const
//   { return pg.Projection(*this); }

//! @brief Return an arbitrary point on the plane.
Pos3d Plane::Point(void) const
  { return Pos3d(cgp.point()); }

bool Plane::positiveSide(const Pos3d &p) const
  { return cgp.has_on_positive_side(p.ToCGAL()); }
bool Plane::negativeSide(const Pos3d &p) const
  { return cgp.has_on_negative_side(p.ToCGAL()); }

//! @brief Return true if the point is in the plane.
bool Plane::In(const Pos3d &p, const GEOM_FT &tol) const
  { 
    bool retval= false;
    if(cgp.has_on(p.ToCGAL()))
      retval= true;
    else if(dist(p)<=tol)
      retval= true;
    return retval;
  }

CGAL::Oriented_side Plane::ClassifyPoint(const Pos3d &p) const
  { return cgp.oriented_side(p.ToCGAL()); }

GEOM_FT Plane::dist(const Pos3d &p) const
  { return sqrt_FT(dist2(p)); }

//! @brief Return the signed distance from the point.
GEOM_FT Plane::PseudoDist(const Pos3d &p) const
  {
    GEOM_FT retval= dist(p);
    if(negativeSide(p))
      retval= -retval;
    return retval;
  }
//! @brief Return the signed squared distance from the point.
GEOM_FT Plane::PseudoDist2(const Pos3d &p) const
  {
    GEOM_FT retval= dist2(p);
    if(negativeSide(p))
      retval= -retval;
    return retval;
  }

//! @brief Return the squared distance from the point.
GEOM_FT Plane::dist2(const Pos3d &p) const
  { return p.dist2(Projection(p)); }

//! @brief Returns the plane equation in general form: ax + by + cz + d = 0
GeneralEquationOfPlane Plane::getGeneralEquation(void) const
  { return GeneralEquationOfPlane(cgp.a(),cgp.b(),cgp.c(),cgp.d()); }

//! @brief Return the position of the centroid.
//! Any point in the plane can be its centroid.
Pos3d Plane::getCenterOfMass(void) const
  { return Point(); }

Plane::polygon_classification Plane::classifyPolygon(const Polygon3d &pol) const
  {
    GeomObj::list_Pos3d lv= pol.getVertexList();
    return ClassifyPoints(lv.begin(),lv.end());
  }

Plane FromCGAL(const CGPlane_3 &p)
  { return Plane(p); }

void Plane::Print(std::ostream &os) const
  {
    os << "point: " << Point()
       << " normal: " << Normal()
       << std::endl;
  }

//! @brief Return the x coordinate of the point of the plane
//! such:
//! y = p.x()
//! z = p.y()
GEOM_FT Plane::x(const Pos2d &p) const
  { return getGeneralEquation().x(p); }

//! @brief Return the y coordinate of the point of the plane
//! such:
//! x = p.x()
//! z = p.y()
GEOM_FT Plane::y(const Pos2d &p) const
  { return getGeneralEquation().y(p); }

//! @brief Return the z coordinate of the point of the plane
//! such:
//! x = p.x()
//! y = p.y()
GEOM_FT Plane::z(const Pos2d &p) const
  { return getGeneralEquation().z(p); }

//! @brief Creates a Plane object from the its equation
//! in general form: general ax + by + cz + d = 0
void Plane::GeneralEquation(const GeneralEquationOfPlane &eq)
  { operator=(Plane(eq)); }

//! @brief Returns true if intersection exists.
bool Plane::intersects(const Plane &p) const
  { return do_intersect(ToCGAL(),p.ToCGAL()); }

//! @brief Returns (if exists) the intersection with the plane argument.
Line3d Plane::getIntersection(const Plane &p) const
  { return intersection_line(*this,p); }

//! @brief Returns true if intersection exists.
bool Plane::intersects(const Polygon3d &plg) const
  {
    bool retval= false;
    Plane plgPlane= plg.getPlane();
    if(intersects(plgPlane))
      {
	Line3d intLine3d= getIntersection(plgPlane);
	retval= plg.intersects(intLine3d);
      }
    return retval;
  }

//! @brief Returns (if exists) the intersection with the plane argument.
Segment3d Plane::getIntersection(const Polygon3d &plg) const
  {
    Segment3d retval;
    Plane plgPlane= plg.getPlane();
    if(intersects(plgPlane))
      {
	Line3d intLine3d= getIntersection(plgPlane);
	retval= plg.getIntersection(intLine3d);
      }
    return retval;
  }

// //! @brief Return (if exists) teh intersection with the plane argument.
// Line3d Plane::getIntersection(const Plane &p) const
//   {
//     const Line3d retval= getIntersection(p);
//     if(!retval.exists())
//       std::cerr << getClassName() << "::" << __FUNCTION__
//                 << "; intersection with plane doesn't exists: "
//                 << p << std::endl;
//     return retval;
//   }

//! @brief Return true if the plane intersects the line argument.
bool Plane::intersects(const Line3d &r) const
  { return do_intersect(ToCGAL(),r.ToCGAL()); }

//! @brief Return (if exists) the intersection with the line argument.
Pos3d Plane::getIntersection(const Line3d &r) const
  {
    Pos3d retval(quiet_nan, quiet_nan,quiet_nan);
    GeomObj3d::list_Pos3d tmp= intersection(*this,r);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

// //! @brief Return (if exists) the intersection with the line argument.
// Pos3d Plane::getIntersection(const Line3d &r) const
//   {
//     const Pos3d retval= getIntersection(r);
//     if(!retval.exists())
//       std::cerr << getClassName() << "::" << __FUNCTION__
//                 << "; intersection with line "
//                 << r << "doesn't exists." << std::endl;
//     return retval;
//   }

//! @brief Return verdadero if exists the intersection with the line argument.
bool Plane::intersects(const Ray3d &sr) const
  { return do_intersect(ToCGAL(),sr.ToCGAL()); }

//! @brief Return (if exists) the intersection with the ray argument.
Pos3d Plane::getIntersection(const Ray3d &sr) const
  {
    Pos3d retval(quiet_nan, quiet_nan,quiet_nan);
    GeomObj3d::list_Pos3d tmp= intersection(*this,sr);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Return true if the intersection with the
//! segment argument exists.
bool Plane::intersects(const Segment3d &sg) const
  { return do_intersect(ToCGAL(),sg.ToCGAL()); }

//! @brief Return (if exists) the intersection with the segment argument.
Pos3d Plane::getIntersection(const Segment3d &sg) const
  {
    Pos3d retval(quiet_nan, quiet_nan, quiet_nan);
    GeomObj3d::list_Pos3d tmp= intersection(*this,sg);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

// //! @brief Return (if exists) the intersection with the line argument
// Pos3d Plane::getIntersection(const Ray3d &sr) const
//   {
//     const Pos3d retval= getIntersection(sr);
//     if(!retval.exists())
//       std::cerr << getClassName() << "::" << __FUNCTION__
//                 << "; intersection with ray doesn't exists: "
//                 << sr << std::endl;
//     return retval;
//   }

//! @brief Return the trace on the XY plane.
Line3d Plane::XYTrace(void) const
  { return getIntersection(XYPlane3d); }

//! @brief Return the trace on the XZ plane.
Line3d Plane::XZTrace(void) const
  { return getIntersection(XZPlane3d); }

//! @brief Return the trace on the YZ plane.
Line3d Plane::YZTrace(void) const
  { return getIntersection(YZPlane3d); }

//! @brief Return the maximum slope line with respect to the XY plane.
Line3d Plane::getMaximumSlopeLineXY(void) const
  {
    Line3d retval;
    Line3d trace= XYTrace();
    if(trace.exists())
      {
        Pos3d point(Point());
        Plane p(perpendicular(trace,point));
        retval= intersection_line(p,*this);
      }
    return retval;
  }

//! @brief Return the maximum slope line with respect to the XZ plane.
Line3d Plane::getMaximumSlopeLineXZ(void) const
  {
    Line3d retval;
    Line3d trace= XZTrace();
    if(trace.exists())
      {
        Pos3d point(Point());
        Plane p(perpendicular(trace,point));
        retval= intersection_line(p,*this);
      }
    return retval;
  }

//! @brief Return the maximum slope line with respect to the YZ plane.
Line3d Plane::getMaximumSlopeLineYZ(void) const
  {
    Line3d retval;
    Line3d trace= YZTrace();
    if(trace.exists())
      {
        Pos3d point(Point());
        Plane p(perpendicular(trace,point));
        retval= intersection_line(p,*this);
      }
    return retval;
  }

//! @brief Return the angle between this plane and the argument.
GEOM_FT Plane::getAngle(const Plane &p) const
  { return angle(*this,p); }

//! @brief Return the angle between this plane and the argument.
GEOM_FT Plane::getAngle(const Vector3d &v) const
  { return angle(*this,v); }

//! @brief Returns the slope angle with respect to the XY plane
GEOM_FT Plane::getSlopeAngleXY(void) const
  { return angle(*this,XYPlane3d); }

//! @brief Returns the slope angle with respect to the XZ plane
GEOM_FT Plane::getSlopeAngleXZ(void) const
  { return angle(*this,XZPlane3d); }

//! @brief Returns the slope angle with respect to the YZ plane
GEOM_FT Plane::getSlopeAngleYZ(void) const
  { return angle(*this,YZPlane3d); }

//! @brief Compute the plane that best suits the point cloud.
GEOM_FT Plane::linearLeastSquaresFitting(const GeomObj3d::list_Pos3d &lp)
  { return linearLeastSquaresFitting(lp.begin(), lp.end()); }

//! @brief Return the plane normal to r that passes through p.
Plane perpendicular(const Line3d &r, const Pos3d &p)
  { return r.Perpendicular(p); }

//! @brief Return the intersection of both planes.
Line3d intersection_line(const Plane &p1, const Plane &p2)
  {
    Line3d retval;
    GeomGroup3d tmp= intersection(p1,p2);
    if(!tmp.empty())
      {
        const Line3d *line_ptr= dynamic_cast<const Line3d *>(tmp.begin()->get_const_ptr());
        if(line_ptr)
          retval= *line_ptr;
      }
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Returnt the intersection of the three planes.
Pos3d intersection_point(const Plane &p1, const Plane &p2, const Plane &p3)
  {
    Pos3d retval;
    GeomGroup3d tmp= intersection(p1,p2);
    if(!tmp.empty())
      {
        const Line3d *line_ptr= dynamic_cast<const Line3d *>(tmp.begin()->get_const_ptr());
        if(line_ptr)
          retval= intersection_point(p3,*line_ptr);
      }
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Intersection of two planes.
GeomGroup3d intersection(const Plane &p1, const Plane &p2)
  {
    GeomGroup3d retval;
    const auto p1_cg= p1.ToCGAL();
    const auto p2_cg= p2.ToCGAL();
    if(do_intersect(p1_cg,p2_cg))
      {
        const CGAL::Object result= CGAL::intersection(p1_cg,p2_cg);
        if(const CGLine_3 *ri= CGAL::object_cast<CGLine_3 >(&result))
	  {
	    const Line3d r3d(*ri);
            retval.push_back(r3d);
	  }
        else
          {
            CGPlane_3 i_plane;
            if(CGAL::assign(i_plane, result))
              {
                std::cerr << __FUNCTION__
		          << "; the planes are the same." 
                          << std::endl;
              }
            else
              {
                std::cerr << __FUNCTION__
			  << "; unknown error." 
                          << std::endl;
              }
	  }
      }
    return retval;
  }

//! @brief Intersection of the a plane with an straight line.
GeomObj3d::list_Pos3d intersection(const Plane &p, const Line3d &r)
  {
    GeomObj3d::list_Pos3d retval;
    if(do_intersect(p.ToCGAL(),r.ToCGAL()))
      {
        CGAL::Object result;
        CGPoint_3 ptoi;
        CGLine_3 ri;
        result = CGAL::intersection(p.ToCGAL(),r.ToCGAL());
        if(CGAL::assign(ptoi, result))
          retval.push_back(Pos3d(ptoi));
        else
          if(CGAL::assign(ri, result))
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Line3d): the plane contains the line." 
                        << std::endl;
            }
          else
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Line3d): unknown error." 
                        << std::endl;
            }
      }
    return retval;
  }

//! @brief Return the intersection of the plane with the ray.
GeomObj3d::list_Pos3d intersection(const Plane &p, const Ray3d &sr)
  {
    GeomObj3d::list_Pos3d retval;
    if(do_intersect(p.ToCGAL(),sr.ToCGAL()))
      {
        CGAL::Object result;
        CGPoint_3 ptoi;
        CGLine_3 ri;
        result = CGAL::intersection(p.ToCGAL(),sr.ToCGAL());
        if(CGAL::assign(ptoi, result))
          retval.push_back(Pos3d(ptoi));
        else
          if(CGAL::assign(ri, result))
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Ray3d): the plane contains the line." 
                   << std::endl;
            }
          else
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Ray3d): unknown error." 
                        << std::endl;
            }
      }
    // else
    //   std::cerr << __FUNCTION__ << "Doesn't itersect." << std::endl;
    return retval;    
  }

//! @brief Return the intersection of the ray with the plane.
GeomObj3d::list_Pos3d intersection(const Ray3d &sr, const Plane &p)
  { return intersection(p,sr); }

//! @brief Return the intersection of the plane with the segment.
GeomObj3d::list_Pos3d intersection(const Plane &p, const Segment3d &sg)
  {
    GeomObj3d::list_Pos3d retval;
    if(do_intersect(p.ToCGAL(),sg.ToCGAL()))
      {
        CGAL::Object result;
        CGPoint_3 ptoi;
        CGLine_3 ri;
        result = CGAL::intersection(p.ToCGAL(),sg.ToCGAL());
        if(CGAL::assign(ptoi, result))
          retval.push_back(Pos3d(ptoi));
        else
          if(CGAL::assign(ri, result))
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Segment3d): the plane contains"
		        << " the segment." << std::endl;
            }
          else
            {
              std::cerr << __FUNCTION__
			<< "(Plane,Segment3d): unknown error." 
                        << std::endl;
            }
      }
    // else
    //   std::cerr << __FUNCTION__ << "; doesn't intersect." << std::endl;
    return retval;    
  }

//! @brief Return the intersection of the segment with the plane.
GeomObj3d::list_Pos3d intersection(const Segment3d &sg, const Plane &p)
  { return intersection(p,sg); }

//! @brief Return the intersection(s) of the plane with the polyline.
GeomObj3d::list_Pos3d intersection(const Plane &p, const Polyline3d &pl)
  { return pl.getIntersection(p); }

//! @brief Return the intersection(s) of the polyline with the plane.
GeomObj3d::list_Pos3d intersection(const Polyline3d &pl, const Plane &p)
  { return pl.getIntersection(p); }

Pos3d intersection_point(const Plane &p, const Line3d &r)
  {
    Pos3d retval;
    GeomObj3d::list_Pos3d tmp= intersection(p,r);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

Pos3d intersection_point(const Line3d &r, const Plane &p)
  { return intersection_point(p,r); }

Pos3d intersection_point(const Plane &p, const Ray3d &sr)
  {
    Pos3d retval;
    GeomObj3d::list_Pos3d tmp= intersection(p,sr);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

Pos3d intersection_point(const Ray3d &sr, const Plane &p)
  { return intersection_point(p,sr); }

Pos3d intersection_point(const Plane &p, const Segment3d &sg)
  {
    Pos3d retval;
    GeomObj3d::list_Pos3d tmp= intersection(p,sg);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }
Pos3d intersection_point(const Segment3d &sg, const Plane &p)
  { return intersection_point(p,sg); }

GEOM_FT angle(const Line3d &r,const Plane &p)
  {
    GEOM_FT retval;
    const Line3d rProj= p.Projection(r);
    if(rProj.exists())
      retval= angle(r,rProj);
    else
      {
        retval= NAN;
        std::cerr << __FUNCTION__
		  << "; error when computing projection." << std::endl;
      }
    return retval;
  }

GEOM_FT angle(const Plane &p,const Line3d &r)
  { return angle(r,p); }

GEOM_FT angle(const Vector3d &v,const Plane &p)
  { return M_PI/2 - angle(p.Normal(),v); }

GEOM_FT angle(const Plane &p,const Vector3d &v)
  { return angle(v,p); }

GEOM_FT angle(const Plane &p1,const Plane &p2)
  {
    GEOM_FT retval= angle(p1.Normal(),p2.Normal());
    return retval;
  }


GeomObj3d::list_Pos3d intersection(const Line3d &r, const Plane &p)
  { return intersection(p,r); }

//! @brief Return the points of intersection between the planes.
GeomObj3d::list_Pos3d intersection_points(const std::deque<Plane> &planes)
  {
    GeomObj3d::list_Pos3d retval;
    const size_t sz= planes.size();
    for(size_t i=0;i<sz;i++)
      for(size_t j=i+1;j<sz;j++)
        for(size_t k=j+1;k<sz;k++)
	  {
            const Pos3d p= intersection_point(planes[i],planes[j],planes[k]);
            if(p.exists())
              retval.push_back(p);
          }
    return retval;
  }

//! @brief Return true if the line is parallel to the plane.
bool parallel(const Plane &p, const Line3d &r)
  { return(!do_intersect(p.ToCGAL(),r.ToCGAL())); }

//! @brief Return true if the planes are parallel.
bool parallel(const Plane &p1, const Plane &p2)
  { return(!do_intersect(p1.ToCGAL(),p2.ToCGAL())); }

