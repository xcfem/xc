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
//GeomGroup2d.h

#ifndef GEOMGROUP2D_H
#define GEOMGROUP2D_H

#include "utility/utils/stl/pdeque.h"
#include "GeomObj2d.h"
#include "utility/geom/lists/GeomGroup.h"

class Line2d;

//! @ingroup GEOM
//
//! @brief  Group of 3D entities.
class GeomGroup2d : public GeomGroup<GeomObj2d>
  {

    virtual GEOM_FT inertia(const Line2d &e) const;
  public:
    typedef pdeque_geom_obj::const_iterator const_iterator;
    typedef pdeque_geom_obj::iterator iterator;


    GeomGroup2d(void) : GeomGroup<GeomObj2d>() {}

    virtual GeomObj2d *clon(void) const
      { return new GeomGroup2d(*this); }

    //! @brief Return the volume of the object.
    inline virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual Pos2d getCenterOfMass(void) const;

    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    GEOM_FT Pxy(void) const;

    void Print(std::ostream &stream) const;
    void Plot(Plotter &) const;

    void Transform(const Trf2d &trf2d);
  };

#endif
