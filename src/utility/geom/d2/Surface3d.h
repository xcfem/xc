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
//Surface3d.h

#ifndef SURFACE3D_H
#define SURFACE3D_H

#include "../d3/GeomObj3d.h"


class Pos3d;


//! @ingroup GEOM
//
//! @brief Base class for surfaces in a three-dimensional space.
class Surface3d : public GeomObj3d
  {
  public:
    Surface3d(void): GeomObj3d(){}

    //! @brief Return the dimension of the object 0, 1, 2 or 3.
    inline virtual unsigned short int Dimension(void) const
      { return 2; }
    //! @brief Return the length of the object.
    virtual GEOM_FT getLength(void) const
      { return 0.0; }
    GEOM_FT getPerimeter(void) const
      { return getLength(); }
    virtual GEOM_FT getArea(void) const;
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual inline GEOM_FT IArea(void) const
      { return getArea(); }
    virtual GEOM_FT dist(const Pos3d &p) const;
  };
#endif



