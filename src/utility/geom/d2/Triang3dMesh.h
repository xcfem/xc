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
//Triang3dMesh.h

#ifndef TRIANG3D_MESH_H
#define TRIANG3D_MESH_H

#include "../d3/3d_polyhedrons/Polyhedron3d.h"

class Triangle3d;
class SoGroup;
class GTSSurface;
class Pos3d;


//! @ingroup GEOM
//
//! @brief 3D Triange mesh.
class Triang3dMesh: public Polyhedron3d
  {
    GTSSurface get_gts_surface(void) const;

    friend class Polyhedron3d;

  public:
    typedef Polyhedron3d::Vertex Vertex;
    typedef Polyhedron3d::Facet Facet;
    typedef Polyhedron3d::Point_iterator Point_iterator;
    typedef Polyhedron3d::Point_const_iterator Point_const_iterator;
    typedef Polyhedron3d::Vertex_iterator Vertex_iterator;
    typedef Polyhedron3d::Vertex_const_iterator Vertex_const_iterator;
    typedef Polyhedron3d::Facet_iterator Facet_iterator;
    typedef Polyhedron3d::Facet_const_iterator Facet_const_iterator;
    typedef Polyhedron3d::Edge_iterator Edge_iterator;
    typedef Polyhedron3d::Halfedge_iterator Halfedge_iterator;
    typedef Polyhedron3d::Halfedge_around_facet_circulator Halfedge_around_facet_circulator;
    typedef Polyhedron3d::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;
    typedef Polyhedron3d::Vertex_handle Vertex_handle;
    typedef Polyhedron3d::Facet_handle Facet_handle;
    typedef Polyhedron3d::Halfedge_handle Halfedge_handle;

  protected:
    Facet_const_iterator find_trihedron(const Pos3d &org,const Pos3d &p,const double &tol) const;

  public:

    Triang3dMesh(void): Polyhedron3d() {}
    explicit Triang3dMesh(const Polyhedron3d &p)
      : Polyhedron3d(p) {}
    Triang3dMesh(const GTSSurface &gts_surf);

    virtual GeomObj *clon(void) const
      { return new Triang3dMesh(*this); }

    Triangle3d getFaceTriangle(const Facet_const_iterator &f) const;

    Facet_const_iterator findTrihedron(const Pos3d &org,const Pos3d &p,const double &tol) const;

    friend Triang3dMesh GTSSurface2Triang3dMesh(const GTSSurface &gts_surf);
  };

#endif
