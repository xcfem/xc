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
//modifier_quad_triangle.h

#ifndef MODIFIER_QUAD_TRIANGLE
#define MODIFIER_QUAD_TRIANGLE

#include "enriched_polyhedron.h"
#include "builder.h"

// Implemented from:
// Stam and Loop. Quad/Triangle Subdivision. 
// Computer Graphics Forum.
// Volume 22, Issue 1, March 2003.

// Another reference is:
// Levin. Polynomial Generation and Quasi-Interpolation 
// in Stationary Non-Uniform Subdivision.
// Computer Aided Geometric Design.
// pages 41-60, volume 20(1), 2003.

float correction_factor(unsigned int ne, unsigned int nq);


template <class HDS,class Polyhedron,class kernel>
class CModifierQuadTriangle: public CGAL::Modifier_base<HDS>
  {
  private:
    typedef typename HDS::Vertex          Vertex;
    typedef typename Vertex::Point        Point;
    typedef typename HDS::Face_handle     Face_handle;
    typedef typename HDS::Halfedge_handle Halfedge_handle;
    typedef typename CGAL::Enriched_polyhedron_incremental_builder_3<HDS> builder;

    Polyhedron *m_pMesh;

  public:
    // life cycle
    CModifierQuadTriangle(Polyhedron *pMesh)
      {
        CGAL_assertion(pMesh != nullptr);
        m_pMesh = pMesh;
      }
    ~CModifierQuadTriangle() {}

    // subdivision
    void operator()( HDS& hds)
      {
        builder B(hds,true);
        B.begin_surface(3,1,6);
          add_vertices(B);
          add_facets(B);
        B.end_surface();
      }

    // add vertices
    int add_original_vertices(builder &B);
    int addv_centro_arista(builder &B,const int &idx);
    int addv_centro_cara_no_triang(builder &B,const int &idx);
    void add_vertices(builder &B);
    // add facets
    void addf_trata_cara_triangular(builder &B,typename Polyhedron::Facet_iterator pFacet);
    void addf_trata_cara_no_triangular(builder &B,typename Polyhedron::Facet_iterator pFacet);
    void add_facets(builder &B);

    // correcting factor
    static float correcting_factor(unsigned int ne,unsigned int nq)
      { return correction_factor(ne,nq); }
  
    // smooth vertex positions
    static void smooth_trata_vertice_borde(const unsigned int &index,Polyhedron *pMesh,typename Polyhedron::Vertex_iterator pVertex,typename kernel::FT *pPos,bool smooth_boundary);
    static void smooth_trata_vertice_interior(const unsigned int &index,Polyhedron *pMesh,typename Polyhedron::Vertex_iterator pVertex,typename kernel::FT *pPos);
    static void smooth_set_new_positions(Polyhedron *pMesh,typename kernel::FT *pPos);
    static void smooth(Polyhedron *pMesh,bool smooth_boundary = true);
  };

// add vertices
template <class HDS,class Polyhedron,class kernel>
int CModifierQuadTriangle<HDS,Polyhedron,kernel>::add_original_vertices(builder &B)
  {
    // put original vertices
    int index = 0;
    typename Polyhedron::Vertex_iterator pVertex;
    for(pVertex = m_pMesh->vertices_begin();
        pVertex != m_pMesh->vertices_end();
        pVertex++)
      {
        pVertex->tag(index);
        B.add_vertex(pVertex->point());
        index++;
      }
    return index;
  }
template <class HDS,class Polyhedron,class kernel>
int CModifierQuadTriangle<HDS,Polyhedron,kernel>::addv_centro_arista(builder &B,const int &idx)
  {
    //un nuevo vértice en el centro de cada arista
    int index= idx;
    m_pMesh->tag_halfedges(-1);
    typename Polyhedron::Halfedge_iterator pHalfedge;
    for(pHalfedge = m_pMesh->halfedges_begin();
        pHalfedge != m_pMesh->halfedges_end();
        pHalfedge++)
      {
        if(pHalfedge->tag() != -1) continue;

        // simple edge bisection
        const Point& p1 = pHalfedge->vertex()->point();
        const Point& p2 = pHalfedge->opposite()->vertex()->point();
        const typename kernel::FT x= (p1.x()+p2.x())/2;
        const typename kernel::FT y= (p1.y()+p2.y())/2;
	const typename kernel::FT z= (p1.z()+p2.z())/2;
/*         const typename kernel::RT wsx = x.numerator() * y.denominator() * z.denominator();  */
/*         const typename kernel::RT wsy = y.numerator() * x.denominator() * z.denominator();  */
/*         const typename kernel::RT wsz = z.numerator() * x.denominator() * y.denominator();  */
/*         const typename kernel::RT wsh  = x.denominator() * y.denominator() * z.denominator(); */
/*         Point point = Point(wsx,wsy,wsz,wsh); */
        Point point = Point(x,y,z,1.0);
        B.add_vertex(point);

        // put vertex indices on both halfedges
        pHalfedge->tag(index);
        pHalfedge->opposite()->tag(index);
        index++;
      }
    return index;
  }
template <class HDS,class Polyhedron,class kernel>
int CModifierQuadTriangle<HDS,Polyhedron,kernel>::addv_centro_cara_no_triang(builder &B,const int &idx)
  {
    //un nuevo vértice en cada cara no triangular (degree > 3).
    int index= idx;
    m_pMesh->tag_facets(-1);
    typename Polyhedron::Facet_iterator pFacet;
    for(pFacet = m_pMesh->facets_begin();
        pFacet != m_pMesh->facets_end();
        pFacet++)
      {
        unsigned int degree = Polyhedron::degree(pFacet);
        CGAL_assertion(degree >= 3);
        // forget about triangles, they are
        // simply 1-to-4 subdivided by edge bisection
        if(degree == 3) continue; 

        // barycentric subdivision
        Point barycenter;
        m_pMesh->compute_facet_center(pFacet,barycenter);
        B.add_vertex(barycenter);
        pFacet->tag(index);
        index++;
      }
    return index;
  }
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::add_vertices(builder &B)
  {
    // put original vertices
    int index = add_original_vertices(B);

    // as many as #edges
    index= addv_centro_arista(B,index);

    // and as many as #facets with degree > 3
    index= addv_centro_cara_no_triang(B,index);
  }

//add facets: Trata cara triangular.
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::addf_trata_cara_triangular(builder &B,typename Polyhedron::Facet_iterator pFacet)
  {
    typename Polyhedron::Halfedge_handle pHalfedge = pFacet->halfedge();
    int i0 = pHalfedge->tag();
    int i1 = pHalfedge->vertex()->tag();
    int i2 = pHalfedge->next()->tag();
    int i3 = pHalfedge->next()->vertex()->tag();
    int i4 = pHalfedge->next()->next()->tag();
    int i5 = pHalfedge->next()->next()->vertex()->tag();
    CGAL_assertion(i0 >= 0);
    CGAL_assertion(i1 >= 0);
    CGAL_assertion(i2 >= 0);
    CGAL_assertion(i3 >= 0);
    CGAL_assertion(i4 >= 0);
    CGAL_assertion(i5 >= 0);

    // create 4 triangles
    B.begin_facet();
      B.add_vertex_to_facet(i1);
      B.add_vertex_to_facet(i2);
      B.add_vertex_to_facet(i0);
    const Halfedge_handle& h1 = B.end_facet();
    B.begin_facet();
      B.add_vertex_to_facet(i3);
      B.add_vertex_to_facet(i4);
      B.add_vertex_to_facet(i2);
    const Halfedge_handle& h2 = B.end_facet();
    B.begin_facet();
      B.add_vertex_to_facet(i5);
      B.add_vertex_to_facet(i0);
      B.add_vertex_to_facet(i4);
    const Halfedge_handle& h3 = B.end_facet();

    // center face
    B.begin_facet();
      B.add_vertex_to_facet(i0);
      B.add_vertex_to_facet(i2);
      B.add_vertex_to_facet(i4);
    Halfedge_handle h4 = B.end_facet();

    h1->control_edge(false);
    h1->next()->control_edge(false);
    h1->next()->next()->control_edge(false);

    h2->control_edge(false);
    h2->next()->control_edge(false);
    h2->next()->next()->control_edge(false);

    h3->control_edge(false);
    h3->next()->control_edge(false);
    h3->next()->next()->control_edge(false);

    h4->control_edge(false);
    h4->next()->control_edge(false);
    h4->next()->next()->control_edge(false);
        
    if(pHalfedge->control_edge())
      {
        h1->control_edge(true);
        h3->next()->control_edge(true);
      }
    if(pHalfedge->next()->control_edge())
      {
        h1->next()->control_edge(true);
        h2->control_edge(true);
      }
    if(pHalfedge->next()->next()->control_edge())
      {
        h2->next()->control_edge(true);
        h3->control_edge(true);
      }
  }

//add facets: Trata cara triangular.
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::addf_trata_cara_no_triangular(builder &B,typename Polyhedron::Facet_iterator pFacet)
  {
    // i1: index of barycenter vertex
    int i1 = pFacet->tag();
    CGAL_assertion(i1 >= 0);

    // for each halfedge
    typename Polyhedron::Halfedge_around_facet_circulator h;
    h = pFacet->facet_begin();
    do
      {
        // i2,3,4: indices of three consecutive
        // vertices on halfedges
        int i2 = h->tag();
        int i3 = h->vertex()->tag();
        int i4 = h->next()->tag();
        CGAL_assertion(i2 >= 0);
        CGAL_assertion(i3 >= 0);
        CGAL_assertion(i4 >= 0);

        // create a quad
        B.begin_facet();
          B.add_vertex_to_facet(i3);
          B.add_vertex_to_facet(i4);
          B.add_vertex_to_facet(i1);
          B.add_vertex_to_facet(i2);
        const Halfedge_handle& pNewHalfedge = B.end_facet();

        pNewHalfedge->control_edge(false);
        pNewHalfedge->next()->control_edge(false);
        pNewHalfedge->next()->next()->control_edge(false);
        pNewHalfedge->next()->next()->next()->control_edge(false);

        if(h->control_edge())
          pNewHalfedge->control_edge(true);
        if(h->next()->control_edge())
          pNewHalfedge->next()->control_edge(true);
      }
    while(++h != pFacet->facet_begin());
  }
//add facets
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::add_facets(builder &B)
  {
    typename Polyhedron::Facet_iterator pFacet;
    for(pFacet = m_pMesh->facets_begin();
        pFacet != m_pMesh->facets_end();
        pFacet++)
      {
        unsigned int degree = Polyhedron::degree(pFacet);
        CGAL_assertion(degree >= 3);

        if(degree == 3)
          addf_trata_cara_triangular(B,pFacet);
        else //degree > 3
          addf_trata_cara_no_triangular(B,pFacet);
      }
  }

// smooth vertex positions
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::smooth_trata_vertice_borde(const unsigned int &index,Polyhedron *pMesh,typename Polyhedron::Vertex_iterator pVertex,typename kernel::FT *pPos,bool smooth_boundary)
  {
    const Point& curr = pVertex->point();
    if(!smooth_boundary)  // do not smooth it
      {
        pPos[3*index]   = curr.x();
        pPos[3*index+1] = curr.y();
        pPos[3*index+2] = curr.z();
      }
    else // smooth using [1/4 1/2 1/4] cubic B-spline averaging mask 
      {
        const typename Polyhedron::Halfedge_handle& pHalfedge = pMesh->get_border_halfedge(pVertex);
        CGAL_assertion(pHalfedge != nullptr);
        const Point& next = pHalfedge->next()->vertex()->point();
        const Point& prev = pHalfedge->prev()->vertex()->point();
        pPos[3*index]   = prev.x()/4 + curr.x()/2 + next.x()/4;
        pPos[3*index+1] = prev.y()/4 + curr.y()/2 + next.y()/4;
        pPos[3*index+2] = prev.z()/4 + curr.z()/2 + next.z()/4;
      }
  }
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::smooth_trata_vertice_interior(const unsigned int &index,Polyhedron *pMesh,typename Polyhedron::Vertex_iterator pVertex,typename kernel::FT *pPos)
  {
    unsigned int nb_quads = 0;
    unsigned int nb_edges = 0;
    typedef typename kernel::FT kern_FT;

    // rotate around vertex to count #edges and #quads
    typename Polyhedron::Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
    typename Polyhedron::Halfedge_around_vertex_circulator end = pHalfEdge;
    CGAL_For_all(pHalfEdge,end)
      {
        const typename Polyhedron::Facet_handle& pFacet = pHalfEdge->facet();
        CGAL_assertion(pFacet != nullptr);
        unsigned int degree = Polyhedron::degree(pFacet);
        CGAL_assertion(degree == 4 || degree == 3);
        if(degree == 4) nb_quads++;
        nb_edges++;
      }

    // compute coefficients
    kern_FT ne = kern_FT(int(nb_edges));
    kern_FT nq = kern_FT(int(nb_quads));
    kern_FT alpha = 1 / (1 + ne/2 + nq/4);
    kern_FT beta = alpha/2;  // edges
    kern_FT gamma = alpha/4; // corners of incident quads
    kern_FT eta = correcting_factor(nb_edges,nb_quads);

    //new position
    pPos[3*index]   = alpha * pVertex->point().x();
    pPos[3*index+1] = alpha * pVertex->point().y();
    pPos[3*index+2] = alpha * pVertex->point().z();

    // rotate around vertex to compute new position
    pHalfEdge = pVertex->vertex_begin();
    end = pHalfEdge;
    CGAL_For_all(pHalfEdge,end)
      {
        const typename Polyhedron::Facet_handle& pFacet = pHalfEdge->facet();
        CGAL_assertion(pFacet != nullptr);
        unsigned int degree = Polyhedron::degree(pFacet);
        CGAL_assertion(degree == 4 || degree == 3);

        // add edge-vertex contribution
        const Point& point = pHalfEdge->prev()->vertex()->point();
        pPos[3*index]   += beta * point.x();
        pPos[3*index+1] += beta * point.y();
        pPos[3*index+2] += beta * point.z();

        // add corner vertex contribution
        if(degree == 4)
          {
            const Point& corner = pHalfEdge->next()->next()->vertex()->point();
            pPos[3*index]   += gamma * corner.x();
            pPos[3*index+1] += gamma * corner.y();
            pPos[3*index+2] += gamma * corner.z();
          }
      }

    // apply correction
    pPos[3*index] = pPos[3*index] + eta*(pPos[3*index]-pVertex->point().x());
    pPos[3*index+1] = pPos[3*index+1] + eta*(pPos[3*index+1]-pVertex->point().y());
    pPos[3*index+2] = pPos[3*index+2] + eta*(pPos[3*index+2]-pVertex->point().z());
  }
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::smooth_set_new_positions(Polyhedron *pMesh,typename kernel::FT *pPos)
  {
    unsigned int index = 0;
    typedef typename kernel::FT kern_FT;
    typename Polyhedron::Vertex_iterator pVertex;
    for(pVertex = pMesh->vertices_begin();
        pVertex != pMesh->vertices_end();
        pVertex++)
      {
        Point& point = pVertex->point();
        const kern_FT x= pPos[3*index];
        const kern_FT y= pPos[3*index+1];
        const kern_FT z= pPos[3*index+2];
/*         const kern_RT wsx = x.numerator() * y.denominator() * z.denominator();  */
/*         const kern_RT wsy = y.numerator() * x.denominator() * z.denominator();  */
/*         const kern_RT wsz = z.numerator() * x.denominator() * y.denominator();  */
/*         const kern_RT wsh  = x.denominator() * y.denominator() * z.denominator();  */
/*         point = Point(wsx,wsy,wsz,wsh); */
        point = Point(x,y,z,1);
        index++;
      }
  }
template <class HDS,class Polyhedron,class kernel>
void CModifierQuadTriangle<HDS,Polyhedron,kernel>::smooth(Polyhedron *pMesh,bool smooth_boundary)
  {
    CGAL_assertion(pMesh != nullptr);

    // alloc position vectors
    unsigned int nb_vertices = pMesh->size_of_vertices();
    typename kernel::FT *pPos = new typename kernel::FT[3*nb_vertices];
    CGAL_assertion(pPos != nullptr);

    // compute new position
    unsigned int index = 0;
    typename Polyhedron::Vertex_iterator pVertex;
    for(pVertex = pMesh->vertices_begin();
        pVertex != pMesh->vertices_end();
        pVertex++)
      {
        // border vertices will not move
        if(Polyhedron::is_border(pVertex))
          smooth_trata_vertice_borde(index,pMesh,pVertex,pPos,smooth_boundary);
        else
          smooth_trata_vertice_interior(index,pMesh,pVertex,pPos);
        index++;
      }
    // asigna new position
    smooth_set_new_positions(pMesh,pPos);
    // cleanup
    delete[] pPos;
  }

#endif // MODIFIER_QUAD_TRIANGLE
