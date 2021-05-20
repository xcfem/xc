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
//Dir2d.cc

#include "Dir2d.h"
#include "Vector2d.h"

#include "utility/geom/FT_matrix.h"

//! @brief Constructor from a vector.
Dir2d::Dir2d(const Vector2d &v)
  : cgd(v.ToCGAL()) {}

//! @brief Constructor from x and y components.
Dir2d::Dir2d(const GEOM_FT &x,const GEOM_FT &y)
  : cgd(x,y) {}

//! @brief Constructor from an angle.
Dir2d::Dir2d(const double &ang)
  : cgd(cos(ang),sin(ang)) {}

//! @brief Comparison operator.
bool Dir2d::operator==(const Dir2d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      { retval= (cgd==other.cgd); }
    return retval;
  }

//! @brief returns the x and y components in a row matrix.
FT_matrix Dir2d::getMatrix(void) const
  {
    FT_matrix retval(2,1,0.0);
    retval(1)= cgd.dx(); retval(2)= cgd.dy();
    return retval;
  }

//! @brief returns the x and y components in a vector.
Vector2d Dir2d::GetVector(void) const
  { return Vector2d(cgd); }

//! @brief insertion into an output operator.
std::ostream &operator<<(std::ostream &stream,const Dir2d &n)
  {
    stream << "[[" << n.dx() << "][" << n.dy() << "]]";
    return stream;
  }
