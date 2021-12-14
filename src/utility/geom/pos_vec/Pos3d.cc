//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//Pos3d.cc

#include "Pos3d.h"
#include "Pos2d.h"
#include "../d2/Plane.h"
#include "Vector3d.h"



#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d1/Ray3d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/geom/d3/Solid3d.h"

//! @brief return true if one of the coordinate components is not a number.
bool Pos3d::notAPoint(void) const
  { return (std::isnan(x()) || std::isnan(y()) || std::isnan(z()) ); }


//! @brief Adds a vector.
Pos3d &Pos3d::operator+=(const Vector3d &v)
  {
    operator=((*this)+v);
    return *this;
  }

//! @brief Substracts a vector.
Pos3d &Pos3d::operator-=(const Vector3d &v)
  {
    operator=((*this)-v);
    return *this;
  }

//! @brief Vector between the points.
Vector3d Pos3d::operator-(const Pos3d &p) const
  {
    Vector3d retval(ToCGAL()-p.ToCGAL());
    return retval;
  }

//! @brief Point obtained by subtraction of the vector.
Pos3d Pos3d::operator-(const Vector3d &v) const
  {
    Pos3d retval(ToCGAL()-v.ToCGAL());
    return retval;
  }

//! @brief Point obtained by addition of the vector.
Pos3d Pos3d::operator+(const Vector3d &v) const
  {
    Pos3d retval(ToCGAL()+v.ToCGAL());
    return retval;
  }

//! @brief Comparison operator.
bool Pos3d::operator==(const Pos3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= ProtoGeom::operator==(other);
        if(retval)
          retval= (cgpt==other.cgpt); 
       }
    return retval;
  }

//! @brief Return the position vector of the point.
Vector3d Pos3d::VectorPos(void) const
  { return (*this)-Origin3d; }

GEOM_FT Pos3d::at0(const size_t &i) const
  { return cgpt.cartesian(i); }

GEOM_FT Pos3d::operator()(const size_t &i) const
  { return cgpt.cartesian(i-1); }

void Pos3d::SetX(const GEOM_FT &vx)
  { cgpt= CGPoint_3(vx,y(),z()); }
void Pos3d::SetY(const GEOM_FT &vy)
  { cgpt= CGPoint_3(x(),vy,z()); }
void Pos3d::SetZ(const GEOM_FT &vz)
  { cgpt= CGPoint_3(x(),y(),vz); }
void Pos3d::Set(unsigned short int i,const GEOM_FT &v)
  {
    switch(i)
      {
      case 1:
        SetX(v);
        break;
      case 2:
        SetY(v);
        break;
      case 3:
        SetZ(v);
        break;
      default:
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; index " << i << " out of range: [1,3]." << std::endl;
        break;
      }
  }

//! @brief Return the quadrant of the point.
int Pos3d::Cuadrante(void) const
  {
    const bool x_no_neg= (x()>=0);
    const bool y_no_neg= (y()>=0);
    const bool z_no_neg= (z()>=0);
    if(x_no_neg) //(1,2,3 ó 4)
      if(y_no_neg) //1 o 4
        if(z_no_neg)
          return 1;
        else
          return 4;
      else //2 o 3
        if(z_no_neg)
          return 2;
        else
          return 3;
    else //(5,6,7 u 8)
      if(y_no_neg) //5 u 8
        if(z_no_neg)
          return 5;
        else
          return 8;
      else //6 o 7
        if(z_no_neg)
          return 6;
        else
          return 7;
  }

//! @brief Return the projection onto XY plane.
Pos3d Pos3d::XY3DProjection(void) const
  { return XYPlane3d.Projection(*this); }
//! @brief Return the projection onto XZ plane.
Pos3d Pos3d::XZ3DProjection(void) const
  { return XZPlane3d.Projection(*this); }
//! @brief Return the projection onto YZ plane.
Pos3d Pos3d::YZ3DProjection(void) const
  { return YZPlane3d.Projection(*this); }

//! @brief Return the projection onto XY plane.
Pos2d Pos3d::XY2DProjection(void) const
  {
    Pos3d p3d= XY3DProjection();
    return Pos2d(p3d.x(),p3d.y());
  }
//! @brief Return the projection onto XZ plane.
Pos2d Pos3d::XZ2DProjection(void) const
  {
    Pos3d p3d= XZ3DProjection();
    return Pos2d(p3d.x(),p3d.z());
  }

//! @brief Return the projection onto YZ plane.
Pos2d Pos3d::YZ2DProjection(void) const
  {
    Pos3d p3d= YZ3DProjection();
    return Pos2d(p3d.y(),p3d.z());
  }

//! @brief Equivalent to the @mod<arg command from Autocad @mod<arg
//! Rotation with respect to the x axis.
Pos3d Pos3d::getModArgX(const GEOM_FT &mod,const GEOM_FT &giroX) const
  { return Pos3d(x(),y()+mod*cos(giroX),z()+mod*sin(giroX)); }

//! @brief Equivalent to the @mod<arg command from Autocad @mod<arg
//! Rotation with respect to the y axis.
Pos3d Pos3d::getModArgY(const GEOM_FT &mod,const GEOM_FT &giroY) const
  { return Pos3d(x()+mod*cos(giroY),y(),z()-mod*sin(giroY)); }


//! @brief Equivalent to the @mod<arg command from Autocad @mod<arg
//! Rotation with respect to the z axis.
Pos3d Pos3d::getModArgZ(const GEOM_FT &mod,const GEOM_FT &giroZ) const
  { return Pos3d(x()+mod*cos(giroZ),y()+mod*sin(giroZ),z()); }

bool Pos3d::domina_a(const Pos3d &b) const
  {
    for(size_t i= 1;i<=dimension();i++)
      if ((*this)(i)<b(i)) return false;
    return true;
  }

//! @brief Return the distance to the point.
GEOM_FT Pos3d::dist(const Pos3d &p) const
  { return sqrt_FT(dist2(p)); }

//! @brief Return the squared distance to the point.
GEOM_FT Pos3d::dist2(const Pos3d &p) const
  { return squared_distance(cgpt,p.cgpt); }

//! @brief Return the squared distance to the line.
GEOM_FT Pos3d::dist2(const Line3d &r) const
  { return r.dist2(*this); }
  
//! @brief Return the distance to the line.
GEOM_FT Pos3d::dist(const Line3d &r) const
  { return r.dist(*this); }

//! @brief Return the squared distance to the ray.
GEOM_FT Pos3d::dist2(const Ray3d &sr) const
  { return sr.dist2(*this); }

//! @brief Return the distance to the ray.
GEOM_FT Pos3d::dist(const Ray3d &sr) const
  { return sr.dist(*this); }

//! @brief Return the squared distance to the segment.
GEOM_FT Pos3d::dist2(const Segment3d &sg) const
  { return sg.dist2(*this); }

//! @brief Return the distance to the segment.
GEOM_FT Pos3d::dist(const Segment3d &sg) const
  { return sg.dist(*this); }

//! @brief Return the squared distance to the plane.
GEOM_FT Pos3d::dist2(const Plane &p) const
  { return p.dist2(*this); }

//! @brief Return the distance to the plane.
GEOM_FT Pos3d::dist(const Plane &p) const
  { return p.dist(*this); }

//! @brief Return the squared distance to the half space.
GEOM_FT Pos3d::dist2(const HalfSpace3d &se) const
  { return se.dist2(*this); }

//! @brief Return the distance to the half space.
GEOM_FT Pos3d::dist(const HalfSpace3d &se) const
  { return se.dist(*this); }

//! @brief Return the distance to the solid.
GEOM_FT Pos3d::dist(const Solid3d &s) const
  { return s.dist(*this); }

std::ostream &operator<<(std::ostream &stream,const Pos3d &n)
  {
//     stream << "x= " << to_double(n.x()) << ','
//            << "y= " << to_double(n.y()) << ','
//            << "z= " << to_double(n.z());
    stream << "x= " << n.x() << ','
           << "y= " << n.y() << ','
           << "z= " << n.z();
    return stream;
  }


Pos3d To3dXY2d(const Pos2d &p,const GEOM_FT &z)
  { return Pos3d(p.x(),p.y(),z); }
Pos3d To3dXZ2d(const Pos2d &p,const GEOM_FT &y)
  { return Pos3d(p.x(),y,p.y()); }
Pos3d To3dYZ2d(const Pos2d &p,const GEOM_FT &x)
  { return Pos3d(x,p.x(),p.y()); }
Pos3d To3d(const Pos2d &p,const size_t &coo,const GEOM_FT &u)
  { 
    const size_t i= (coo-1)%3+1;
    if(i==3) return To3dXY2d(p,u);
    if(i==2) return To3dXZ2d(p,u);
    return To3dYZ2d(p,u);
  }

//! @brief Return true if the points are collinear.
bool colineales(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  { return collinear(p1.cgpt,p2.cgpt,p3.cgpt); }

//! @brief Return true if the points are coplanar.
bool coplanarios(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4)
  { return coplanar(p1.cgpt,p2.cgpt,p3.cgpt,p4.cgpt); } 

//! @brief Return a Python dictionary with the object members values.
boost::python::dict Pos3d::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["x"]= x();
    retval["y"]= y();
    retval["z"]= z();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Pos3d::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    SetX(boost::python::extract<GEOM_FT>(d["x"]));
    SetY(boost::python::extract<GEOM_FT>(d["y"]));
    SetZ(boost::python::extract<GEOM_FT>(d["z"]));
  }



