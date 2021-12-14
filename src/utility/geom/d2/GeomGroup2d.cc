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
//GeomGroup2d.cc

#include "GeomGroup2d.h"

#include <plotter.h>
#include <iostream>

#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/d0/Point2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d1/Ray2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d2/2d_polygons/Triangle2d.h"
#include "utility/geom/trf/Trf2d.h"

//! @brief Return el moment of inertia with respect to the line argument.
GEOM_FT GeomGroup2d::inertia(const Line2d &e) const
  {
    if(objetos.empty()) return 0.0;
    if(!equal_dimension())
      {
        cerr << getClassName() << "::" << __FUNCTION__
	     << "; warning, the object of the group have"
	     << " different dimensions." << endl;
      }
    pdeque_geom_obj::const_iterator i(objetos.begin());
    GEOM_FT retval((*i)->I(e));
    i++;
    for(;i!=objetos.end();i++)
      retval+= (*i)->I(e);
    return retval;
  }

//! @brief Moment of inertia with respect to an axis parallel to
//! the x axis passing through the center of mass.
GEOM_FT GeomGroup2d::Ix(void) const
  { return inertia(Line2d(getCenterOfMass(),Dir2d(1.0,0.0))); }

//! @brief Moment of inertia with respect to an axis parallel to
//! the y axis passing through the center of mass.
GEOM_FT GeomGroup2d::Iy(void) const
  { return inertia(Line2d(getCenterOfMass(),Dir2d(0.0,1.0))); }

//! @brief Moment of inertia with respect to the axis parallel to
//! the x and y axis passing through the center of mass.
GEOM_FT GeomGroup2d::Pxy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	 << "; not implemented, 0 is returned." << endl;
    return 0.0;
  }

//! @brief Return the center of mass.
Pos2d GeomGroup2d::getCenterOfMass(void) const
  {
    if(objetos.empty()) return Pos2d();
    if(!equal_dimension())
      {
        cerr << getClassName() << "::" << __FUNCTION__
	     << "; Warning!, the objects in the group"
	     << " have different dimensions." << endl;
      }
    pdeque_geom_obj::const_iterator i(objetos.begin());
    GEOM_FT area_i= (*i)->getCenterOfMassArea();
    Vector2d num= (*i)->getCenterOfMass().VectorPos()*area_i;
    GEOM_FT denom(area_i);
    i++;
    for(;i!=objetos.end();i++)
      {
        area_i= (*i)->getCenterOfMassArea();
        num= num + (*i)->getCenterOfMass().VectorPos()*area_i;
        denom+= area_i;
      }
    num= num*(1/denom);
    return Origin2d+num;
  }

//! @brief Applies the transformation to the points.
void GeomGroup2d::Transform(const Trf2d &trf2d)
  {
    for(pdeque_geom_obj::iterator i= objetos.begin();i!=objetos.end();i++)
      (*i)->Transform(trf2d);
  }

void GeomGroup2d::Print(std::ostream &stream) const
  {
    for(pdeque_geom_obj::const_iterator i= objetos.begin();i!=objetos.end();i++)
      (*i)->Print(stream);
  }
void GeomGroup2d::Plot(Plotter &plotter) const
  {
    for(pdeque_geom_obj::const_iterator i= objetos.begin();i!=objetos.end();i++)
      (*i)->Plot(plotter);
  }

