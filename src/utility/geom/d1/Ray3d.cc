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
//Ray3d.cc

#include "Ray3d.h"

#include "../pos_vec/Dir3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Pos3d.h"

//! @brief Default constructor.
Ray3d::Ray3d(void)
  : Linear3d(),cgsr(CGPoint_3(0,0,0),CGPoint_3(1,0,0)) {}

//! @brief Constructor from CGAL object.
Ray3d::Ray3d(const CGRay_3 &r)
  : Linear3d(), cgsr(r) {}

//! @brief Constructor.
//! @param p1: ray origing.
//! @param p2: point in the ray direction.
Ray3d::Ray3d(const Pos3d &p1,const Pos3d &p2)
  : Linear3d(), cgsr(p1.ToCGAL(),p2.ToCGAL())
  {
    if(isDegenerated())
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; degenerate ray, the points: "
             << p1 << " and " << p2 << " are the same." << std::endl;
      }
  }

//! @brief Constructor.
//! @param p: ray origing.
//! @param v: ray direction.
Ray3d::Ray3d(const Pos3d &p,const Vector3d &v)
  : Linear3d(), cgsr(p.ToCGAL(),v.ToCGAL())
  {
    if(isDegenerated())
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; degenerate ray." << std::endl;
      }
  }

//! @brief Comparison operator.
bool Ray3d::operator==(const Ray3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Linear3d::operator==(other);
        if(retval)
          retval= (cgsr==other.cgsr); 
       }
    return retval;
  }

//! @ brief Swaps the ray orientation.
void Ray3d::swap(void)
  { cgsr= cgsr.opposite(); }

void Ray3d::TwoPoints(const Pos3d &p1,const Pos3d &p2)
  { (*this)= Ray3d(p1,p2); }

Dir3d Ray3d::GetDir(void) const
  { return Dir3d(cgsr.direction()); }

Vector3d Ray3d::VDir(void) const
  { return GetDir().GetVector(); }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d Ray3d::getIVector(void) const
  { return getSupportLine().getIVector(); }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d Ray3d::getJVector(void) const
  { return getSupportLine().getJVector(); }

//! @brief Returns a vector in the direction of the local
//! Z axis
Vector3d Ray3d::getKVector(void) const
  { return getSupportLine().getKVector(); }

//! @brief Returns the squared distand from the point.
GEOM_FT Ray3d::dist2(const Pos3d &p) const
  {
    const Line3d r= getSupportLine();
    const Pos3d proj= r.Projection(p);
    GEOM_FT retval= p.dist2(proj); //OK if projection inside half-line.
    if(!In(proj)) //Projection outside half-line.
      retval= p.dist2(getFromPoint());
    return retval;
  }

//! @brief Return the distance 
//! from the point to the ray.
GEOM_FT Ray3d::dist(const Pos3d &p) const
  { return sqrt_FT(dist2(p)); }

//! @brief Return true if parallel.
bool Ray3d::isParallel(const Ray3d &r) const
  { return (GetDir()== r.GetDir()); }

//! @brief Return true if parallel.
bool Ray3d::isParallel(const Line3d &r) const
  { return (GetDir()== r.GetDir()); }

//! @brief Return the intersection point with the argument line
//! if it doesn't exist return an empty list.
GeomObj3d::list_Pos3d Ray3d::getIntersection(const Line3d &r) const
  {
    const Line3d sop= getSupportLine();
    GeomObj3d::list_Pos3d retval= sop.getIntersection(r);
    if(!retval.empty()) //Intersection exists.
      {
        const Pos3d &pint= *retval.begin();
        if(!In(pint)) //intersection point is NOT on the ray.
          retval.erase(retval.begin(),retval.end());
      }
    return retval;
  }

//! @brief Return the intersection of the ray with the plane
//! defined by the equation coord_i= d.
GeomObj3d::list_Pos3d Ray3d::getIntersection(unsigned short int i, const double &d) const
  {
    GeomObj3d::list_Pos3d lp;
    lp= getSupportLine().getIntersection(i,d);
    if(!lp.empty())
      {
        const Vector3d i_= VDir();
        const double l= getSupportLine().getLambda(i,d,i_);
        if(l<0.0)
          lp.erase(lp.begin(),lp.end());
      }
    return lp;
  }

//! @brief Return the intersection point of both rays, if it doesn't exists
//! return an empty list.
GeomObj3d::list_Pos3d Ray3d::getIntersection(const Ray3d &sr) const
  {
    const Line3d sop1= getSupportLine();
    GeomObj3d::list_Pos3d retval= sr.getIntersection(sop1);
    if(!retval.empty()) //Intersection exists.
      {
        const Pos3d &pint= *retval.begin();
        if(!In(pint)) //intersection point is NOT on the ray.
          retval.erase(retval.begin(),retval.end());
      }
    return retval;
  }

//! @brief Return the orthogonal projection onto the line.
//! @param p: point to project.
Pos3d Ray3d::Projection(const Pos3d &p) const
  {
    const Line3d l= getSupportLine();
    return l.Projection(p);
  }

//! @brief Return the projection onto the line.
//! @param v: vector to project.
Vector3d Ray3d::Projection(const Vector3d &v) const
  {
    const Line3d l= getSupportLine();
    return l.Projection(v);
  }

