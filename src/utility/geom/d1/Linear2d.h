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
//Linear2d.h

#ifndef LINEAR2D_H
#define LINEAR2D_H

#include <stdlib.h>
#include "../d2/GeomObj2d.h"


//! @ingroup GEOM
//
//! @brief Base class for the objects involving a single dimension
//! in a two-dimensional space.
class Linear2d : public GeomObj2d
  {
  public:
    Linear2d(void): GeomObj2d(){}
    ~Linear2d(void) {}
    virtual GeomObj *getCopy(void) const= 0;
    inline virtual unsigned short int Dimension(void) const
      { return 1; }
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const
      { return 0.0; }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    //@brief Return true if the points is on sobre la Linea.
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const= 0;
  };
#endif



