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
//VERTEXMAP.h

#ifndef VERTEXMAP_H
#define VERTEXMAP_H

#include <vector>
#include "utility/utils/stl/TripletMap.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "Polyhedron3d.h"

//Convenience classes to generate polyhedrons from
//half-space intersections.

class Pos3dList;


//! @ingroup GEOM
//
//! @brief Vertices of a polyhedron edge.
class VerticesArista
  {
    size_t v0;
    size_t v1;
  public:
    VerticesArista(const size_t &i,const size_t &j);
    void swap(void);
    const size_t &getVI(void) const
      { return v0; }
    const size_t &getVJ(void) const
      { return v1; }
    bool comparteVertices(const VerticesArista &other) const;
    friend bool operator==(const VerticesArista &,const VerticesArista &);
  };

bool operator==(const VerticesArista &,const VerticesArista &);

//! @ingroup GEOM
//
//! @brief Vertices of a polyhedron face.
class VerticesCara: public std::list<size_t>
  {
    bool visitada;
  public:
    typedef std::list<size_t> base;
    typedef base::iterator iterator;
    typedef base::const_iterator const_iterator;
    VerticesCara(void);
    void setVisitada(const bool &);
    const bool &yaVisitada(void) const;
    void swap(void);
    Pos3dList getPosVertices(const std::vector<Pos3d> &) const;
    Pos3d getCentroide(const std::vector<Pos3d> &) const;
    std::list<VerticesArista> getEdges(void) const;
    bool tieneArista(const VerticesArista &) const;
    bool tieneAristaOrientada(const VerticesArista &) const;
    std::deque<VerticesCara> compartenArista(const std::deque<VerticesCara> &) const;
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &, const VerticesCara &);

//! @ingroup GEOM
//
//! @brief Polyhedron vertex map.
class VertexMap
  {
    std::vector<Pos3d> vertices;
    std::deque<VerticesCara > caras;
  public:
    VertexMap(const TripletMap<Pos3d> &);
    const std::vector<Pos3d> &getVertices(void) const;
    Pos3d getCentroide(void) const;
    size_t getNumVertices(void) const;
    size_t getNumCaras(void) const;
    const std::deque<VerticesCara> &getCaras(void) const;
    const VerticesCara &getCara(const size_t &) const;
    std::deque<VerticesCara *> tienenArista(const VerticesArista &);
    void makeConsistent(void);
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &, const VertexMap &);

//! @brief Creates a polyhedron from a vertex map.
class BuildFromVertexMap: public CGAL::Modifier_base<Polyhedron3d::HalfedgeDS>
  {
    const VertexMap vertex_map;
  public:
    BuildFromVertexMap(const TripletMap<Pos3d> &);
    void operator()(Polyhedron3d::HalfedgeDS &);
  };
#endif
