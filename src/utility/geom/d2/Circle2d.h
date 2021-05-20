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

#ifndef CIRCLE2D_H
#define CIRCLE2D_H

#include "Surface2d.h"
#include "../cgal_types.h"
#include "utility/utils/misc_utils/matem.h"

class Pos2d;
class Pos2dArray;
class Polygon2d;

//! @ingroup GEOM
//
//! @brief Círculo en dos dimensiones.
class Circle2d : public Surface2d
  {
    CGCircle_2 cgcirc;

  protected:
    void arc_points(const double &theta_inic,const double &delta_theta,Pos2dArray &ptos) const;
  public:
    Circle2d(void) : Surface2d(), cgcirc() {}
    Circle2d(const Pos2d &centro,const GEOM_FT &rad);
    Circle2d(const GEOM_FT &rad2,const Pos2d &centro);
    Circle2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3);
    
    virtual Surface2d *clon(void) const
      { return new Circle2d(*this); }
    Pos2d Centro(void) const;
    virtual Pos2d getCenterOfMass(void) const;
    Pos2d Point(const double &ang) const;
    inline GEOM_FT getSquaredRadius(void) const
      { return cgcirc.squared_radius(); }
    GEOM_FT getRadius(void) const;
    inline GEOM_FT getDiameter(void) const
      { return 2*getRadius(); }
    double getAngle(const Pos2d &p) const;
    virtual GEOM_FT getLength(void) const;
    //! @brief Return the perimeter of the object.
    inline GEOM_FT getPerimeter(void) const
      { return getLength(); }
    virtual double getIncludedAngle(void) const;
    virtual GEOM_FT getArea(void) const;
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const
      { return Ix(); }
    inline virtual GEOM_FT Pxy(void) const
      { return 0.0; }
    virtual GEOM_FT Iz(void) const
      { return 2*Ix(); }
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;
    Pos2dArray getPointsOnPerimeter(const size_t &n,const double &theta_inic= 0.0) const;
    Polygon2d getInscribedPolygon(const size_t &n,const double &theta_inic= 0.0) const;

    void Transform(const Trf2d &trf2d);

    friend int operator ==(const Circle2d &a,const Circle2d &b)
      { return ( a.cgcirc == b.cgcirc ); };
    
    void Print(std::ostream &os) const;
    void Plot(Plotter &) const;
  };

Circle2d Circle2dRTT(const GEOM_FT &,const Line2d &,const Line2d &,const bool &left,const bool &far);

#endif



