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
//PolYgonoWithHoles2d.h

#ifndef POLYGON_WITH_HOLES_2D_H
#define POLYGON_WITH_HOLES_2D_H

#include "utility/geom/d2/Surface2d.h"

class Line2d;
class Ray2d;
class Segment2d;
class Polygon2d;
class Polyline2d;


//! @ingroup GEOM
//
//! @brief Polígono que puede tener huecos.
class PolygonWithHoles2d: public Surface2d
  {
    CGPolygonWithHoles_2 cgpol; //CGAL polygon.
  public:
    typedef CGPolygonWithHoles_2::Hole_const_iterator Hole_const_iterator;
 
    PolygonWithHoles2d(void);
    explicit PolygonWithHoles2d(const Polygon2d &);
    virtual GeomObj *clon(void) const;

    bool In(const Pos2d &p, const double &tol) const;
    GEOM_FT getLength(void) const;
    inline GEOM_FT getPerimeter(void) const
      { return getLength(); }
    GEOM_FT getArea(void) const;
    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    Pos2d getCenterOfMass(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Pxy(void) const;

    PolygonWithHoles2d getTransformed(const Trf2d &);
    void Transform(const Trf2d &);

    void add_hole(const Polygon2d &);
    void contour(const Polygon2d &);

    void Print(std::ostream &) const;
  };

#endif



