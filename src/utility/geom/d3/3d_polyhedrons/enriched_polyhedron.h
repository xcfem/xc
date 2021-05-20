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
///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Class: Enriched_polyhedron                                           //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#ifndef _POLYGON_MESH_
#define _POLYGON_MESH_

// CGAL stuff
#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <list>
#include "utility/geom/cgal_types.h"
#include "PolygonMap.h"
#include <cassert>
#include <GL/gl.h>
#include <GL/glu.h>
#include <fstream>

// a refined facet with a normal and a tag
template <class Refs, class T, class P, class Norm>
class Enriched_facet : public CGAL::HalfedgeDS_face_base<Refs, T>
  {
    // tag
    int m_tag; 

    // normal
    Norm m_normal;
  public:
    // life cycle
    // no constructors to repeat, since only
    // default constructor mandatory

    Enriched_facet(void) {}

    // tag
    const int&  tag(void) { return m_tag; }
    void tag(const int& t)  { m_tag = t; }

    // normal
    typedef Norm Normal_3; //glVertex3f
    Normal_3& normal(void) { return m_normal; }
    const Normal_3& normal(void) const { return m_normal; }
  };

// a refined halfedge with a general tag and 
// a binary tag to indicate wether it belongs 
// to the control mesh or not
template <class Refs, class Tprev, class Tvertex, class Tface, class Norm>
class Enriched_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs,Tprev,Tvertex,Tface>
  {
  private:

    // tag
    int m_tag; 
    // option for edge superimposing
    bool m_control_edge; 

  public:
    // life cycle
    Enriched_halfedge(void)
      { m_control_edge = true; }

    // tag
    const int& tag(void) const { return m_tag;  }
    int& tag(void) { return m_tag;  }
    void tag(const int& t)  { m_tag = t; }

    // control edge 
    bool& control_edge(void)  { return m_control_edge; }
    const bool& control_edge(void)  const { return m_control_edge; }
    void control_edge(const bool& flag) { m_control_edge  = flag; }
  };

// a refined vertex with a normal and a tag
template <class Refs, class T, class P, class Norm>
class Enriched_vertex : public CGAL::HalfedgeDS_vertex_base<Refs, T, P>
  {
    // tag
    int m_tag; 

    // normal
    Norm m_normal;

  public:
    // life cycle
    Enriched_vertex(void){}
    // repeat mandatory constructors
    Enriched_vertex(const P& pt)
      :CGAL::HalfedgeDS_vertex_base<Refs, T, P>(pt)
      {}

    // normal
    typedef Norm Normal_3;
    Normal_3& normal(void) { return m_normal; }
    const Normal_3& normal(void) const { return m_normal; }

    // tag
    int& tag(void) {  return m_tag; }
    const int& tag(void) const {  return m_tag; }
    void tag(const int& t)  { m_tag = t; }
  };

// A redefined items class for the Polyhedron_3 
// with a refined vertex class that contains a 
// member for the normal vector and a refined
// facet with a normal vector instead of the 
// plane equation (this is an alternative 
// solution instead of using 
// Polyhedron_traits_with_normals_3).

struct Enriched_items : public CGAL::Polyhedron_items_3
  {
    // wrap vertex
    template <class Refs, class Traits>
    struct Vertex_wrapper
      {
        typedef typename Traits::Point_3  Point;
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_vertex<Refs,
                          CGAL::Tag_true,
                          Point,
                          Normal> Vertex;
      };

    // wrap face
    template <class Refs, class Traits>
    struct Face_wrapper
      {
        typedef typename Traits::Point_3  Point;
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_facet<Refs,
                         CGAL::Tag_true,
                         Point,
                         Normal> Face;
      };

    // wrap halfedge
    template <class Refs, class Traits>
    struct Halfedge_wrapper
      {
        typedef typename Traits::Vector_3 Normal;
        typedef Enriched_halfedge<Refs,
                            CGAL::Tag_true,
                            CGAL::Tag_true,
                            CGAL::Tag_true,
                            Normal> Halfedge;
      };
  };

//*********************************************************
template <class kernel, class items>
class Enriched_polyhedron : public CGAL::Polyhedron_3<kernel,items>
  {
  public :
    typedef typename kernel::FT FT;
    typedef typename kernel::Point_3 Point;
    typedef typename kernel::Vector_3 Vector;
    typedef typename kernel::Iso_cuboid_3 Iso_cuboid;
    //Empieza modificación LCPT
    typedef typename CGAL::Polyhedron_3<kernel,items>::Point_iterator Point_iterator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Vertex_iterator Vertex_iterator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Facet_iterator Facet_iterator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Edge_iterator Edge_iterator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Halfedge_iterator Halfedge_iterator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Halfedge_around_facet_circulator Halfedge_around_facet_circulator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Vertex_handle Vertex_handle;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Facet_handle Facet_handle;
    typedef typename CGAL::Polyhedron_3<kernel,items>::Halfedge_handle Halfedge_handle;
    //Termina modificación LCPT

  private :
    // bounding box
    Iso_cuboid m_bbox;
  
    // type
    bool m_pure_quad;
    bool m_pure_triangle;
  
  public :
    // life cycle
    Enriched_polyhedron(void)
      : CGAL::Polyhedron_3<kernel,items>()
      {
        m_pure_quad = false;
        m_pure_triangle = false;
      }
    virtual ~Enriched_polyhedron(void) {}
  
    // type
    bool is_pure_triangle(void) { return m_pure_triangle; }
    bool is_pure_quad(void) { return m_pure_quad; }
  
    // normals (per facet, then per vertex)
    void compute_normals_per_facet(void)
      { std::for_each(this->facets_begin(),this->facets_end(),this->Facet_normal()); }
    void compute_normals_per_vertex(void)
      { std::for_each(this->vertices_begin(),this->vertices_end(),this->Vertex_normal()); }
    void compute_normals(void)
      {
        compute_normals_per_facet();
        compute_normals_per_vertex();
      }
  
    // bounding box
    Iso_cuboid& bbox(void) { return m_bbox; }
    const Iso_cuboid bbox(void) const { return m_bbox; }
  
    // compute bounding box
    void compute_bounding_box(void);
  
    // bounding box
    FT xmin(void) { return m_bbox.xmin(); }
    FT xmax(void) { return m_bbox.xmax(); }
    FT ymin(void) { return m_bbox.ymin(); }
    FT ymax(void) { return m_bbox.ymax(); }
    FT zmin(void) { return m_bbox.zmin(); }
    FT zmax(void) { return m_bbox.zmax(); }
  
    // copy bounding box
    void copy_bounding_box(Enriched_polyhedron<kernel,items> *pMesh)
      { m_bbox = pMesh->bbox(); }
  
    // degree of a face
    static unsigned int degree(Facet_handle pFace)
      { return CGAL::circulator_size(pFace->facet_begin()); }
  
    // valence of a vertex
    static unsigned int valence(Vertex_handle pVertex)
      { return CGAL::circulator_size(pVertex->vertex_begin()); }
  
    // check wether a vertex is on a boundary or not
    static bool is_border(Vertex_handle pVertex);
  
    // get any border halfedge attached to a vertex
    Halfedge_handle get_border_halfedge(Vertex_handle pVertex);
  
    // tag all halfedges
    void tag_halfedges(const int tag);
    // tag all facets
    void tag_facets(const int tag);

    // set index for all vertices
    void set_index_vertices(void);
  
    // is pure degree ?
    bool is_pure_degree(unsigned int d);
  
    // compute type
    void compute_type(void)
      {
        m_pure_quad = is_pure_degree(4);
        m_pure_triangle = is_pure_degree(3);
      }
  
    // compute facet center
    void compute_facet_center(Facet_handle pFace,Point &center);
  
    // compute average edge length around a vertex
    FT average_edge_length_around(Vertex_handle pVertex);
  
    // draw using OpenGL commands (display lists)
    void gl_draw(bool smooth_shading,bool use_normals);
  
    void gl_draw_facet(Facet_handle pFacet,bool smooth_shading,bool use_normals);
  
    // superimpose edges
    void superimpose_edges(bool skip_ordinary_edges = true,bool skip_control_edges = false,bool voronoi_edge = false);
    // superimpose vertices
    void superimpose_vertices(void);
    // superimpose vertices
    void superimpose_spheres(GEOM_FT scale);
  
    // write in obj file format (OBJ).
    void write_obj(char *pFilename,int incr  = 1); // 1-based by default
  
    // draw bounding box
    void gl_draw_bounding_box(void);
  
    // count #boundaries
    unsigned int nb_boundaries(void);
  
    // tag component 
    void tag_component(Facet_handle pSeedFacet,const int tag_free, const int tag_done);
  
    // count #components
    unsigned int nb_components(void);
  
    // compute the genus
    // V - E + F + B = 2 (C - G)
    // C -> #connected components
    // G : genus
    // B : #boundaries
    int genus(void);
    int genus(int c,int v,int f,int e,int b);
  };

template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::compute_bounding_box(void)
  {
    if(this->size_of_vertices() == 0)
      {
        assert(false);
        return;
      }
  
    FT xmin,xmax,ymin,ymax,zmin,zmax;
    Vertex_iterator pVertex = this->vertices_begin();
    xmin = xmax = pVertex->point().x();
    ymin = ymax = pVertex->point().y();
    zmin = zmax = pVertex->point().z();
    for(;pVertex !=  this->vertices_end();pVertex++)
      {
        const Point& p = pVertex->point();
  
        xmin =  std::min(xmin,p.x());
        ymin =  std::min(ymin,p.y());
        zmin =  std::min(zmin,p.z());
  
        xmax =  std::max(xmax,p.x());
        ymax =  std::max(ymax,p.y());
        zmax =  std::max(zmax,p.z());
      }
    m_bbox= Iso_cuboid(xmin,ymin,zmin,xmax,ymax,zmax);
  }

//check wether a vertex is on a boundary or not
template <class kernel, class items>
bool Enriched_polyhedron<kernel,items>::is_border(Vertex_handle pVertex)
  {
    Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
    if(pHalfEdge == nullptr) // isolated vertex
      return true;
    Halfedge_around_vertex_circulator d = pHalfEdge;
    CGAL_For_all(pHalfEdge,d)
      if(pHalfEdge->is_border())
        return true;
    return false;
  }

// get any border halfedge attached to a vertex
template <class kernel, class items>
typename Enriched_polyhedron<kernel,items>::Halfedge_handle Enriched_polyhedron<kernel,items>::get_border_halfedge(Vertex_handle pVertex)
  {
    Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
    Halfedge_around_vertex_circulator d = pHalfEdge;
    CGAL_For_all(pHalfEdge,d)
      if(pHalfEdge->is_border())
        return pHalfEdge;
    return nullptr;
  }

// tag all halfedges
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::tag_halfedges(const int tag)
  {
    for(Halfedge_iterator pHalfedge = this->halfedges_begin();
        pHalfedge != this->halfedges_end();
        pHalfedge++)
      pHalfedge->tag(tag);
  }
  
// tag all facets
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::tag_facets(const int tag)
  {
    for(Facet_iterator pFacet = this->facets_begin();
        pFacet  != this->facets_end();
        pFacet++)
      pFacet->tag(tag);
  }

// set index for all vertices
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::set_index_vertices()
  {
    int index = 0;
    for(Vertex_iterator pVertex = this->vertices_begin();
        pVertex != this->vertices_end();
        pVertex++)
      pVertex->tag(index++);
  }

// is pure degree ?
template <class kernel, class items>
bool Enriched_polyhedron<kernel,items>::is_pure_degree(unsigned int d)
  {
    for(Facet_iterator pFace  = this->facets_begin();
        pFace != this->facets_end();
        pFace++)
      if(degree(pFace) != d)
        return false;
    return true;
  }

// compute facet center
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::compute_facet_center(Facet_handle pFace,Point &center)
  {
    Halfedge_around_facet_circulator pHalfEdge = pFace->facet_begin();
    Halfedge_around_facet_circulator end = pHalfEdge;
    Vector vec(0.0,0.0,0.0);
    int degree = 0;
    CGAL_For_all(pHalfEdge,end)
      {
        vec = vec + (pHalfEdge->vertex()->point()-CGAL::ORIGIN);
        degree++;
      }
    center = CGAL::ORIGIN + (vec/(typename kernel::FT)degree);
  }
  
// compute average edge length around a vertex
template <class kernel, class items>
typename Enriched_polyhedron<kernel,items>::FT Enriched_polyhedron<kernel,items>::average_edge_length_around(Vertex_handle pVertex)
  {
    FT sum = 0.0;
    Halfedge_around_vertex_circulator pHalfEdge = pVertex->vertex_begin();
    Halfedge_around_vertex_circulator end = pHalfEdge;
    Vector vec(0.0,0.0,0.0);
    int degree = 0;
    CGAL_For_all(pHalfEdge,end)
      {
        Vector vec = pHalfEdge->vertex()->point()-
                     pHalfEdge->opposite()->vertex()->point();
        sum += sqrt_FT(vec*vec);
        degree++;
      }
    return sum / (FT) degree;
  }

// draw using OpenGL commands (display lists)
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::gl_draw(bool smooth_shading,bool use_normals)
  {
    // draw polygons
    Facet_iterator pFacet= this->facets_begin();
    for(;pFacet != this->facets_end();pFacet++)
      {
        // begin polygon assembly
        glBegin(GL_POLYGON);
          gl_draw_facet(pFacet,smooth_shading,use_normals);
        glEnd(); // end polygon assembly
      }
    glFlush();
  }
  
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::gl_draw_facet(Facet_handle pFacet,bool smooth_shading,bool use_normals)
  {
    // one normal per face
    if(use_normals && !smooth_shading)
      {
        const typename Enriched_polyhedron<kernel,items>::Facet::Normal_3 &normal= pFacet->normal();
        ::glNormal3f(this->normal[0],this->normal[1],this->normal[2]);
      }
  
    // revolve around current face to get vertices
    Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
    do
      {
        // one normal per vertex
        if(use_normals && smooth_shading)
          {
            const typename Enriched_polyhedron<kernel,items>::Facet::Normal_3& normal = pHalfedge->vertex()->normal();
            ::glNormal3f(normal[0],normal[1],normal[2]);
          }
  
        // polygon assembly is performed per vertex
        const Point& point  = pHalfedge->vertex()->point();
        ::glVertex3d(point[0],point[1],point[2]);
      }
    while(++pHalfedge != pFacet->facet_begin());
  }

// superimpose edges
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::superimpose_edges(bool skip_ordinary_edges,bool skip_control_edges, bool voronoi_edge)
  {
    glBegin(GL_LINES);
    for(Edge_iterator h = this->edges_begin();h != this->edges_end(); h++)
      {
        // ignore this edges
        if(skip_ordinary_edges && !h->control_edge()) continue;
  
        // ignore control edges
        if(skip_control_edges && h->control_edge()) continue;
  
        if(voronoi_edge)
          {
            Facet_handle pFace1 = h->facet();
            Facet_handle pFace2 = h->opposite()->facet();
            if(pFace1 == nullptr || pFace2 == nullptr) continue;
  
            const Point &p1 = h->vertex()->point();
            const Point &p2 = h->next()->vertex()->point();
            const Point &p3 = h->next()->next()->vertex()->point();
  
            kernel k;
            Point d1 = k.construct_circumcenter_3_object()(p1,p2,p3);
            glVertex3f(d1[0],d1[1],d1[2]);
  
            const Point &pp1 = h->opposite()->vertex()->point();
            const Point &pp2 = h->opposite()->next()->vertex()->point();
            const Point &pp3 = h->opposite()->next()->next()->vertex()->point();
            Point d2 = k.construct_circumcenter_3_object()(pp1,pp2,pp3);
            glVertex3f(d2[0],d2[1],d2[2]);
          }
        else
          {
            // assembly and draw line segment
            const Point& p1 = h->prev()->vertex()->point();
            const Point& p2 = h->vertex()->point();
            glVertex3f(p1[0],p1[1],p1[2]);
            glVertex3f(p2[0],p2[1],p2[2]);
          }
      }
    ::glEnd();
  }
  
// superimpose vertices
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::superimpose_vertices(void)
  {
    glBegin(GL_POINTS);
    for(Point_iterator pPoint = this->points_begin();pPoint != this->points_end();pPoint++)
      glVertex3f(pPoint->x(),pPoint->y(),pPoint->z());
    ::glEnd(); // // end point assembly
  }
  
// superimpose vertices
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::superimpose_spheres(GEOM_FT scale)
  {
    GLUquadricObj* pQuadric = gluNewQuadric();
    ::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    for(Vertex_iterator pVertex = this->vertices_begin();pVertex !=  this->vertices_end(); pVertex++)
      {
        glPushMatrix();
        GEOM_FT radius = average_edge_length_around(pVertex);
        glTranslatef(pVertex->point().x(),
                       pVertex->point().y(),
                       pVertex->point().z());
        gluSphere(pQuadric,scale*radius,24,24); 
        glPopMatrix();
      }
    gluDeleteQuadric(pQuadric);
  }

// write in obj file format (OBJ).
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::write_obj(char *pFilename,int incr)
  {
    std::ofstream stream(pFilename);
  
    // output vertices
    for(Point_iterator pPoint = this->points_begin();pPoint != this->points_end();pPoint++) 
      stream << 'v' << ' ' << pPoint->x() << ' ' <<
                              pPoint->y() << ' ' <<
                              pPoint->z() << std::endl;
  
    // precompute vertex indices
    this->set_index_vertices(); 
  
    // output facets
    for(Facet_iterator pFacet = this->facets_begin();pFacet != this->facets_end(); pFacet++) 
      {
        stream << 'f';
        Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
        do 
          stream << ' ' << pHalfedge->vertex()->tag()+incr;
        while(++pHalfedge != pFacet->facet_begin());
        stream << std::endl;
      }
  }
  
    // draw bounding box
template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::gl_draw_bounding_box(void)
  {
    glBegin(GL_LINES);
  
    // along x axis
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());
  
    // along y axis
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());
  
    // along z axis
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmin(),m_bbox.ymax(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymin(),m_bbox.zmax());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmin());
    glVertex3f(m_bbox.xmax(),m_bbox.ymax(),m_bbox.zmax());

    ::glEnd();
  }

// count #boundaries
template <class kernel, class items>
unsigned int Enriched_polyhedron<kernel,items>::nb_boundaries(void)
  {
    unsigned int nb = 0;
    tag_halfedges(0);
    for(Halfedge_iterator he = this->halfedges_begin();
        he != this->halfedges_end();
        he++)
      {
        if(he->is_border() && he->tag() == 0)
          {
            nb++;
            Halfedge_handle curr = he;
            do
              {
                curr  = curr->next();
                curr->tag(1);
              }
            while(curr != he);
          }
      }
    return nb;
  }

template <class kernel, class items>
void Enriched_polyhedron<kernel,items>::tag_component(Facet_handle pSeedFacet,
                                                      const int tag_free,
                                                      const int tag_done)
  {
    pSeedFacet->tag(tag_done);
    std::list<Facet_handle> facets;
    facets.push_front(pSeedFacet);
    while(!facets.empty())
      {
        Facet_handle pFacet = facets.front();
        facets.pop_front();
        pFacet->tag(tag_done);
        Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
        Halfedge_around_facet_circulator end = pHalfedge;
        CGAL_For_all(pHalfedge,end)
          {
            Facet_handle pNFacet = pHalfedge->opposite()->facet();
            if(pNFacet != nullptr && pNFacet->tag() == tag_free)
              {
                facets.push_front(pNFacet);
                pNFacet->tag(tag_done);
              }
          }
      }
  }

// count #components
template <class kernel, class items>
unsigned int Enriched_polyhedron<kernel,items>::nb_components()
  {
    unsigned int nb = 0;
    tag_facets(0);
    for(Facet_iterator pFacet = this->facets_begin(); pFacet != this->facets_end(); pFacet++)
      {
        if(pFacet->tag() == 0)
          {
            nb++;
            tag_component(pFacet,0,1);
          }
      }
    return nb;
  }  
    // compute the genus
    // V - E + F + B = 2 (C - G)
    // C -> #connected components
    // G : genus
    // B : #boundaries
template <class kernel, class items>
int Enriched_polyhedron<kernel,items>::genus()
  {
    const int c = this->nb_components();
    const int b = this->nb_boundaries();
    const int v = this->size_of_vertices();
    const int e = this->size_of_halfedges()/2;
    const int f = this->size_of_facets();
    return genus(c,v,f,e,b);
  }
template <class kernel, class items>
int Enriched_polyhedron<kernel,items>::genus(int c,int v,int f,int e,int b)
  { return (2*c+e-b-f-v)/2; }
  
// compute facet normal 
struct Facet_normal // (functor)
  {
    template <class Facet>
    void operator()(Facet& f);
  };

template <class Facet>
void Facet_normal::operator()(Facet& f)
  {
    typename Facet::Normal_3 sum = CGAL::NULL_VECTOR;
    typename Facet::Halfedge_around_facet_circulator h = f.facet_begin();
    do
      {
        typename Facet::Normal_3 normal = CGAL::cross_product(
          h->next()->vertex()->point() - h->vertex()->point(),
          h->next()->next()->vertex()->point() - h->next()->vertex()->point());
        GEOM_FT sqnorm = normal * normal;
        if(sqnorm != 0) normal = normal / sqrt_FT(sqnorm);
        sum = sum + normal;
      }
    while(++h != f.facet_begin());
    float sqnorm = sum * sum;
    if(sqnorm != 0.0)
      f.normal() = sum / sqrt_FT(sqnorm);
    else
      {
        f.normal() = CGAL::NULL_VECTOR;
	std::cerr << "degenerate face\n";
      }
  }
  
  
// compute vertex normal 
struct Vertex_normal // (functor)
  {
    template <class Vertex>
    void operator()(Vertex& v);
  };

template <class Vertex>
void Vertex_normal::operator()(Vertex& v)
  {
    typename Vertex::Normal_3 normal = CGAL::NULL_VECTOR;
    typename Vertex::Halfedge_around_vertex_const_circulator pHalfedge = v.vertex_begin();
    typename Vertex::Halfedge_around_vertex_const_circulator begin = pHalfedge;
    CGAL_For_all(pHalfedge,begin) 
      if(!pHalfedge->is_border()) normal = normal + pHalfedge->facet()->normal();
      float sqnorm = normal * normal;
      if(sqnorm != 0.0f)
        v.normal() = normal / sqrt_FT(sqnorm);
      else
        v.normal() = CGAL::NULL_VECTOR;
  }

typedef Enriched_polyhedron<GEOMKernel,Enriched_items> EPolyhedron;

EPolyhedron enriquece(const CGPolyhedron_3 &pol);
CGPolyhedron_3 empobrece(const EPolyhedron &pol);

#endif // _POLYGON_MESH_
