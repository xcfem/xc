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
//SlidingVector2d.h

#ifndef VDESLIZ2D_H
#define VDESLIZ2D_H


#include "Pos2d.h"
#include "Vector2d.h"
#include "utility/utils/misc_utils/mchne_eps.h"


//! @ingroup GEOM
//
//! @brief sliding vector en dos dimensiones.
class SlidingVector2d : public Vector2d
  {
    Pos2d org; //Point that fixes the line of action.
  public:
    SlidingVector2d(const Pos2d &o= Pos2d(),const Vector2d &v= Vector2d())
      : Vector2d(v), org(o) {}
    SlidingVector2d(const Pos2d &o,const Pos2d &p);

    virtual GEOM_FT getMoment(const Pos2d &o) const;
    virtual const Pos2d &getOrg(void) const
      { return org; }
    const Pos2d getDest(void) const
      { return org+(const Vector2d &)(*this); }
    const Vector2d &getVector(void) const;
    Vector2d getVector2d(void) const;
    SlidingVector2d &operator*=(const GEOM_FT &);
    friend SlidingVector2d operator*(const SlidingVector2d &m,const GEOM_FT &p);
    friend SlidingVector2d operator*(const GEOM_FT &p,const SlidingVector2d &m);
    friend SlidingVector2d operator/(const SlidingVector2d &m,const GEOM_FT &p);
    friend std::ostream &operator<<(std::ostream &os,const SlidingVector2d &v);
  };

SlidingVector2d operator*(const SlidingVector2d &m,const GEOM_FT &p);
SlidingVector2d operator*(const GEOM_FT &p,const SlidingVector2d &m);
SlidingVector2d operator/(const SlidingVector2d &m,const GEOM_FT &p);
std::ostream &operator<<(std::ostream &os,const SlidingVector2d &v);
SlidingVector2d operator-(const SlidingVector2d &v);

inline GEOM_FT Abs(const SlidingVector2d &v)
  { return v.GetModulus(); }

#endif
