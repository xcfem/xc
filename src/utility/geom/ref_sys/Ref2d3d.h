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
//Ref2d3d.h

#ifndef REF2D3D_H
#define REF2D3D_H

#include "Ref.h"
#include "../coo_sys/Rect2d3dCooSys.h"
#include "../d3/GeomObj3d.h"
#include "../pos_vec/Pos3d.h"

class Line3d;
class Dir3d;
class Plane;
class Vector3d;
class Ref3d3d;

//! @ingroup SisRef
//! 
//! @brief Two-dimensional reference system defined in a 
//! three-dimensional space.
class Ref2d3d : public Ref<Rect2d3dCooSys>
  {
  public:
    typedef GeomObj3d::list_Pos3d list_Pos3d;
    typedef Ref<Rect2d3dCooSys> BaseRef;
  public:
    Ref2d3d(void);
    explicit Ref2d3d(const Pos3d &o);
    Ref2d3d(const Pos3d &,const Rect2d3dCooSys &);
    Ref2d3d(const Pos3d &o,const Pos3d &p,const Pos3d &q);
    Ref2d3d(const Pos3d &o,const Vector3d &v);
    Ref2d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2);
    explicit Ref2d3d(const Ref3d3d &);
    
    Vector3d getIVector(void) const; //I unary vector.
    Vector3d getJVector(void) const; //J unary vector.
    Vector3d getKVector(void) const; //K unary vector.
    Line3d getXAxis(void) const; //Return the x axis.
    Line3d getYAxis(void) const; //Return the y axis.
    //void PointVector(const Pos3d &o,const Vector3d &vX);

    Plane getXYPlane(void) const; //Return the XY plane.

  };

#endif






