// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//PrincipalAxes3D.h

#ifndef PRINCIPALAXESOFINERTIA3D_H
#define PRINCIPALAXESOFINERTIA3D_H

#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"

class Ref3d3d;

//! @ingroup GEOM
//
//! @brief Principals axes of inertia in planar geometry.
class PrincipalAxes3D
  {
    Pos3d center_of_mass; //! Centroid.
    Vector3d axis1; //!< Direction of the first principal moment of inertia.
    Vector3d axis2; //!< Direction of the second principal moment of inertia.
    GEOM_FT i1; //!< First principal moment of inertia.
    GEOM_FT i2; //!< Second principal moment of inertia.
    GEOM_FT i3; //!< Third principal moment of inertia.
  public:
    PrincipalAxes3D(void);
    PrincipalAxes3D(const Pos3d &, const double A[3][3]);
    PrincipalAxes3D(const Pos3d &, const GEOM_FT &, const GEOM_FT &, const GEOM_FT &, const GEOM_FT &, const GEOM_FT &, const GEOM_FT &);
    inline Pos3d getOrg(void) const
      { return center_of_mass; }
    Ref3d3d getAxis(void) const;
    const GEOM_FT &I1() const;
    const GEOM_FT &I2() const;
    const GEOM_FT &I3() const;
    inline Vector3d getAxis1VDir(void) const
      { return axis1; }
    inline Vector3d getAxis2VDir(void) const
      { return axis2; }
    Vector3d getAxis3VDir(void) const;
  };

std::ostream &operator<<(std::ostream &os,const PrincipalAxes3D &axis);


#endif




