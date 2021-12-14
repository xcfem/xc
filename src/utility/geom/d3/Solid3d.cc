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
//Solid3d.cc

#include "Solid3d.h"
#include <limits>       // std::numeric_limits

//! @brief Constructor.
Solid3d::Solid3d(void)
  : GeomObj3d(){}

//! @brief Return the distance to the argument.
GEOM_FT Solid3d::dist(const Pos3d &p) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented."
              << std::endl;
    return std::numeric_limits<double>::quiet_NaN( );
  }

void Solid3d::Print(std::ostream &os) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented."
              << std::endl;
  }
