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
//GeomGroup3d.h

#ifndef GEOMGROUP3D_H
#define GEOMGROUP3D_H

#include "utility/utils/stl/pdeque.h"
#include "GeomObj3d.h"
#include "utility/geom/lists/GeomGroup.h"

class Line3d;


//! @ingroup GEOM
//
//! @brief Group of 3D entities.
class GeomGroup3d : public GeomGroup<GeomObj3d>
  {

    virtual GEOM_FT inertia(const Line3d &e) const;
  public:
    typedef pdeque_geom_obj::const_iterator const_iterator;
    typedef pdeque_geom_obj::iterator iterator;


    GeomGroup3d(void) : GeomGroup<GeomObj3d>() {}

    virtual GeomObj3d *clon(void) const
      { return new GeomGroup3d(*this); }

    virtual Pos3d getCenterOfMass(void) const;

    virtual GEOM_FT Ix(void) const;
    //Moment of inertia with respect to the center of mass in local coordinates.
    virtual GEOM_FT Iy(void) const;
    //Moment of inertia with respect to the center of mass in local coordinates.
    virtual GEOM_FT Iz(void) const;
    //Moment of inertia with respect to the center of mass in local coordinates.
    GEOM_FT Pxy(void) const;
    //product of inertia with respect to the center of mass in local coordinates.

    
  };

#endif
