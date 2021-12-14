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
//Ref3d3d.h

#ifndef REF3D3D_H
#define REF3D3D_H

#include "Ref.h"
#include "../coo_sys/Rect3d3dCooSys.h"
#include "../d3/GeomObj3d.h"
#include "../pos_vec/Pos3d.h"

class Line3d;
class Vector3d;
class Plane;

//! @ingroup SisRef
//! 
//! @brief Three-dimensional reference system defined 
//! in a three-dimensional space.
class Ref3d3d : public Ref<Rect3d3dCooSys>
  {
  public:
    typedef GeomObj3d::list_Pos3d list_Pos3d;
    typedef Ref<Rect3d3dCooSys> BaseRef;

  public:
    Ref3d3d(void);
    explicit Ref3d3d(const Pos3d &o);
    Ref3d3d(const Pos3d &,const Rect3d3dCooSys &);
    Ref3d3d(const Pos3d &o,const Pos3d &p);
    Ref3d3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    Ref3d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2);
    Ref3d3d(const Pos3d &o,const Vector3d &v1,const Vector3d &v2,const Vector3d &v3);
    Ref3d3d(const Line3d &r,const Pos3d &p);
    Vector3d getIVector(void) const; //I unary vector.
    Vector3d getJVector(void) const; //J unary vector.
    Vector3d getKVector(void) const; //K unary vector.
    Line3d getXAxis(void) const; //Return the x axis.
    Line3d getYAxis(void) const; //Return the y axis.
    Line3d getZAxis(void) const; //Return the z axis.
    Plane getXYPlane(void) const; //Return the XY plane.
    Plane getXZPlane(void) const; //Return the XZ plane.
    Plane getYZPlane(void) const; //Return the YZ plane.
/*     inline virtual void GiraX(const double &ang_rad) */
/*       { trf.GiraX(ang_rad); } */
/*     inline virtual void GiraY(const double &ang_rad) */
/*       { trf.GiraY(ang_rad); } */
/*     inline virtual void GiraZ(const double &ang_rad) */
/*       { trf.GiraZ(ang_rad); } */

    //double GetZLocal(const Pos3d &p) const;
    //Return the la Z of the point p expressed in local coordinates
    //expressed in local coordinates.

    //Return the global coordinates of vector v from its
    //local coordinates.
/*     m_double Trf(const m_double &v) */
/*       { */
/*         m_double vt(3,1); */
/*         vt= trf * v; */
/*         return vt; */
/*       } */

    virtual ~Ref3d3d(void);
  };

#endif






