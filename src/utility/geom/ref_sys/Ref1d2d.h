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
//Ref1d2d.h

#ifndef REF1D2D_H
#define REF1D2D_H

#include "Ref.h"
#include "../coo_sys/Rect1d2dCooSys.h"
#include "../d2/GeomObj2d.h"
#include "../pos_vec/Pos2d.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Dir2d.h"

class Line2d;
class Dir2d;
class Ref2d2d;

//! @ingroup SisRef
//! 
//! @brief 1D Reference system defined in a
//! bi-dimensional one.
class Ref1d2d : public Ref<Rect1d2dCooSys>
  {
  public:
    typedef GeomObj2d::list_Pos2d list_Pos2d;
    typedef Ref<Rect1d2dCooSys> BaseRef;

  public:
    Ref1d2d(void);
    explicit Ref1d2d(const Pos2d &);
    Ref1d2d(const Pos2d &,const Rect1d2dCooSys &);
    Ref1d2d(const Pos2d &o,const Vector2d &vX);
    Ref1d2d(const Pos2d &o,const Dir2d &dirX);
    Ref1d2d(const Pos2d &o,const Pos2d &p);
    explicit Ref1d2d(const Ref2d2d &);

    Vector2d getIVector(void) const;
    //! @brief Return the I unary vector of the global system.
    Line2d getXAxis(void) const; //Return the line defining x axis.

    virtual ~Ref1d2d(void)
      {}
  };

#endif






