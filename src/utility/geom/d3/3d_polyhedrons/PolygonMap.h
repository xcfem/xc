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
//PolygonMap.h

#ifndef MAP_POLYGONS_H
#define MAP_POLYGONS_H

#include <deque>
#include <CGAL/iterator.h>
#include "../../cgal_types.h"
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <iostream>
#include <map>
#include "utility/geom/pos_vec/Pos3d.h"


//! @ingroup GEOM
//
//! @brief Almacena los indices de los vértices
//! de un polígono.
class PolygonVertexStorage
  {
  protected:
    std::deque<size_t> ind_vertices;

  public:
    PolygonVertexStorage(const size_t &sz,const size_t &V);
    inline size_t getNumVertices(void) const
      { return ind_vertices.size(); }
    inline const size_t &Vertice(const size_t &i) const
      { return ind_vertices[(i-1)]; }
    inline size_t &Vertice(const size_t &i)
      { return ind_vertices[(i-1)]; }
    void Print(std::ostream &os) const;
  };

template <class TPOL>
class PolygonVerticesIndices: public PolygonVertexStorage
  {
  public:
    typedef typename TPOL::Vertex_const_iterator VCI;
    typedef CGAL::Inverse_index<VCI> Index;
    typedef typename TPOL::Halfedge_around_facet_const_circulator HFCC;

  public:
    PolygonVerticesIndices(const Index &index,const HFCC &f);
  };

template <class TPOL>
PolygonVerticesIndices<TPOL>::PolygonVerticesIndices(const PolygonVerticesIndices<TPOL>::Index &index,const PolygonVerticesIndices<TPOL>::HFCC &ihc)
  : PolygonVertexStorage(circulator_size(ihc),0)
  {
    HFCC hc= ihc;
    HFCC hc_end = hc;
    CGAL_assertion(circulator_size( hc) >= 3);
    size_t i= 0;
    do
      {
        ind_vertices[i]= index[VCI(hc->vertex())];
        ++hc; i++;
      }
    while(hc != hc_end);
  }

template <class TPOS>
class MapPosVertices: protected std::map<size_t,TPOS>
  {

  public:
    typedef std::map<size_t,TPOS> map_p3d;
    typedef typename map_p3d::iterator iterator;
    typedef typename map_p3d::const_iterator const_iterator;

    inline const_iterator begin(void) const
      { return map_p3d::begin(); }
    inline const_iterator end(void) const
      { return map_p3d::end(); }
    inline iterator begin(void)
      { return map_p3d::begin(); }
    inline iterator end(void)
      { return map_p3d::end(); }
    void insert(const size_t &i,const TPOS &p);
    void Print(std::ostream &os) const;
  };

template <class TPOS>
void MapPosVertices<TPOS>::insert(const size_t &i,const TPOS &p)
  { (*this)[i]= p; }
template <class TPOS>
void MapPosVertices<TPOS>::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << "Vertice: " << (*i).first << ": " << (*i).second << std::endl;
  }

template <class TPOL>
class PolygonMap
  {
  public:
    typedef PolygonVerticesIndices<TPOL> IndVertices;
    typedef typename IndVertices::VCI VCI;
    typedef typename IndVertices::Index Index;
    typedef typename IndVertices::HFCC HFCC;
    typedef typename TPOL::Vertex::Point Point;
    typedef MapPosVertices<Point> map_pos_vertices;
    typedef typename map_pos_vertices::const_iterator vertices_const_iterator;
    typedef typename std::deque<IndVertices >::const_iterator caras_const_iterator;

  private:
    map_pos_vertices mv;
    std::deque<IndVertices> caras;
  public:

    PolygonMap(const TPOL &pol);

    vertices_const_iterator VerticesBegin(void) const
      { return mv.begin(); }
    vertices_const_iterator VerticesEnd(void) const
      { return mv.end(); }
    caras_const_iterator CarasBegin(void) const
      { return caras.begin(); }
    caras_const_iterator CarasEnd(void) const
      { return caras.end(); }

    void Print(std::ostream &os) const;
  };

template <class TPOL>
PolygonMap<TPOL>::PolygonMap(const TPOL &pol)
  {
    typedef typename TPOL::Facet_const_iterator FCI;
    size_t cont= 0;
    for(VCI vi = pol.vertices_begin();
            vi != pol.vertices_end(); ++vi)
      {
        mv.insert(cont,vi->point());
        cont++;
      }
    const Index index(pol.vertices_begin(),pol.vertices_end());
    for(FCI fi = pol.facets_begin(); fi != pol.facets_end(); ++fi)
      {
        HFCC hc = fi->facet_begin();
        caras.push_back(IndVertices(index,hc));
      }
  }

template <class TPOL>
void PolygonMap<TPOL>::Print(std::ostream &os) const
  {
    os << "vertex list: " << std::endl;
    mv.Print(os);
    os << "face list: " << std::endl;
    for(caras_const_iterator i= caras.begin();i!=caras.end();i++)
      {
        (*i).Print(os);
        os << std::endl;
      }
  }


template <class TPOL>
std::ostream &operator<<(std::ostream &os, const PolygonMap<TPOL> &mt)
  {
    mt.Print(os);
    return os;
  }

template <class TPOL>
PolygonMap<TPOL> getPolygonMap(const TPOL &pol)
  { return PolygonMap<TPOL>(pol); }

template <class TPOLORG,class HDS,class CVPOS>
class Build_tdest_polyhedron: public CGAL::Modifier_base<HDS>
  {
    const TPOLORG &sf;

  public:
    typedef typename HDS::Vertex Vertex;
    typedef typename Vertex::Point Point;
    typedef typename PolygonMap<TPOLORG>::vertices_const_iterator vconst_iter;
    typedef typename PolygonMap<TPOLORG>::caras_const_iterator cconst_iter;

  public:
    Build_tdest_polyhedron(const TPOLORG &pol)
      : sf(pol) {}
    void operator()(HDS& hds);
  };

template <class TPOLORG,class HDS,class CVPOS>
void Build_tdest_polyhedron<TPOLORG,HDS,CVPOS>::operator()(HDS& hds)
  {
    const size_t num_facetas= sf.size_of_facets();
    const size_t num_edges= sf.size_of_halfedges();
    const size_t num_vertices= sf.size_of_vertices();
    PolygonMap<TPOLORG> mt= getPolygonMap(sf);
    // Postcondition: `hds' is a valid polyhedral surface.
    CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);

    B.begin_surface(num_vertices,num_facetas,num_edges);
    for(vconst_iter i= mt.VerticesBegin();i!=mt.VerticesEnd();i++)
      {
	auto p= (*i).second;
        B.add_vertex(CVPOS()(p));        
      }
    for(cconst_iter j= mt.CarasBegin();j!=mt.CarasEnd();j++)
      {
        B.begin_facet();
        const size_t nv= (*j).getNumVertices();
        for(size_t i=0;i<nv;i++)
          B.add_vertex_to_facet((*j).Vertice(i+1));
        B.end_facet();
      }
    B.end_surface();
  }

#endif
