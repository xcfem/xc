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
//d2to3D.h

#ifndef D2TO3D_H
#define D2TO3D_H

#include "utility/geom/d2/Surface3d.h"
#include "utility/geom/ref_sys/Ref2d3d.h"
#include "../cgal_types.h"

class Plane;
class Line2d;
class Segment2d;
class Line3d;
class Segment3d;
class Pos3dArray;
class Pos2dArray;

//! @ingroup GEOM
//
//! @brief Class that places 2D objects
//! (circle, ellipse, polygons, ...) in a 3D space.
class D2to3d: public Surface3d
  {
    Ref2d3d ref;

  protected:
    explicit D2to3d(const Ref2d3d &);
    explicit D2to3d(const Ref3d3d &);
    explicit D2to3d(const Pos3d &o);
    D2to3d(const Pos3d &o,const Pos3d &p,const Pos3d &q);
    D2to3d(const Pos3d &o,const Vector3d &n,const Vector3d &i);

  public:
    D2to3d(void);
    
    virtual bool operator==(const D2to3d &) const;
    void ThreePoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);

    //! @brief Return the dimension of the object: 0, 1, 2 or 3.
    inline virtual unsigned short int Dimension(void) const
      { return 2; }

    
    Vector3d getIVector(void) const; //I unary vector.
    Vector3d getJVector(void) const; //J unary vector.
    Vector3d getKVector(void) const; //K unary vector.

    Pos3d to_3d(const Pos2d &) const;
    Line3d to_3d(const Line2d &) const;
    Segment3d to_3d(const Segment2d &) const;
    Pos3dArray to_3d(const Pos2dArray &) const;
    Pos2d to_2d(const Pos3d &) const;
    Line2d to_2d(const Line3d &) const;
    Segment2d to_2d(const Segment3d &) const;
    const Ref2d3d &getRef(void) const
      { return ref; }
    Plane getPlane(void) const;

    void Print(std::ostream &os) const;
  };

#endif
