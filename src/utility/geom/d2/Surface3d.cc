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
//Surface3d.cc

#include "Surface3d.h"
#include <cstdlib>
#include <cmath>
#include "utility/utils/misc_utils/matem.h"

//! @brief Return the object area.
GEOM_FT Surface3d::getArea(void) const
  { return NAN; }

//! @brief Return the distance from the point to this surface.
GEOM_FT Surface3d::dist(const Pos3d &p) const
  {
    std::cout << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return NAN;
  }

//! @brief Return the squared distance from the point to this surface.
GEOM_FT Surface3d::dist2(const Pos3d &p) const
  {
    std::cout << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return NAN;
  }
