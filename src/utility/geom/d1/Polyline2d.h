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
//Polyline2d.h

#ifndef POLYLINE2D_H
#define POLYLINE2D_H

#include "../pos_vec/Pos2d.h"
#include "Linear2d.h"

class Segment2d;
class Ray2d;
class Line2d;
class Pos2dList;

//! @ingroup GEOM
//
//! @brief Polyline in a two-dimensional space.
class Polyline2d : public Linear2d, public GeomObj::list_Pos2d
  {
  public:
    Polyline2d(void);
    explicit Polyline2d(const GeomObj::list_Pos2d &);
    explicit Polyline2d(const Pos2dList &);
    explicit Polyline2d(const boost::python::list &);

    virtual GeomObj *clon(void) const
      { return new Polyline2d(*this); }
    const GeomObj::list_Pos2d &getVertices(void) const;
    const GeomObj::list_Pos2d &getVertexList(void) const;
    boost::python::list getVertexListPy(void) const;
    inline size_t getNumVertices(void) const
      { return GeomObj::list_Pos2d::size(); }
    size_t getNumSegments(void) const;
    inline const_iterator vertices_begin(void) const
      { return GeomObj::list_Pos2d::begin(); }
    const const_iterator vertices_end(void) const
      { return GeomObj::list_Pos2d::end(); }

    void Transform(const Trf2d &trf2d);
    
/*     inline virtual void GiraX(double ang) */
/*       { Ref2d::GiraX(ang); TrfPoints(); } */
/*     inline virtual void GiraY(double ang) */
/*       { Ref2d::GiraY(ang); TrfPoints();  } */
/*     inline virtual void GiraZ(double ang) */
/*       { Ref2d::GiraZ(ang); TrfPoints();  } */
    const Pos2d *AgregaVertice(const Pos2d &p);
    //! @brief Insert the vertices between [first,last) antes
    template <class InputIterator>
    inline void assign(InputIterator first, InputIterator last)
      { GeomObj::list_Pos2d::assign(first,last); }
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;

    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Iz(void) const;
    
    //! @brief Inertia product.
    inline virtual GEOM_FT Pxy(void) const
      { return NAN; }
    //! @brief Return the length of the object.
    virtual GEOM_FT getLength(void) const
      { return GeomObj::list_Pos2d::getLength(); }
    virtual GEOM_FT GetMax(unsigned short int i) const;
    //Return the maximum value of the i coordinate.
    virtual GEOM_FT GetMin(unsigned short int i) const;
    //! @brief Return the minimum value of the i coordinate.
    Polyline2d GetMayores(unsigned short int i,const GEOM_FT &d) const;
    Polyline2d GetMenores(unsigned short int i,const GEOM_FT &d) const;
    const Pos2d &Vertice(const size_t &i) const;
    Segment2d getSegment(const const_iterator &i) const;
    Segment2d getSegment(const size_t &i) const;
/*     virtual list_Pos2d Int(unsigned short int i, const double d) const */
/*       { */
/*         cerr << "Polyline2d Int(i,d) not implemented" << endl; */
/*         return list_Pos2d(); */
/*       } */
    Polyline2d Offset(const GEOM_FT &d) const;
    //! @brief Return the intersections of the polyline with
    //! the line coord_i= d
    // list_Pos2d Int(unsigned short int i,const GEOM_FT &d) const;
    list_Pos2d getIntersection(const Line2d &r) const;
    list_Pos2d getIntersection(const Ray2d &sr) const;
    list_Pos2d getIntersection(const Segment2d &sr) const;
    //Polyline2d Corta(unsigned short int i,const GEOM_FT &d) const;
    Polyline2d Separa(const Pos2d &p,const short int &sgn) const;
    Pos2d getCenterOfMass(void) const
      { return GeomObj::list_Pos2d::getCenterOfMass(); }

    iterator getFarthestPointFromSegment(iterator it1, iterator it2, GEOM_FT &pMaxDist);

    void Print(std::ostream &stream) const;
    void Plot(Plotter &) const;
  };

GeomObj::list_Pos2d intersection(const Polyline2d &p,const Line2d &r);
GeomObj::list_Pos2d intersection(const Polyline2d &p,const Ray2d &);
GeomObj::list_Pos2d intersection(const Polyline2d &p,const Segment2d &);
GeomObj::list_Pos2d intersection(const Line2d &,const Polyline2d &);
GeomObj::list_Pos2d intersection(const Ray2d &,const Polyline2d &p);
GeomObj::list_Pos2d intersection(const Segment2d &,const Polyline2d &p);

#endif






