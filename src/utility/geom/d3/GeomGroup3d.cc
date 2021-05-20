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
//GeomGroup3d.cc

#include "GeomGroup3d.h"


#include "utility/geom/pos_vec/Dir3d.h"
#include "utility/geom/d0/Point3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d1/Ray3d.h"
#include "utility/geom/d1/Segment3d.h"

//Return the moment of inertia with respect to the line argument
GEOM_FT GeomGroup3d::inertia(const Line3d &e) const
  {
    if(objetos.empty()) return 0.0;
    if(!equal_dimension())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Warning!, the objects of the group have"
	          << " different dimensions." << std::endl;
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
GEOM_FT GeomGroup3d::Ix(void) const
  { return inertia(Line3d(getCenterOfMass(),Dir3d(1.0,0.0,0.0))); }

//! @brief Moment of inertia with respect to an axis parallel to
//! the y axis passing through the center of mass.
GEOM_FT GeomGroup3d::Iy(void) const
  { return inertia(Line3d(getCenterOfMass(),Dir3d(0.0,1.0,0.0))); }

//! @brief Moment of inertia with respect to an axis parallel to
//! the z axis passing through the center of mass.
GEOM_FT GeomGroup3d::Iz(void) const
  { return inertia(Line3d(getCenterOfMass(),Dir3d(0.0,0.0,1.0))); }

//! @brief Moment of inertia with respect to the axis parallel to
//! the x and y axis passing through the center of mass.
GEOM_FT GeomGroup3d::Pxy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented, 0 is returned." << std::endl;
    return 0.0;
  }

//! @brief Return the position of the object's center of mass.
Pos3d GeomGroup3d::getCenterOfMass(void) const
  {
    if(objetos.empty()) return Pos3d();
    if(!equal_dimension())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; Warning!, the objects of this group"
	          << " have different dimensions." << std::endl;
      }    pdeque_geom_obj::const_iterator i(objetos.begin());
    GEOM_FT area_i= (*i)->getCenterOfMassArea();
    Vector3d num= (*i)->getCenterOfMass().VectorPos()*area_i;
    GEOM_FT denom(area_i);
    i++;
    for(;i!=objetos.end();i++)
      {
        area_i= (*i)->getCenterOfMassArea();
        num= num + (*i)->getCenterOfMass().VectorPos()*area_i;
        denom+= area_i;
      }
    num= num*(1/denom);
    return Origin3d+num;
  }
