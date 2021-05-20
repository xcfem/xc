// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//PrincipalAxesOfInertia2D.h

#ifndef PRINCIPALAXESOFINERTIA2D_H
#define PRINCIPALAXESOFINERTIA2D_H

#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Vector2d.h"

class Ref2d2d;

//! @ingroup GEOM
//
//! @brief Principals axes of inertia in planar geometry.
class PrincipalAxesOfInertia2D
  {
    Pos2d center_of_mass; //! Centroid.
    Vector2d axis1; //!< Direction of the first principal moment of inertia.
    GEOM_FT i1; //!< First principal moment of inertia.
    GEOM_FT i2; //!< Second principal moment of inertia.
  public:
    PrincipalAxesOfInertia2D(const Pos2d &,const GEOM_FT &,const GEOM_FT &,const GEOM_FT &);
    Ref2d2d getAxis(void) const;
    const GEOM_FT &I1() const;
    const GEOM_FT &I2() const;
    inline Vector2d getAxis1VDir(void) const
      { return axis1; }
    Vector2d getAxis2VDir(void) const;
  };

std::ostream &operator<<(std::ostream &os,const PrincipalAxesOfInertia2D &axis);


#endif




