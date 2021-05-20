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
//Circle2d.h

#ifndef CIRCLE3D_H
#define CIRCLE3D_H

#include "D2to3d.h"
#include "Circle2d.h"

class Pos3d;
class Polygon3d;

//! @ingroup GEOM
//
//! @brief Círculo en tres dimensiones.
class Circle3d : public D2to3d
  {
    Circle2d circ;
  public:
    Circle3d(void);
    Circle3d(const Pos3d &,const GEOM_FT &);
    Circle3d(const GEOM_FT &,const Pos3d &);
    Circle3d(const Pos3d &,const Pos3d &,const Pos3d &);
    Circle3d(const Ref2d3d &,const Circle2d &);
    Circle3d(const Ref3d3d &,const Circle2d &);

    virtual D2to3d *clon(void) const
      { return new Circle3d(*this); }
    Pos3d Centro(void) const;
    virtual Pos3d getCenterOfMass(void) const;
    GEOM_FT getSquaredRadius(void) const;
    GEOM_FT getRadius(void) const;
    GEOM_FT getDiameter(void) const;
    double getAngle(const Pos3d &p) const;
    //! @brief Return the length of the object.
    virtual GEOM_FT getLength(void) const
      { return circ.getLength(); }
    //! @brief Return the perimeter of the object.
    inline GEOM_FT getPerimeter(void) const
      { return getLength(); }
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const
      { return circ.getArea(); }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0; }
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Pxy(void) const;
    virtual GEOM_FT Iz(void) const;

    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;

    virtual bool In(const Pos3d &p, const double &tol= 0.0) const;
    const Pos3dArray &getPointsOnPerimeter(const size_t &n,const double &theta_inic= 0.0) const;
    Polygon3d getInscribedPolygon(const size_t &n,const double &theta_inic= 0.0) const;

    friend bool operator ==(const Circle3d &a,const Circle3d &b);

    
    void Print(std::ostream &os) const;
  };

Circle3d Circle3dThreepoints(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);

#endif



