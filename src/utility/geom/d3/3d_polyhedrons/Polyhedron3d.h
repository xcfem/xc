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
//POLYHEDRON3D.h

#ifndef POLYHEDRON3D_H
#define POLYHEDRON3D_H

#include <iostream>
#include "PolyhedronBase.h"
#include "../../cgal_types.h"
#include "PolygonMap.h"
#include "enriched_polyhedron.h"

class Plane;
class HalfSpace3d;
class Triang3dMesh;
class Polygon3d;


//! @ingroup GEOM
//
//! @brief Base class for polyhedrons in a three-dimensional space.
class Polyhedron3d: public PolyhedronBase
  {
  protected:
    CGPolyhedron_3 cgpolyhedron;

    explicit Polyhedron3d(const CGPolyhedron_3 &cgp) 
      : PolyhedronBase(), cgpolyhedron(cgp) {}
    void make_tetrahedron(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3);
    void make_tetrahedron(const HalfSpace3d &, const HalfSpace3d &,const HalfSpace3d &, const HalfSpace3d &);
    void make_polyhedron(const std::deque<HalfSpace3d> &);
    //void halfSpaces(const std::string &,const bool &clear= true);

  public:
    typedef CGPolyhedron_3::Vertex Vertex;
    typedef CGPolyhedron_3::Facet Facet;

    typedef CGPolyhedron_3::Point_iterator Point_iterator;
    typedef CGPolyhedron_3::Point_const_iterator Point_const_iterator;
    typedef CGPolyhedron_3::Vertex_iterator Vertex_iterator;
    typedef CGPolyhedron_3::Vertex_const_iterator Vertex_const_iterator;
    typedef CGPolyhedron_3::Facet_iterator Facet_iterator;
    typedef CGPolyhedron_3::Facet_const_iterator Facet_const_iterator;
    typedef CGPolyhedron_3::Edge_iterator Edge_iterator;
    typedef CGPolyhedron_3::Halfedge_iterator Halfedge_iterator;
    typedef CGPolyhedron_3::Halfedge_around_facet_circulator Halfedge_around_facet_circulator;
    typedef CGPolyhedron_3::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;
    typedef CGPolyhedron_3::Vertex_handle Vertex_handle;
    typedef CGPolyhedron_3::Facet_handle Facet_handle;
    typedef CGPolyhedron_3::Halfedge_handle Halfedge_handle;
    typedef CGPolyhedron_3::HalfedgeDS HalfedgeDS;

    Polyhedron3d(void);
    Polyhedron3d(const Pos3d &, const Pos3d &,const Pos3d &, const Pos3d &);
    Polyhedron3d(const HalfSpace3d &, const HalfSpace3d &,const HalfSpace3d &, const HalfSpace3d &);
    Polyhedron3d(const std::deque<HalfSpace3d> &);
    Polyhedron3d(const Polyhedron3d &);
    Polyhedron3d(const EPolyhedron &e);
    Polyhedron3d &operator=(const Polyhedron3d &);
    virtual GeomObj *getCopy(void) const;

    Point_iterator points_begin(void)
      { return cgpolyhedron.points_begin(); }
    Point_iterator points_end(void)
      { return cgpolyhedron.points_end(); }
    Vertex_iterator vertices_begin(void)
      { return cgpolyhedron.vertices_begin(); }
    Vertex_iterator vertices_end(void)
      { return cgpolyhedron.vertices_end(); }
    Facet_iterator facets_begin(void)
      { return cgpolyhedron.facets_begin(); }
    Facet_iterator facets_end(void)
      { return cgpolyhedron.facets_end(); }
    Point_const_iterator points_begin(void) const
      { return cgpolyhedron.points_begin(); }
    Point_const_iterator points_end(void) const
      { return cgpolyhedron.points_end(); }
    Vertex_const_iterator vertices_begin(void) const
      { return cgpolyhedron.vertices_begin(); }
    Vertex_const_iterator vertices_end(void) const
      { return cgpolyhedron.vertices_end(); }
    Facet_const_iterator facets_begin(void) const
      { return cgpolyhedron.facets_begin(); }
    Facet_const_iterator facets_end(void) const
      { return cgpolyhedron.facets_end(); }

    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    size_t getNumVertices(void) const
      { return cgpolyhedron.size_of_vertices(); }
    size_t getNumEdges(void) const
      { return cgpolyhedron.size_of_halfedges(); }
    size_t GetNumCaras(void) const
      { return cgpolyhedron.size_of_facets(); }

    void clear(void)
      { cgpolyhedron.clear(); }

    GeomObj::list_Pos3d GetVerticesCara(const Facet_const_iterator &f) const;
    Plane getPlaneFromFace(const Facet_const_iterator &fi) const;
    Polygon3d GetCara(const Facet_const_iterator &f) const;
    std::deque<Polygon3d> GetCaras(void) const;

    GeomObj::list_Pos3d getVertices(void) const;

    virtual GEOM_FT getArea(void) const;

    bool TocaCuadrante(const int &) const;

    PolygonMap<CGPolyhedron_3> GetPolygonMap(void) const
      { return getPolygonMap(cgpolyhedron); }
    EPolyhedron GetEnriquecido(void) const;
    Triang3dMesh Triangula(void) const;

    GEOM_FT PseudoDist(const Pos3d &p) const;
    //! @brief Return the distance to the point
    //! (negative if the point is inside the solid).
    inline virtual GEOM_FT dist(const Pos3d &p) const
      { return PseudoDist(p); }

    friend Polyhedron3d Union(const Polyhedron3d &a,const Polyhedron3d &b);
    friend Polyhedron3d intersection(const Polyhedron3d &a,const Polyhedron3d &b);
    friend Polyhedron3d Diferencia(const Polyhedron3d &a,const Polyhedron3d &b);
    friend Polyhedron3d subdivide_quad_triangle(const Polyhedron3d &pol,bool smooth_boundary);
    template <class InputIterator>
    friend Polyhedron3d get_convex_hull(InputIterator first,InputIterator last);

    void Print(std::ostream &os) const;
    void Print(CGview_stream &v) const;
    void ReadOFF(std::istream &is);
  };

CGview_stream &operator<<(CGview_stream &v,const Polyhedron3d &p);

inline PolygonMap<CGPolyhedron_3> getPolygonMap(const Polyhedron3d &p)
  { return p.GetPolygonMap(); }
inline EPolyhedron enriquece(const Polyhedron3d &p)
  { return p.GetEnriquecido(); }

Polyhedron3d subdivide_quad_triangle(const Polyhedron3d &pol,bool smooth_boundary= false);

#endif
