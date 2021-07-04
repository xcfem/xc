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
//Square2d.h

#ifndef SQUARE2D_H
#define SQUARE2D_H

#include "Quadrilateral2d.h"


//! @ingroup GEOM
//
//! @brief Square in a two-dimensional space.
class Square2d : public Quadrilateral2d
  {
  public:
    Square2d(const GEOM_FT &l= 1.0)
      : Quadrilateral2d(Pos2d(0,0),Pos2d(l,0),Pos2d(l,l),Pos2d(0,l)) {}
    Square2d(const Pos2d &o,const GEOM_FT &l= 1.0);
    //Square2d(const Ref2d &ref,const GEOM_FT &l= 1.0): PolygonalSurface(ref), lado(l) {}
    ~Square2d(void) {}

    virtual GeomObj *getCopy(void) const
      { return new Square2d(*this); }
/*     void Offset(const GEOM_FT &offset) */
/*       { lado+=2*offset; } */
/*     Square2d GetOffset(const GEOM_FT &offset) const */
/*       { */
/*         Square2d retval(*this); */
/*         retval.Offset(offset); */
/*         return retval; */
/*       } */
    

  };

#endif






