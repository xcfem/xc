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
//Circle2d.cc

#include "Circle2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/ref_sys/Ref2d2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include <plotter.h>

#include "utility/geom/trf/Trf2d.h"




//! @brief Circle defined by two tangents and the radius.
//!
//! Basado en Mathematical elements for computer graphics
//! (Rogers and Adams) page 234 y siguientes.
Circle2d Circle2dRTT(const GEOM_FT &radius,const Line2d &p,const Line2d &l,const bool &left,const bool &far)
  {
    std::cerr << __FUNCTION__
	      << "; no está probada." << std::endl;
    const GeomObj2d::list_Pos2d points_int= p.getIntersection(l);
    if(points_int.size()<1)
      {
        std::cerr << __FUNCTION__
		  << "; error; parallel lines." << std::endl;
        return Circle2d();
      }
    else
      {
        const Pos2d &o= *points_int.begin();
        const Line2d bisect= bisectriz(p,l);
        Ref2d2d ref(o,bisect.GetDir()); //Origin on intersection, x axis aligned with the bisecting line.
        const double theta= angle(bisect,p); //Angle between the bisecting line and the p line.
        const GEOM_FT sintheta= double_to_FT(sin(theta));
        const GEOM_FT costheta= double_to_FT(cos(theta));
	GEOM_FT h= 0,k= 0;
        if(left) //Círculos C2 o C3 de la figura.
          {
            if(far) //Círculo C2 de la figura.
              k= radius/costheta; //y coordinate of the center expressed in the local system.
            else //Círculo C3 de la figura.
              h= -radius/sintheta; //x coordinate of the center expressed in the local system.
          }
        else //Círculos C1 o C4 de la figura.
          {
            if(far) //Círculo C1 de la figura.
              h= -radius/sintheta; //x coordinate of the center expressed in the local system.
            else //Círculo C4 de la figura.
              k= -radius/costheta; //y coordinate of the center expressed in the local system.
          }
        const Pos2d centro= ref.getGlobalPosition(Pos2d(h,k)); //Centro del círculo.
        return Circle2d(centro,radius);
      }
  }

//! @brief Build the circle from its center and its radius.
Circle2d::Circle2d(const Pos2d &centro,const GEOM_FT &rad)
  : Surface2d(), cgcirc(centro.ToCGAL(),rad*rad) {}

//! @brief Build the circle from its center and its squared radius.
Circle2d::Circle2d(const GEOM_FT &rad2,const Pos2d &centro)
  : Surface2d(), cgcirc(centro.ToCGAL(),rad2) {}

//! @brief Build the circle from three points.
//!
//! Basado en Mathematical elements for computer graphics
//! (Rogers and Adams) page 233.
Circle2d::Circle2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3)
  : Surface2d(), cgcirc(p1.ToCGAL(),p2.ToCGAL(),p3.ToCGAL())
  {}
Pos2d Circle2d::Centro(void) const
  { return Pos2d(cgcirc.center()); }

//! @brief Return the position of the center of mass.
Pos2d Circle2d::getCenterOfMass(void) const
  { return Centro(); }

//! @brief Return the point of the circle that corresponds to
//! the parameter angle.
Pos2d Circle2d::Point(const double &ang) const
  {
    const Pos2d o= Centro();
    const GEOM_FT r= getRadius();
    const GEOM_FT x= o.x()+r*double_to_FT(cos(ang));
    const GEOM_FT y= o.y()+r*double_to_FT(sin(ang));
    return Pos2d(x,y);
  }

//! @brief Return the radius of the circle.
GEOM_FT Circle2d::getRadius(void) const
  { return sqrt_FT(getSquaredRadius()); }

//! @brief Return the angle between the line that passes through the center
//! and the p point and the x axis.
double Circle2d::getAngle(const Pos2d &p) const
  {
    static const Vector2d horiz(1,0);
    Vector2d v1(Centro(),p);
    double retval= angle(v1,horiz);
    return retval;
  }

//! @brief Return the length of the círcle.
GEOM_FT Circle2d::getLength(void) const
  { return M_PI_FT*getDiameter(); }
//! @brief Return the area of the circle.
GEOM_FT Circle2d::getArea(void) const
  { return M_PI_FT*getSquaredRadius(); }
//! @brief Return the maximum value of the i coordinate of the points of the circle.
GEOM_FT Circle2d::GetMax(unsigned short int i) const
  { return Centro()(i)+getRadius(); }
//! @brief Return el valor mínimo of the i coordinate of the points of the circle.
GEOM_FT Circle2d::GetMin(unsigned short int i) const
  { return Centro()(i)-getRadius(); }
//! @brief Return el moment of inertia del círculo with respect to 
//! axis parallel to x que pasa por su centro.
GEOM_FT Circle2d::Ix(void) const
  { return M_PI_FT/4*sqr(getSquaredRadius()); }


//! @brief Return true if the point is on the circle.
bool Circle2d::In(const Pos2d &p, const double &tol) const
  { return (cgcirc.has_on_positive_side(p.ToCGAL()) || cgcirc.has_on_boundary(p.ToCGAL())); }


//! @brief Return the angle included by the circle (2*pi).
double Circle2d::getIncludedAngle(void) const
  { return 2*M_PI; }

//! @brief Return n points equally espaces over the circle perimenter.
void Circle2d::arc_points(const double &theta_inic,const double &delta_theta,Pos2dArray &ptos) const
  {
    const GEOM_FT r= getRadius();
    GEOM_FT x= r*double_to_FT(cos(theta_inic));
    GEOM_FT y= r*double_to_FT(sin(theta_inic));
    const size_t &n= ptos.size();
    const GEOM_FT cosdth= double_to_FT(cos(delta_theta));
    const GEOM_FT sindth= double_to_FT(sin(delta_theta));
    ptos(1,1)= Pos2d(x,y);
    for(size_t i= 2;i<=n;i++)
      {
        x= ptos(i-1,1).x()*cosdth-ptos(i-1,1).y()*sindth;
        y= ptos(i-1,1).x()*sindth+ptos(i-1,1).y()*cosdth;
        ptos(i,1)= Pos2d(x,y);
      }
    const Vector2d v_translac= Centro().VectorPos();
    for(size_t i= 1;i<=n;i++) //Traslación.
      ptos(i,1)= ptos(i,1)+v_translac;
    return;
  }

//! @brief Return n points equally spaced over the cicumference.
Pos2dArray Circle2d::getPointsOnPerimeter(const size_t &n,const double &theta_inic) const
  {
    Pos2dArray retval;
    if(n>0)
      {
        retval= Pos2dArray(n,1);
        const double delta_theta= 2*M_PI/n;
        arc_points(theta_inic,delta_theta,retval);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: number of points must be greater than zero."
		<< std::endl;
    return retval;
  }

//! @brief Return the n-gon inscribed int the circle.
Polygon2d Circle2d::getInscribedPolygon(const size_t &n,const double &theta_inic) const
  {
    Polygon2d retval;
    const Pos2dArray points= getPointsOnPerimeter(n,theta_inic);
    for(size_t i= 1;i<=n;i++)
      retval.push_back(points(i,1));
    return retval;
  }

//! @brief Aplica al objeto la transformación que se
//! pasa como parámetro.
void Circle2d::Transform(const Trf2d &trf2d)
  {
    const Pos2d pA= trf2d.Transform(getCenterOfMass());
    (*this)= Circle2d(pA,getRadius());
  }

void Circle2d::Print(std::ostream &os) const
  {
    os << "x centro: " << Centro().x() 
       << " y centro: " << Centro().y()
       << " radius: " << getRadius();
  }
void Circle2d::Plot(Plotter &plotter) const
  {
    const Pos2d c= Centro();
    plotter.fcircle(c.x(),c.y(),getRadius());
  }
