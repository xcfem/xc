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
//Dir3d.cc

#include "Dir3d.h"
#include "Vector3d.h"

#include "utility/geom/FT_matrix.h"

//! @brief Constructor from (x,y,z) components.
Dir3d::Dir3d(GEOM_FT x,GEOM_FT y,GEOM_FT z)
  : cgd(x,y,z) {}

//! @brief Constructor from a Vector3d.
Dir3d::Dir3d(const Vector3d &v)
  : cgd(v.ToCGAL()) {}

//! @brief Comparison operator.
bool Dir3d::operator==(const Dir3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      { retval= (cgd==other.cgd); }
    return retval;
  }

//! @brief Returns the x, y and z components in a row matrix.
FT_matrix Dir3d::getMatrix(void) const
  {
    FT_matrix retval(3,1,0.0);
    retval(1)= cgd.dx(); retval(2)= cgd.dy(); retval(3)= cgd.dz();
    return retval;
  }

//! @brief Returns the x, y and z components in a vector.
Vector3d Dir3d::GetVector(void) const
  { return Vector3d(cgd.vector()); }

//! @brief Insertion into an output stream.
std::ostream &operator<<(std::ostream &stream,const Dir3d &n)
  {
    stream << "[[" << n.dx() << "][" << n.dy() << "]["
           << n.dz() << "]]";
    return stream;
  }
