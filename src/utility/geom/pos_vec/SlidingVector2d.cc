//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//SlidingVector2d.cc

#include "SlidingVector2d.h"

//! @brief Constructor.
SlidingVector2d::SlidingVector2d(const Pos2d &o,const Pos2d &p)
  : Vector2d(p-o), org(o) {}

//! @brief Moment of a sliding vector with respect to a point.
GEOM_FT SlidingVector2d::getMoment(const Pos2d &o) const
  {
    GEOM_FT retval(0.0);
    if(!this->Nulo()) //Si ESTE vector no es nulo.
      {
        Vector2d r= org - o;
        if(!r.Nulo()) //Si r no es nulo.
          if(!parallel(r,*this)) //If r and THIS are not parallel.
            retval= r.x()*this->y()-this->x()*r.y();
      }
    return retval;
  }

//! @brief Return the vector part of the object.
const Vector2d &SlidingVector2d::getVector(void) const
  { return *this; }

//! @brief Return the vector part of the object.
//! This function exists because Python "recongnizes"
//! the address returned by getVector as a SlidingVector2d
//! object.
Vector2d SlidingVector2d::getVector2d(void) const
  { return *this; }

GEOM_FT inverso(const GEOM_FT &p)
  { return GEOM_FT(1.0/p); }

SlidingVector2d &SlidingVector2d::operator*=(const GEOM_FT &p)
  {
    (*this)= SlidingVector2d(getOrg(),getVector()*p);
    return *this;
  }

SlidingVector2d operator*(const SlidingVector2d &m,const GEOM_FT &p)
  { return SlidingVector2d(m.getOrg(),m.getVector()*p); }

SlidingVector2d operator*(const GEOM_FT &p,const SlidingVector2d &m)
  { return m*p; }

SlidingVector2d operator/(const SlidingVector2d &m,const GEOM_FT &p)
  { return m*inverso(p); }

std::ostream &operator<<(std::ostream &os,const SlidingVector2d &v)
  {
    os << (const Vector2d &) v;
    os << " (O=" << v.org << ')';
    return os;
  }

SlidingVector2d operator-(const SlidingVector2d &v)
  {
    SlidingVector2d neg(v);
    neg.Neg();
    return neg;
  }

