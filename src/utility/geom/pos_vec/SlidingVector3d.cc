//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//SlidingVector3d.cc

#include "SlidingVector3d.h"
#include "utility/geom/d1/Line3d.h"





//! @brief Constructor.
SlidingVector3d::SlidingVector3d(const Pos3d &o,const Vector3d &v)
  : Vector3d(v), org(o) {}

//! @brief Constructor.
SlidingVector3d::SlidingVector3d(const Pos3d &o,const Pos3d &p)
  : Vector3d(p-o), org(o) {}

//! @brief Moment of a sliding vector with respect to an axis.
//! Is the moment with respect to an axis point
//! projected onto the axis.
GEOM_FT SlidingVector3d::getMoment(const Line3d &e) const
  { return dot(getMoment(e.Point()),e.VDir().getNormalized()); }

//! @brief Moment of a sliding vector with respect to a point.
SlidingVector3d SlidingVector3d::getMoment(const Pos3d &o) const
  {
    SlidingVector3d retval(o);
    if(!Nulo()) //Si ESTE vector no es nulo.
      {
        Vector3d r= org - o;
        if(!r.Nulo()) //Si r no es nulo.
          if(!parallel(r,*this)) //If r and THIS are not parallel.
            retval= SlidingVector3d(o,r^*this);
      }
    return retval;
  }

const Pos3d &SlidingVector3d::getOrg(void) const
  { return org; }

const Pos3d SlidingVector3d::getDest(void) const
  { return org+(const Vector3d &)(*this); }

//! @brief Return the vector part of the object.
const Vector3d &SlidingVector3d::getVector(void) const
  { return *this; }

//! @brief Return the vector part of the object.
//! This function exists because Python "recongnizes"
//! the address returned by getVector as a SlidingVector3d
//! object.
Vector3d SlidingVector3d::getVector3d(void) const
  { return *this; }

SlidingVector3d operator-(const SlidingVector3d &v)
  {
    SlidingVector3d neg(v);
    neg.Neg();
    return neg;
  }
SlidingVector3d  &SlidingVector3d::operator*=(const GEOM_FT &p)
  {
    (*this)= SlidingVector3d(getOrg(),getVector()*p);
    return *this;
  }

SlidingVector3d operator*(const SlidingVector3d &m,const GEOM_FT &p)
  { return SlidingVector3d(m.getOrg(),m.getVector()*p); }

SlidingVector3d operator*(const GEOM_FT &p,const SlidingVector3d &m)
  { return m*p; }

SlidingVector3d operator/(const SlidingVector3d &m,const GEOM_FT &p)
  { return m*(1/p); }

std::ostream &operator<<(std::ostream &os,const SlidingVector3d &v)
  {
    os << (const Vector3d &) v;
    os << " (O=" << v.org << ')';
    return os;
  }
