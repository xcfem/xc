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
//Circle3d.cc

#include "Circle3d.h"
#include "Polygon3d.h"
#include "../cgal_types.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/matem.h"
#include <plotter.h>

#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/pos_vec/Pos3dArray.h"


//! @brief Constructor.
Circle3d::Circle3d(void)
  : D2to3d(), circ() {}

//! @brief Circle defined by three points.
Circle3d Circle3dThreepoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  { return Circle3d(p1,p2,p3); }

//! @brief Constructor.
Circle3d::Circle3d(const Pos3d &centro,const GEOM_FT &rad)
 : D2to3d(), circ()
  { circ= Circle2d(to_2d(centro),rad); }

//! @brief Constructor.
Circle3d::Circle3d(const GEOM_FT &rad2,const Pos3d &centro)
 : D2to3d(), circ()
  { circ= Circle2d(rad2,to_2d(centro)); }

//! @brief Constructor.
Circle3d::Circle3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : D2to3d(p1,p2,p3), circ()
  { circ= Circle2d(to_2d(p1),to_2d(p2),to_2d(p3)); }

//! @brief Constructor.
Circle3d::Circle3d(const Ref2d3d &ref,const Circle2d &c)
  : D2to3d(ref), circ(c) {}

//! @brief Constructor.
Circle3d::Circle3d(const Ref3d3d &ref,const Circle2d &c)
  : D2to3d(ref), circ(c) {}

Pos3d Circle3d::Centro(void) const
  { return to_3d(circ.Centro()); }
Pos3d Circle3d::getCenterOfMass(void) const
  { return Centro(); }
GEOM_FT Circle3d::getSquaredRadius(void) const
  { return circ.getSquaredRadius(); }
GEOM_FT Circle3d::getRadius(void) const
  { return circ.getRadius(); }
GEOM_FT Circle3d::getDiameter(void) const
  { return circ.getDiameter(); }
double Circle3d::getAngle(const Pos3d &p) const
  { return circ.getAngle(to_2d(p)); }
GEOM_FT Circle3d::Ix(void) const
  { return circ.Ix(); }
GEOM_FT Circle3d::Iy(void) const
  { return circ.Iy(); }
GEOM_FT Circle3d::Pxy(void) const
  { return circ.Pxy(); }
GEOM_FT Circle3d::Iz(void) const
  { return circ.Iz(); }

GEOM_FT Circle3d::GetMax(unsigned short int i) const
  {
    cerr << "Circle3d::GetMax(i), not implemented." << endl;
    return 1;
  }
GEOM_FT Circle3d::GetMin(unsigned short int i) const
  {
    cerr << "Circle3d::GetMin(i), not implemented." << endl;
    return -1;
  }

//! @brief Return true if the points is inside the circle.
bool Circle3d::In(const Pos3d &p, const double &tol) const
  {
    if(getPlane().In(p,tol))
      return circ.In(to_2d(p),tol);
    else
      return false;
  }

//! @brief Return n points equally spaced on the object perimeter.
const Pos3dArray &Circle3d::getPointsOnPerimeter(const size_t &n,const double &theta_inic) const
  {
    static Pos3dArray retval= to_3d(circ.getPointsOnPerimeter(n,theta_inic));
    return retval;
  }

//! @brief Return the n-gon inscribed int the circle.
Polygon3d Circle3d::getInscribedPolygon(const size_t &n,const double &theta_inic) const
  {
    Polygon2d plg2d= circ.getInscribedPolygon(n, theta_inic);
    return Polygon3d(getRef(), plg2d);
  }

void Circle3d::Print(std::ostream &os) const
  { os << circ; }

//! @brief Equal operator.
bool operator ==(const Circle3d &a,const Circle3d &b)
  {
    if((const D2to3d &) a ==(const D2to3d &) b)  
      return ( a.circ == b.circ );
    else
      return false;
  }

//! @brief Return the curvature radius of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
GEOM_FT curvature_radius(const Pos3d &A,const Pos3d &B,const Pos3d &C)
  {
    const Vector3d D= cross(B-A,C-A); // cross-product.
    const GEOM_FT b= Abs(A-C); // norm
    const GEOM_FT c= Abs(A-B); // norm
    const GEOM_FT a= Abs(B-C); //slightly faster if only R is required
    GEOM_FT retval= a*b*c/2.0/Abs(D);
    if(Abs(D) == 0.0)
      retval= std::numeric_limits<GEOM_FT>::infinity();
    return retval;
  }

//! @brief Return the curvature of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
GEOM_FT curvature(const Pos3d &A,const Pos3d &B,const Pos3d &C)
  { return 1.0/curvature_radius(A,B,C); }

//! @brief Return the curvature vector of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
Vector3d curvature_vector(const Pos3d &A,const Pos3d &B,const Pos3d &C)
  {
    const Vector3d D= cross(B-A,C-A); // cross-product.
    const GEOM_FT b= Abs(A-C); // norm
    const GEOM_FT c= Abs(A-B); // norm
    const Vector3d E= cross(D,B-A);
    const Vector3d F= cross(D,C-A); 
    const Vector3d G= (b*b*E-c*c*F)/pow(Abs(D),2)/2.0;
    const Pos3d M= A + G;
    GEOM_FT R= Abs(G); // Radius of curvature
    Vector3d retval;
    if(R == 0.0)
      retval = G;
    else if(Abs(D) == 0.0) // aligned points.
      {
        R= std::numeric_limits<GEOM_FT>::infinity();
        retval= D;
      }
     else
       retval = G/pow(R,2); // Curvature vector
    return retval;
  }

//! @brief Return the curvature radius of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
GEOM_FT curvature_radius(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3)
  {
    const Pos3d A= To3dXY2d(p1,0.0);
    const Pos3d B= To3dXY2d(p2,0.0);
    const Pos3d C= To3dXY2d(p3,0.0);
    return curvature_radius(A,B,C);
  }

//! @brief Return the curvature of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
GEOM_FT curvature(const Pos2d &A,const Pos2d &B,const Pos2d &C)
  { return 1.0/curvature_radius(A,B,C); }

//! @brief Return the curvature vector of the circumscribed circle
//! for the triangle ABC (based on https://es.mathworks.com/matlabcentral/fileexchange/69452-curvature-of-a-1d-curve-in-a-2d-or-3d-space).
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
Vector2d curvature_vector(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3)
  {
    const Pos3d A= To3dXY2d(p1,0.0);
    const Pos3d B= To3dXY2d(p2,0.0);
    const Pos3d C= To3dXY2d(p3,0.0);
    const Vector3d tmp= curvature_vector(A,B,C);
    return Vector2d(tmp.x(), tmp.y());
  }

//! @brief Return the center of the circumscribed circle
//! for the triangle ABC. Inspired on: https://github.com/sergarrido/random/blob/master/circle3d/circle3d.cpp
//
//! @param A: first point.
//! @param B: second point.
//! @param C: third point.
Pos3d circle_center(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  {
    const Vector3d v1= p2-p1;
    const Vector3d v2= p3-p1;

    const GEOM_FT v1v1= v1.GetDot(v1);
    const GEOM_FT v2v2= v2.GetDot(v2);
    const GEOM_FT v1v2= v1.GetDot(v2);

    const GEOM_FT base= 0.5/(v1v1*v2v2-v1v2*v1v2);
    const GEOM_FT k1= base*v2v2*(v1v1-v1v2);
    const GEOM_FT k2= base*v1v1*(v2v2-v1v2);
    const Pos3d retval= p1 + v1*k1 + v2*k2; // center
    return retval;
  }
