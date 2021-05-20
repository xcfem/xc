//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Polyhedron3d.cc

#include "Polyhedron3d.h"
#include "../../d2/Triang3dMesh.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d2/Polygon3d.h"
#include "utility/geom/d3/HalfSpace3d.h"
#include "utility/geom/d1/Segment3d.h"
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/IO/Polyhedron_geomview_ostream.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include "quad-triangle.h"
#include "polyhedron3d_bool_op.h"
#include "VertexMap.h"
#include <set>
#include "utility/geom/pos_vec/Pos3dList.h"





//! @brief Default constructor.
Polyhedron3d::Polyhedron3d(void): cgpolyhedron() {}

//! @brief Constructor.
Polyhedron3d::Polyhedron3d(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  : cgpolyhedron()
  { make_tetrahedron(p0,p1,p2,p3); }

//! @brief Constructor.
Polyhedron3d::Polyhedron3d(const std::deque<HalfSpace3d> &se)
  : cgpolyhedron()
  { make_polyhedron(se); }


//! @brief Constructor.
Polyhedron3d::Polyhedron3d(const HalfSpace3d &se1, const HalfSpace3d &se2,const HalfSpace3d &se3, const HalfSpace3d &se4)
  { make_tetrahedron(se1,se2,se3,se4); }

//! @brief Constructor.
Polyhedron3d::Polyhedron3d(const EPolyhedron &e)
  : cgpolyhedron(empobrece(e)) {}

//! @brief Copy constructor
Polyhedron3d::Polyhedron3d(const Polyhedron3d &other) 
  : PolyhedronBase(other), cgpolyhedron(other.cgpolyhedron) {}

//! @brief Assignment operator.
Polyhedron3d &Polyhedron3d::operator=(const Polyhedron3d &other)
  {
    PolyhedronBase::operator=(other);
    cgpolyhedron= other.cgpolyhedron;
    return *this;
  }

//! @brief Constructor virtual.
GeomObj *Polyhedron3d::clon(void) const
  { return new Polyhedron3d(*this); }

void Polyhedron3d::make_tetrahedron(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  { cgpolyhedron.make_tetrahedron(p0.ToCGAL(),p1.ToCGAL(),p2.ToCGAL(),p3.ToCGAL()); }

void Polyhedron3d::make_polyhedron(const std::deque<HalfSpace3d> &se)
  {
    TripletMap<Pos3d> vertices= polyhedron_vertices(se);
    BuildFromVertexMap build(vertices);
    cgpolyhedron.delegate(build);
  }

void Polyhedron3d::make_tetrahedron(const HalfSpace3d &se1, const HalfSpace3d &se2,const HalfSpace3d &se3, const HalfSpace3d &se4)
  {
    std::deque<HalfSpace3d> se;
    se.push_back(se1);
    se.push_back(se2);
    se.push_back(se3);
    se.push_back(se4);
    make_polyhedron(se);
  }

// //! @brief Interprets the half spacess that define the polyhedron.
// //! @param cle: if true it deletes the previously defined vertices,
// //! faces and edges.
// void Polyhedron3d::halfSpaces(const std::string &str,const bool &cle)
//   {
//     const std::deque<HalfSpace3d> se= interpretaHalfSpaces3d(str);
//     const size_t sz= se.size();
//     if(sz>=4)
//       {
//         if(cle)
//           clear();
//         make_polyhedron(se);
//       }
//     else
//       std::cerr << getClassName() << "::" << __FUNCTION__
//                 << "; four half spaces expected." << std::endl;
//   }

//! @brief Return the sum of the areas of the faces.
GEOM_FT Polyhedron3d::getArea(void) const
  {
    GEOM_FT retval= 0.0;
    for(Polyhedron3d::Facet_const_iterator i= facets_begin();i!=facets_end();i++)
      retval+= GetCara(i).getArea();
    return retval;
  }

GEOM_FT Polyhedron3d::GetMax(unsigned short int i) const
  {
    CGPolyhedron_3::Vertex_const_iterator vi= cgpolyhedron.vertices_begin();
    GEOM_FT retval= vi->point().cartesian(i-1);
    for(;vi!=cgpolyhedron.vertices_end();vi++)
      retval= std::max(retval,vi->point().cartesian(i-1));
    return retval;
  }
GEOM_FT Polyhedron3d::GetMin(unsigned short int i) const
  {
    CGPolyhedron_3::Vertex_const_iterator vi= cgpolyhedron.vertices_begin();
    GEOM_FT retval= vi->point().cartesian(i-1);
    for(;vi!=cgpolyhedron.vertices_end();vi++)
      retval= std::min(retval,vi->point().cartesian(i-1));
    return retval;
  }

GeomObj::list_Pos3d Polyhedron3d::GetVerticesCara(const Facet_const_iterator &f) const
  {
    Facet::Halfedge_const_handle h = f->halfedge();
    GeomObj::list_Pos3d retval;
    retval.push_back(Pos3d(h->vertex()->point()));
    retval.push_back(Pos3d(h->next()->vertex()->point()));
    retval.push_back(Pos3d(h->next()->next()->vertex()->point()));
    return retval;
  }

GeomObj::list_Pos3d Polyhedron3d::getVertices(void) const
  {
    GeomObj::list_Pos3d retval;
    for(CGPolyhedron_3::Vertex_const_iterator vi= cgpolyhedron.vertices_begin();vi!=cgpolyhedron.vertices_end();vi++)
      retval.push_back(Pos3d(vi->point()));
    return retval;
  }

Plane Polyhedron3d::getPlaneFromFace(const Facet_const_iterator &f) const
  {
    Facet::Halfedge_const_handle h= f->halfedge();
    Plane retval(Pos3d(h->vertex()->point()),Pos3d(h->next()->vertex()->point()),Pos3d(h->next()->next()->vertex()->point()));
    return retval;
  }

//! @brief Return la cara correspondiente al iterador.
Polygon3d Polyhedron3d::GetCara(const Facet_const_iterator &f) const
  {
    const GeomObj::list_Pos3d vertices= getVertices();
    Polygon3d retval(vertices.begin(),vertices.end());
    return retval;
  }

//! @brief Return the faces of the polyhedron.
std::deque<Polygon3d> Polyhedron3d::GetCaras(void) const
  {
    std::deque<Polygon3d> retval;
    for(Polyhedron3d::Facet_const_iterator i= facets_begin();i!=facets_end();i++)
      retval.push_back(GetCara(i));
    return retval;
  }

//! @brief Print stuff.
void Polyhedron3d::Print(std::ostream &os) const
  {
    CGAL::set_ascii_mode(os);
    CGAL::set_pretty_mode(os);
    os << cgpolyhedron << std::endl;
  }

void Polyhedron3d::Print(CGview_stream &v) const
  {
    v << cgpolyhedron;
  }

//! @brief Read the polyhedrom from an Object File Format file.
void Polyhedron3d::ReadOFF(std::istream &is)
  { is >> cgpolyhedron; }

CGview_stream &operator<<(CGview_stream &v,const Polyhedron3d &p)
  {
    p.Print(v);
    return v;
  }

EPolyhedron Polyhedron3d::GetEnriquecido(void) const
  { return enriquece(cgpolyhedron); }


Polyhedron3d subdivide_quad_triangle(const Polyhedron3d &pol,bool smooth_boundary)
  {
    CSubdivider_quad_triangle<EPolyhedron,GEOMKernel> subdivider;
    EPolyhedron retval, tmp(pol.GetEnriquecido());
    subdivider.subdivide(tmp,retval);
    return Polyhedron3d(retval);
  }

Triang3dMesh Polyhedron3d::Triangula(void) const
  {
    EPolyhedron retval(GetEnriquecido());
    EPolyhedron::Facet_iterator pFacet;
    for(pFacet = retval.facets_begin();
        pFacet != retval.facets_end();
        pFacet++)
      {
        const unsigned int degree = EPolyhedron::degree(pFacet);
        CGAL_assertion(degree >= 3);

        const EPolyhedron::Halfedge_handle pHalfedge1 = pFacet->halfedge();
        if(degree==4)
          {
            const EPolyhedron::Halfedge_handle pHalfedge2 = pHalfedge1->next()->next();
            retval.split_facet(pHalfedge1,pHalfedge2);
          }
        else
          if(degree>4)
            {
              if(pHalfedge1->is_border())
                std::cerr << "Se intentó subdividir una cara hueca." << std::endl;
              else
                retval.create_center_vertex(pHalfedge1);
            }

      }
    return Triang3dMesh(Polyhedron3d(retval));
  }

//! @brief Return the SIGNED distance from the point to the tetrahedron.
//! The distance is computed as the maximum of the distances from the point
//! to each of the planes that limit the tetrahedron.
GEOM_FT Polyhedron3d::PseudoDist(const Pos3d &p) const
  {
    const size_t nv= GetNumVertices();
    assert(nv>0);
    Polyhedron3d::Vertex_const_iterator pVertex0= vertices_begin();
    if(nv==1) return p.dist(Pos3d(pVertex0->point()));
    Polyhedron3d::Vertex_const_iterator pVertex1= pVertex0; pVertex1++;
    if(nv==2) return p.dist(Segment3d(Pos3d(pVertex0->point()),Pos3d(pVertex1->point())));
    Polyhedron3d::Facet_const_iterator i= facets_begin();
    const Plane pl(getPlaneFromFace(i));
    GEOM_FT retval= pl.PseudoDist(p);
    for(;i!=facets_end();i++)
      {
        const Plane plane(getPlaneFromFace(i));
        const GEOM_FT d1= plane.PseudoDist(p);
        retval= std::max(retval,d1);
      }
    return retval;
  }

//! @brief Return verdadero si alguno de los vertices toca el cuadrante
//! que se pasa como parámetro.
bool Polyhedron3d::TocaCuadrante(const int &cuadrante) const
  {
    for(CGPolyhedron_3::Vertex_const_iterator vi= cgpolyhedron.vertices_begin();vi!=cgpolyhedron.vertices_end();vi++)
      if(Pos3d((*vi).point()).Cuadrante() == cuadrante)
        return true;
    return false;
  }

