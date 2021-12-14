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
//GeomObj.cc

#include "GeomObj.h"

#include <iostream>

//! @brief Constructor
GeomObj::GeomObj(void) 
  : ProtoGeom() {}

//! Return the dimension used to compute the center of mass, as follows:
//! - dimension == 0 => return 1.0
//! - dimension == 1 => return getLength()
//! - dimension == 2 => return getArea()
//! - dimension == 3 => return getVolume()
GEOM_FT GeomObj::getCenterOfMassArea(void) const
  {
    unsigned short int d= Dimension();
    GEOM_FT retval= 1.0;
    switch(d)
      {
      case 0:
	retval= 1.0;
	break;
      case 1:
	retval= getLength();
	break;
      case 2:
	retval= getArea();
	break;
      case 3:
	retval= getVolume();
	break;
      default:
	retval= 1.0;
      }
    return retval;
  }

std::ostream &operator<<(std::ostream &os, const GeomObj &go)
  {
    go.Print(os);
    return os;
  }

