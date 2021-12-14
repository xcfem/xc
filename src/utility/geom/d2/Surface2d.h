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
//Surface2d.h

#ifndef SURFACE2D_H
#define SURFACE2D_H

#include "../d2/GeomObj2d.h"

class Pos2d;

//! @ingroup GEOM
//
//! @brief Base class for surfaces in a two-dimensional space.
class Surface2d : public GeomObj2d
  {
  public:
    Surface2d(void): GeomObj2d(){}
    //! @brief Return the dimension of the object 0, 1, 2 or 3.
    inline virtual unsigned short int Dimension(void) const
      { return 2; }
    //! @brief Return the object length.
    virtual GEOM_FT getLength(void) const
      { return 0.0; }
    //! @brief Return the object perimeter.
    GEOM_FT getPerimeter(void) const
      { return getLength(); }
    //! @brief Return the object area.
    virtual GEOM_FT getArea(void) const;
    //! @brief Return the object volume
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual inline GEOM_FT IArea(void) const
      { return getArea(); }
    //@brief Return true if the point is on the surface.
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const= 0;
  };
#endif



