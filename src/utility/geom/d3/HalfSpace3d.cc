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
//HalfSpace3d.cc

#include "HalfSpace3d.h"



#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d1/Line3d.h"


//! @brief Default constructor.
HalfSpace3d::HalfSpace3d(const Plane &p)
  : GeomObj3d(), lim(p) {}

//! @brief Comparison operator.
bool HalfSpace3d::operator==(const HalfSpace3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= GeomObj3d::operator==(other);
        if(retval)
          retval= (lim==other.lim);
       }
    return retval;
  }

//! @brief Return the plane that defined de half space boudary.
const Plane &HalfSpace3d::getBoundaryPlane(void) const
  { return lim; }

void HalfSpace3d::setBoundaryPlane(const Plane &p)
  { lim= p; }

//! @brief Return true if the point is inside the half-plane.
//! The points of the edge line belong to the half-plane.
bool HalfSpace3d::In(const Pos3d &p, const double &tol) const
  {
    bool retval= false;
    if(lim.negativeSide(p))
      retval= true;
    else if(lim.In(p,tol))
      retval= true;
    return retval;
  }

//! @brief Return the signed distance from the point to the half plane.
GEOM_FT HalfSpace3d::distSigno(const Pos3d &p) const
  { return lim.PseudoDist(p); }

//! @brief Return the signed squared distance from the point to the half plane.
GEOM_FT HalfSpace3d::distSigno2(const Pos3d &p) const
  { return lim.PseudoDist2(p); }

//! @brief Return the distance from the point to the half plane.
GEOM_FT HalfSpace3d::dist(const Pos3d &p) const
  { return fabs(distSigno(p)); }

//! @brief Return the squared distance from the point to the half plane.
GEOM_FT HalfSpace3d::dist2(const Pos3d &p) const
  { return fabs(distSigno2(p)); }

//! @brief Return el the angle with the half space argument.
GEOM_FT HalfSpace3d::getAngle(const HalfSpace3d &se) const
  { return angle(lim,se.getPlane()); }

//! @brief Return the angle with respect to the plane argument.
GEOM_FT HalfSpace3d::getAngle(const Plane &p) const
  { return angle(lim,p); }

//! @brief Return the angle with the vector argument.
GEOM_FT HalfSpace3d::getAngle(const Vector3d &v) const
  { return angle(v,lim); }

//! @brief Return la arista de the intersection entre half spaces.
Line3d HalfSpace3d::getLima(const HalfSpace3d &se) const
  {
     const Line3d retval= lim.getIntersection(se.lim);
     if(!retval.exists())
       std::cerr << getClassName() << "::" << __FUNCTION__
	         << "; the half space boundaries doesn't intersect."
                 << std::endl;
     return retval;
  }

//!brief Return the dimension of the object 0, 1, 2 or 3.
unsigned short int HalfSpace3d::Dimension(void) const
  { return 3; }

//! @brief Return the lenght of the object.
GEOM_FT HalfSpace3d::getLength(void) const
  { return NAN; }
//! @brief Return the object area.
GEOM_FT HalfSpace3d::getArea(void) const
  { return NAN; }
//! @brief Return the object volume
GEOM_FT HalfSpace3d::getVolume(void) const
  { return NAN; }
Pos3d HalfSpace3d::getCenterOfMass(void) const
  { return lim.getCenterOfMass(); }

//! @brief Return el moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT HalfSpace3d::Ix(void) const
  { return NAN; }

//! @brief Return el moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT HalfSpace3d::Iy(void) const
  { return NAN; }

//! @brief Return el product of inertia.
GEOM_FT HalfSpace3d::Pxy(void) const
  { return NAN; }

//! @brief Return el moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT HalfSpace3d::Iz(void) const
  { return NAN; }

//! @brief Return a vector normal to the boundary plane
//! oriented inside-out.
Vector3d HalfSpace3d::NormalExterior(void) const
  { return lim.Normal(); }

//! @brief Return a vector normal to the boundary plane
//! oriented outside-in.
Vector3d HalfSpace3d::NormalInterior(void) const
  { return -NormalExterior(); }

//! @brief Imprime el half space
void HalfSpace3d::Print(std::ostream &os) const
      { os << lim; }

//! @brief Return de points of intersection between half spaces.
TripletMap<Pos3d> intersection_points(const std::deque<HalfSpace3d> &se)
  {
    TripletMap<Pos3d> retval;
    const size_t sz= se.size();
    for(size_t i=0;i<sz;i++)
      for(size_t j=i+1;j<sz;j++)
        for(size_t k=j+1;k<sz;k++)
	  {
            const Pos3d p= intersection_point(se[i].getPlane(),se[j].getPlane(),se[k].getPlane());
            if(p.exists())
              retval[Triplete(i,j,k)]= p;
          }
    return retval;
  }

//! @brief Return the vertices of the polyhedron that results from
//! the half spaces intersection.
TripletMap<Pos3d> polyhedron_vertices(const std::deque<HalfSpace3d> &se, const double &tol)
  {
    TripletMap<Pos3d> tmp= intersection_points(se);
    TripletMap<Pos3d> retval= tmp;
    for(std::deque<HalfSpace3d>::const_iterator j=se.begin();j!=se.end();j++)
      for(TripletMap<Pos3d>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        if((*j).Out((*i).second,tol))
          {
            retval.erase(i->first);
            continue;
          }
    if(retval.size()<4)
      std::cerr << __FUNCTION__
		<< "; less than four vertices were obtained."
                << std::endl;
    for(TripletMap<Pos3d>::const_iterator i= retval.begin();i!=retval.end();i++)
      {
        for(TripletMap<Pos3d>::const_iterator j= retval.begin();j!=retval.end();j++)
          if(j!=i)
            {
              const GEOM_FT d2= dist2(i->second,j->second);
              if(d2<1e-4)
                {
		  std::cerr << "Los vértices: ";
                  retval.PrintMember(std::cerr,i);
		  std::cerr << " y ";
                  retval.PrintMember(std::cerr,j);
		  std::cerr << " están muy próximos. d= "
                            << sqrt(d2) << std::endl;
                }
            }
      }
    return retval;
  }

//! @brief Return the intersection lines between half spaces.
std::deque<Line3d> intersection_lines(const std::deque<HalfSpace3d> &se)
  {
    std::deque<Line3d> retval;
    const size_t sz= se.size();
    for(size_t i=0;i<sz;i++)
      for(size_t j=i+1;j<sz;j++)
        {
          const Line3d r= intersection_line(se[i].getPlane(),se[j].getPlane());
          if(r.exists())
            retval.push_back(r);
        }
    return retval;
  }

//! @brief Return los vectores normales a cada uno de los half spaces.
std::deque<Vector3d> vectores_normales(const std::deque<HalfSpace3d> &se)
  {
    std::deque<Vector3d> retval;
    const size_t sz= se.size();
    for(size_t i=0;i<sz;i++)
      retval.push_back(se[i].NormalExterior());
    return retval;
  }

//! @brief Return the angle between half spaces.
GEOM_FT angle(const HalfSpace3d &se1,const HalfSpace3d &se2)
  { return se1.getAngle(se2); }

//! @brief Return the angle between the half space and the vector.
GEOM_FT angle(const Vector3d &v,const HalfSpace3d &se)
  { return se.getAngle(v); }

//! @brief Return the angle between the half space and the vector.
GEOM_FT angle(const HalfSpace3d &se,const Vector3d &v)
  { return se.getAngle(v); }
