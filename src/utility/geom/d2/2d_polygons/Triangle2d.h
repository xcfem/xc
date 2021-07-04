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
//Triangle2d.h

#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include "PolygonalSurface2d.h"
#include "utility/geom/cgal_types.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/utils/misc_utils/matem.h"

class Segment2d;
class Polygon2d;


//! @ingroup GEOM
//
//! @brief Triangle in a two-dimensional space.
class Triangle2d : public PolygonalSurface2d
  {
    CGTriangle_2 cgtriang;
  public:
    Triangle2d(void) : PolygonalSurface2d(), cgtriang() {}
    Triangle2d(const Pos2d p1,const Pos2d &p2,const Pos2d &p3)
     : PolygonalSurface2d(), cgtriang(p1.ToCGAL(),p2.ToCGAL(),p3.ToCGAL())
      {}

    virtual PolygonalSurface2d *getCopy(void) const
      { return new Triangle2d(*this); }
    inline virtual unsigned int getNumVertices(void) const
      { return 3; }
    bool isDegenerated(void) const
      { return cgtriang.is_degenerate(); }
    Pos2d Vertice(unsigned int i) const
      { return Vertice0(i-1); }
    Pos2d Vertice0(unsigned int j) const
      { return Pos2d(cgtriang.vertex(j)); }
    Segment2d Base(const size_t &i) const; //Return la base opuesta al vertice i.
    Segment2d Altura(const size_t &i) const; //Return la altura del vertice i.
    virtual Pos2d getCenterOfMass(void) const;
    virtual GEOM_FT getLength(void) const;
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const
      { return cgtriang.area(); }
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const
      { return 0.0; }
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    //! @brief Return true if the point is inside the triangle.
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const
      { return (cgtriang.has_on_positive_side(p.ToCGAL()) || cgtriang.has_on_boundary(p.ToCGAL())); }

    Polygon2d getPolygon(void) const;

    void Transform(const Trf2d &trf2d);

    friend int operator ==(const Triangle2d &a,const Triangle2d &b)
      { return ( a.cgtriang == b.cgtriang ); };
    inline friend bool intersecan(const Triangle2d &tr1,const Triangle2d &tr2)
      { return do_intersect(tr1.cgtriang,tr2.cgtriang); }
    
    void Print(std::ostream &os) const;
    void Plot(Plotter &) const;
  };
#endif



