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
//Triangle3d.h

#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H

#include "utility/geom/d2/Polygon3d.h"

class Plane;
class Triangle2d;

//! @ingroup GEOM
//
//! @brief Triangle in a three-dimensional space.
class Triangle3d: public Polygon3d
  {
    Triangle3d(const Ref2d3d &rf,const Triangle2d &p);

    inline void push_back(const Pos3d &p)
      { Polygon3d::push_back(p); }
  public:
    Triangle3d(void);
    Triangle3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    
    virtual GeomObj *clon(void) const
      { return new Triangle3d(*this); }

    inline virtual unsigned int getNumVertices(void) const
      { return 3; }
    GEOM_FT dist(const Pos3d &p) const;
    GEOM_FT dist2(const Pos3d &p) const;
  };

#endif
