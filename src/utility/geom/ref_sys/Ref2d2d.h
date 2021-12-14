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
//Ref2d2d.h

#ifndef REF2D2D_H
#define REF2D2D_H

#include "Ref.h"
#include "../coo_sys/Rect2d2dCooSys.h"
#include "../d2/GeomObj2d.h"
#include "../pos_vec/Pos2d.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Dir2d.h"

class Line2d;

//! @ingroup SisRef
//! 
//! @brief Two-dimensional reference system defined in a 
//! two-dimensional space.
class Ref2d2d : public Ref<Rect2d2dCooSys>
  {
  public:
    typedef GeomObj2d::list_Pos2d list_Pos2d;
    typedef Ref<Rect2d2dCooSys> BaseRef;

  public:
    Ref2d2d(void);
    Ref2d2d(const Pos2d &o);
    Ref2d2d(const Pos2d &,const Rect2d2dCooSys &);
    Ref2d2d(const Pos2d &o,const Vector2d &vX);
    Ref2d2d(const Pos2d &o,const Dir2d &dirX);
    Ref2d2d(const Pos2d &o,const Pos2d &p);
    Vector2d getIVector(void) const; //I unary vector.
    Vector2d getJVector(void) const; //J unary vector.
    Line2d getXAxis(void) const; //Return the x axis.
    Line2d getYAxis(void) const; //Return the y axis.

    virtual ~Ref2d2d(void)
      {}
  };

#endif






