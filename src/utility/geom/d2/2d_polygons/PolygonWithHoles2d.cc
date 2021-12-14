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
//PolygonWithHoles2d.cc

#include "PolygonWithHoles2d.h"
#include "Polygon2d.h"
#include "utility/geom/pos_vec/Vector2d.h"



//! @brief Default constructor.
PolygonWithHoles2d::PolygonWithHoles2d(void)
  : Surface2d(){}

//! @brief Default constructor.
PolygonWithHoles2d::PolygonWithHoles2d(const Polygon2d &p)
  : Surface2d(), cgpol(p.ToCGAL()) {}

//! @brief Constructor virtual.
GeomObj *PolygonWithHoles2d::getCopy(void) const
  { return new PolygonWithHoles2d(*this); }

void PolygonWithHoles2d::contour(const Polygon2d &plg)
  { cgpol= CGPolygonWithHoles_2(plg.ToCGAL()); }

//! @brief Return object length.
GEOM_FT PolygonWithHoles2d::getLength(void) const
  {
    GEOM_FT temp = 0;
    for(Hole_const_iterator i= cgpol.holes_begin();i!=cgpol.holes_end();i++)
      {
        const Polygon2d p(*i);
        temp += p.getLength();
      }
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        temp += p.getLength();
      }      
    return temp;
  }

//! @brief Return the object area.
GEOM_FT PolygonWithHoles2d::getArea(void) const
  {
    GEOM_FT temp = 0;
    for(Hole_const_iterator i= cgpol.holes_begin();i!=cgpol.holes_end();i++)
      {
        const Polygon2d p(*i);
        temp-= p.getArea();
      }
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        temp+= p.getArea();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; exterior contour doesn't exists." << std::endl;
    return temp;    
  }

//! @brief Return the maximum value of the i-th coordinate.
GEOM_FT PolygonWithHoles2d::GetMax(unsigned short int i) const
  { 
    GEOM_FT temp = 0;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        temp= p.GetMax(i);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        temp= std::max(temp,p.GetMax(i));
      }
    return temp;
  }

//! @brief Return the minimum value of the i-th coordinate.
GEOM_FT PolygonWithHoles2d::GetMin(unsigned short int i) const
  {
    GEOM_FT temp = 0;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        temp= p.GetMin(i);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        temp= std::min(temp,p.GetMin(i));
      }
    return temp;
  }

//! @brief Return true if the point is inside the object.
bool PolygonWithHoles2d::In(const Pos2d &p, const double &tol) const
  {
    bool retval= false;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d pol(cgpol.outer_boundary());
        retval= pol.In(p,tol);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    if(retval)
      for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
        {
          const Polygon2d pol(*j);
          if(pol.In(p,tol))
            {
              retval= false;
              break;
            }
        }
    return retval;

  }

//! @brief Return the center of mass.
Pos2d PolygonWithHoles2d::getCenterOfMass(void) const
  {
    GEOM_FT areaTotal= getArea();
    Vector2d vPos;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        vPos= p.getCenterOfMass().VectorPos()*p.getArea();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        vPos= vPos-p.getCenterOfMass().VectorPos()*p.getArea();
      }
    vPos= vPos*(1.0/areaTotal);
    return Pos2d(vPos.x(),vPos.y());
  }



//! @brief Compute moment of inertia with respect to an axis parallel to the
//! x axis that passes through thecenter of mass of the surface.
//! Ix = Integral y^2 dA
GEOM_FT PolygonWithHoles2d::Ix(void) const
  { 
    Pos2d center_of_mass=getCenterOfMass();
    GEOM_FT retval= 0.0;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        retval+= p.Ix()+p.getArea()*sqr(p.getCenterOfMass().y()-center_of_mass.y());
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        retval-= p.Ix()+p.getArea()*sqr(p.getCenterOfMass().y()-center_of_mass.y());
      }
    return retval;
  }

//! @brief Calcula el moment of inertia with respect to an axis parallel to the
//! y axis que pasa por el polygon centroid.
//! Iy = Integral x^2 dA
GEOM_FT PolygonWithHoles2d::Iy(void) const
  { 
    Pos2d center_of_mass=getCenterOfMass();
    GEOM_FT retval= 0.0;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        retval+= p.Iy()+p.getArea()*sqr(p.getCenterOfMass().x()-center_of_mass.x());
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        retval-= p.Iy()+p.getArea()*sqr(p.getCenterOfMass().x()-center_of_mass.x());
      }
    return retval;
  }

//! @brief Calcula el product of inertia with respect to the axis parallel
//! to the x and y that passes through the centroid of the polygon.
//! Pxy = Integral x*y dA
GEOM_FT PolygonWithHoles2d::Pxy(void) const
  {
    Pos2d center_of_mass=getCenterOfMass();
    GEOM_FT retval= 0.0;
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
        const GEOM_FT dx= p.getCenterOfMass().x()-center_of_mass.x();
        const GEOM_FT dy= p.getCenterOfMass().y()-center_of_mass.y();
        retval+= p.Pxy()+p.getArea()*dx*dy;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
        const GEOM_FT dx= p.getCenterOfMass().x()-center_of_mass.x();
        const GEOM_FT dy= p.getCenterOfMass().y()-center_of_mass.y();
        retval-= p.Iy()+p.getArea()*dx*dy;
      }
    return retval;
  }

//! @brief Hace un agujero en este polígono con
//! la forma del que se le pasa como parámetro.
void PolygonWithHoles2d::add_hole(const Polygon2d &p)
  { cgpol.add_hole(p.ToCGAL()); }

//! @brief Applies the transformation being passed as parameter.
PolygonWithHoles2d PolygonWithHoles2d::getTransformed(const Trf2d &trf2d)
  {
    PolygonWithHoles2d retval;
    if(!cgpol.is_unbounded())
      {
        Polygon2d p(cgpol.outer_boundary());
        p.Transform(trf2d);
        retval.cgpol= CGPolygonWithHoles_2(p.ToCGAL());
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        Polygon2d p(*j);
        p.Transform(trf2d);
        retval.cgpol.add_hole(p.ToCGAL());
      }
    return retval;
  }

//! @brief Applies the transformation being passed as parameter.
void PolygonWithHoles2d::Transform(const Trf2d &trf2d)
  {
    PolygonWithHoles2d tmp= getTransformed(trf2d);
    PolygonWithHoles2d::operator=(tmp); 
  }

void PolygonWithHoles2d::Print(std::ostream &os) const
  {
    if(!cgpol.is_unbounded())
      {
        const Polygon2d p(cgpol.outer_boundary());
	os << "contorno: " << p << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exterior contour doesn't exists." << std::endl;

    for(Hole_const_iterator j= cgpol.holes_begin();j!=cgpol.holes_end();j++)
      {
        const Polygon2d p(*j);
	os << "hueco: " << p << std::endl;
      }
  }
