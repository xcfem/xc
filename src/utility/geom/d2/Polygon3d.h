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
//Polygon3d.h

#ifndef POLYGON3D_H
#define POLYGON3D_H

#include "utility/geom/d2/D2to3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "../cgal_types.h"
#include "utility/geom/d2/Plane.h"

//! @ingroup GEOM
//
//! @brief Plane polygon in a 3D space.
class Polygon3d: public D2to3d
  {
    Polygon2d plg2d;
  public:
    Polygon3d(void): D2to3d(),plg2d() {}
    template <typename InputIterator>
    explicit Polygon3d(InputIterator first,InputIterator last);
    Polygon3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    Polygon3d(const Ref2d3d &,const Polygon2d &p= Polygon2d());
    Polygon3d(const Ref3d3d &,const Polygon2d &p= Polygon2d());
    explicit Polygon3d(const GeomObj::list_Pos3d &);
    explicit Polygon3d(const boost::python::list &);

    virtual GeomObj *clon(void) const
      { return new Polygon3d(*this); }

    inline void push_back(const Pos3d &p)
      { plg2d.push_back(to_2d(p)); }

    inline virtual unsigned int GetNumVertices(void) const
      { return plg2d.GetNumVertices(); }
    inline virtual unsigned int GetNumLados(void) const
      { return plg2d.GetNumLados(); }
    //! @brief Return the length of the object.
    inline GEOM_FT getLength(void) const
      { return plg2d.getLength(); }
    //! @brief Return the area of the object.
    inline GEOM_FT getArea(void) const
      { return plg2d.getArea(); }
    std::vector<Polygon3d> getTributaryPolygons(void) const;
    inline std::vector<double> getTributaryAreas(void) const
      { return plg2d.getTributaryAreas(); }
    //! @brief Return the position of the i-th vertex.
    inline Pos3d Vertice(unsigned int i) const
      { return to_3d(plg2d.Vertice(i)); }
    //! @brief Return the position of the i-th vertex
    //! (0 based: j=0..GetNumVertices()-1)
    inline Pos3d Vertice0(unsigned int j) const
      { return to_3d(plg2d.Vertice0(j)); }
    GeomObj::list_Pos3d getVertexList(void) const;
    boost::python::list getVertexListPy(void) const;
    Segment3d Lado0(unsigned int i) const;
    Segment3d Lado(unsigned int i) const;

    bool clockwise(const Pos3d &) const;
    bool counterclockwise(const Pos3d &) const;
    void swap(void);
    std::string orientation(const Pos3d &) const;

    Plane getPlaneFromSide0(unsigned int i) const;
    Plane getPlaneFromSide(unsigned int i) const;

    Pos3d getCenterOfMass(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Iz(void) const;

    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    bool In(const Pos3d &p,const double &tol) const;
    bool TocaCuadrante(const int &) const;

    GEOM_FT distSigno(const Pos3d &p) const;
    GEOM_FT dist(const Pos3d &p) const;
    GEOM_FT distSigno2(const Pos3d &p) const;
    GEOM_FT dist2(const Pos3d &p) const;

    std::list<Polygon3d> Corta(const Plane &p) const;

    void Print(std::ostream &os) const;
  };

template <typename InputIterator>
Polygon3d::Polygon3d(InputIterator begin,InputIterator end)
  : D2to3d(), plg2d()
  {
    Plane p(begin, end);
    auto i= begin;
    const Pos3d A= *i; i++;
    const Pos3d B= *i; i++;
    const Pos3d C= *i;
    const Ref2d3d ref= Ref2d3d(*begin,p.Base1(), p.Base2());
    Polygon3d tmp(ref);
    for(InputIterator i= begin; i!= end; i++)
      tmp.push_back(*i);
    (*this)= tmp;

  }

inline std::list<Polygon3d> corta(const Polygon3d &pol,const Plane &pl)
  { return pol.Corta(pl); }

#endif
