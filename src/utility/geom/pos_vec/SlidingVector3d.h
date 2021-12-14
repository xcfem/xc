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
//SlidingVector3d.h

#ifndef VDESLIZ3D_H
#define VDESLIZ3D_H


#include "Pos3d.h"
#include "Vector3d.h"
#include "utility/utils/misc_utils/mchne_eps.h"

class Line3d;

//! @ingroup GEOM
//
//! @brief sliding vector en tres dimensiones.
class SlidingVector3d : public Vector3d
  {
  protected:
    Pos3d org; //!< Point that fix the line of action.
  public:
    SlidingVector3d(const Pos3d &o= Pos3d(),const Vector3d &v= Vector3d());
    SlidingVector3d(const Pos3d &o,const Pos3d &p);
    virtual SlidingVector3d getMoment(const Pos3d &o) const;
    virtual GEOM_FT getMoment(const Line3d &e) const;
    const Pos3d &getOrg(void) const;
    const Pos3d getDest(void) const;
    const Vector3d &getVector(void) const;
    Vector3d getVector3d(void) const;
    SlidingVector3d &operator*=(const GEOM_FT &);
    friend SlidingVector3d operator*(const SlidingVector3d &m,const GEOM_FT &p);
    friend SlidingVector3d operator*(const GEOM_FT &p,const SlidingVector3d &m);
    friend SlidingVector3d operator/(const SlidingVector3d &m,const GEOM_FT &p);
    friend std::ostream &operator<<(std::ostream &os,const SlidingVector3d &v);
  };

SlidingVector3d operator*(const SlidingVector3d &m,const GEOM_FT &p);
SlidingVector3d operator*(const GEOM_FT &p,const SlidingVector3d &m);
SlidingVector3d operator/(const SlidingVector3d &m,const GEOM_FT &p);
std::ostream &operator<<(std::ostream &os,const SlidingVector3d &v);
SlidingVector3d operator-(const SlidingVector3d &v);

inline GEOM_FT Abs(const SlidingVector3d &v)
  { return v.GetModulus(); }

#endif
