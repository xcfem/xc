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
//CircularSector2d.cc

#include "CircularSector2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/coo_sys/ref_sys/Ref2d2d.h"
#include "utility/geom/d1/Line2d.h"
#include <plotter.h>

//! @brief Circular sector defined by three points p1->p2->p3.
CircularSector2d CircularSector2dThreepoints(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3)
  {
    const Circle2d tmp= Circle2d(p1,p2,p3);
    double th1= tmp.getAngle(p1);
    double th3= tmp.getAngle(p3);
    // if(th3<th1)
    //   std::swap(th1, th3);
    CircularSector2d retval(tmp,th1,th3);
    double alpha1= 0.0;
    const double alpha2= retval.getAngle(p2);
    const double alpha3= retval.getAngle(p3);
    // Check if the angles are sorted:
    if(((alpha1>=alpha2) && (alpha3>=alpha2)) || ((alpha1<=alpha2) && (alpha3<=alpha2)))
      {
	// Check again with alpha1= 360 degrees.
	alpha1= 2.0*M_PI;
	if(((alpha1>=alpha2) && (alpha3>=alpha2)) || ((alpha1<=alpha2) && (alpha3<=alpha2)))
          {
	    std::cerr << __FUNCTION__
		      << "; error when defining circular sector: " << std::endl
		      << "  p1= " << p1
		      << "  alpha1= " << RadToDeg(alpha1) <<std::endl
		      << "  p2= " << p2
		      << "  alpha2= " << RadToDeg(alpha2) <<std::endl
		      << "  p3= " << p3
		      << "  alpha3= " << RadToDeg(alpha3) <<std::endl;
	  }
      }
    return retval;
  }

//! @brief Default constructor.
CircularSector2d::CircularSector2d(void)
  : Circle2d(), theta1(0), theta2(M_PI/2) {}

//! @brief Build the circle from its center and its radius.
CircularSector2d::CircularSector2d(const Circle2d &c,const double &th1,const double &th2)
  : Circle2d(c), theta1(th1), theta2(th2) 
  {
    if(theta1>theta2)
      theta1-= 2*M_PI;
  }

//! @brief Build the circle from three points.
CircularSector2d::CircularSector2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3)
  : Circle2d(), theta1(0), theta2(M_PI/2)
  {
    CircularSector2d tmp(CircularSector2dThreepoints(p1, p2, p3));
    (*this)= tmp;
  }

//! @brief Comparison operator.
bool CircularSector2d::operator==(const CircularSector2d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Circle2d::operator==(other);
        if(retval)
          retval= ((theta1==other.theta1) && (theta2==other.theta2));
       }
    return retval;
  }

//! @brief Constructor virtual.
Surface2d *CircularSector2d::getCopy(void) const
  { return new CircularSector2d(*this); }


//! @brief Return the position of the center of mass.
Pos2d CircularSector2d::getCenterOfMass(void) const
  {
    const GEOM_FT theta= getIncludedAngle()/2.0;
    const GEOM_FT lambda= 2.0/3.0*sin(theta)/theta;
    const Vector2d v= lambda*(PMed()-Centro());
    return Centro()+v;
  }

//! @brief Return the position of the arc centroid.
Pos2d CircularSector2d::ArcCentroid(void) const
  {
    const GEOM_FT theta= getIncludedAngle()/2.0;
    const GEOM_FT lambda= sin(theta)/theta;
    const Vector2d v= lambda*(PMed()-Centro());
    return Centro()+v;
  }


//! @brief Return the point inicial del arco.
Pos2d CircularSector2d::PInic(void) const
  { return Point(theta1); }

//! @brief Return the point final del arco.
Pos2d CircularSector2d::PFin(void) const
  { return Point(theta2); }

//! @brief Return the mid-point of the arc.
Pos2d CircularSector2d::PMed(void) const
  { return Point((theta1+theta2)/2); }

//! @brief Return the angle between the line from the center of the circle
//! to the point p and the line from the center of the circle to the sector
//! start point.
double CircularSector2d::getAngle(const Pos2d &p) const
  { return Circle2d::getAngle(p)-theta1; }

//! @brief Returns the parameter of the point in the arc (distance to the arc's first point measured over the arc)
double CircularSector2d::getLambda(const Pos2d &p) const
  { return getAngle(p)*getRadius(); }


//! @brief Return the included angle.
double CircularSector2d::getIncludedAngle(void) const
  { return theta2-theta1; }

//! @brief Return the length of the arc.
GEOM_FT CircularSector2d::getArcLength(void) const
  { return double_to_FT(getIncludedAngle())*getRadius(); }

//! @brief Return the length of the circular sector.
GEOM_FT CircularSector2d::getLength(void) const
  { return getArcLength()+ getDiameter(); }

//! @brief Return the area of the circular sector.
GEOM_FT CircularSector2d::getArea(void) const
  { return double_to_FT(getIncludedAngle())*getSquaredRadius()/2; }

//! @brief Return the maximum value of the i coordinate of the object points.
GEOM_FT CircularSector2d::GetMax(unsigned short int i) const
  {
    std::cerr << "CircularSector2d::GetMax() not implemented." << std::endl;
    return 0;
  }

//! @brief Return el valor mínimo of the i coordinate of the object points.
GEOM_FT CircularSector2d::GetMin(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to x que pasa por su centro.
GEOM_FT CircularSector2d::Ix(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to y que pasa por su centro.
GEOM_FT CircularSector2d::Iy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to z que pasa por su centro.
GEOM_FT CircularSector2d::Iz(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el product of inertia of the circular sector with respect to
//! the axis parallel to x e y que pasan por su centro.
GEOM_FT CircularSector2d::Pxy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return true if the points lies inside the object.
bool CircularSector2d::In(const Pos2d &p, const double &tol) const
  {
    if(Circle2d::In(p,tol))
      {
        const double th= getAngle(p);
        return ((th>=theta1) && (th<=theta2));
      }
    else 
      return false;
  }

//! @brief Return n points equally spaced over the arc of the circular sector.
//! @brief n: number of points (at least 2).
Pos2dArray CircularSector2d::getArcPoints(const size_t &n) const
  {
    Pos2dArray retval;
    if(n>1)
      {
        retval= Pos2dArray(n,1);
        if(n==2)
          {
            retval(1,1)= PInic(); //Initial and final points
            retval(n,1)= PFin();
          }
        else
          {
            const double delta_theta= getIncludedAngle()/(n-1);
            arc_points(theta1,delta_theta,retval);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the number of points must be greater than two."
		<< std::endl;
    return retval;
  }

//! @brief Return a Python list containing the points that results
//! from the division of the arc.
//!
//! @param n: number of points.
boost::python::list CircularSector2d::getArcPointsPy(const size_t &n) const
  {
    Pos2dArray tmp= this->getArcPoints(n);
    boost::python::list retval;
    for(Pos2dArray::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return a circular sector parallel to this one at the distance
//! being passed as parameter. The new circular sector will be exterior
//! if the distance is positive.
CircularSector2d CircularSector2d::offset(const GEOM_FT &d) const
  {
    Circle2d tmp(this->Centro(), this->getRadius()+d);
    return CircularSector2d(tmp, this->theta1, this->theta2);
  }

void CircularSector2d::Print(std::ostream &os) const
  {
    Circle2d::Print(os);
    os << " theta1= " << theta1
       << " theta2= " << theta2;
  }
void CircularSector2d::Plot(Plotter &plotter) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return;
  }

