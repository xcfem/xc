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
//Reflection2d.h
//Reflection in a two-dimensional space.

#ifndef REFLECTION2D_H
#define REFLECTION2D_H

#include "Trf2d.h"

class Line2d;


//! @ingroup GEOM
//
//! @brief Reflection in a two-dimensional space.
class Reflection2d: public Trf2d
  {
  private:
    static Reflection2d crea_reflection2d(const  Pos2d &Q,const Vector2d &d);

    Reflection2d( const GEOM_FT & m00,const GEOM_FT & m01,const GEOM_FT & m02,
                 const GEOM_FT & m10,const GEOM_FT & m11,const GEOM_FT & m12);
/*     Reflection2d( const GEOM_RT & m00,const GEOM_RT & m01,const GEOM_RT & m02, */
/*                  const GEOM_RT & m10,const GEOM_RT & m11,const GEOM_RT & m12, */
/*                  const GEOM_RT & hw = GEOM_RT(1.0)); */
  public:
    Reflection2d(void);
    Reflection2d(const Pos2d &Q,const Vector2d &d);
    Reflection2d(const Line2d &r);
  };

#endif
