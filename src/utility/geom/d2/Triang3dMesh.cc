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
//Triang3dMesh.cc

#include "Triang3dMesh.h"
#include "utility/geom/d2/Triangle3d.h"
#include "Trihedron.h"
#include "utility/gnu_gts/GTSSurface.h"
#include "utility/gnu_gts/GTSEdge.h"
#include "utility/gnu_gts/TriangleMap.h"


template <class HDS>
class Build_polyhedron: public CGAL::Modifier_base<HDS>
  {
    const GTSSurface &sf;
  public:
    Build_polyhedron(const GTSSurface &gts_surf)
      : sf(gts_surf) {}
    void operator()( HDS& hds)
      {
        const size_t num_facetas= sf.GetNumCaras();
        const size_t num_edges= sf.getNumEdges();
        const size_t num_vertices= sf.getNumVertices();
        TriangleMap mt= sf.GetTriangleMap();
        // Postcondition: `hds' is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
        B.begin_surface(num_vertices,num_facetas,num_edges);
        const VerticesMap &vertices= mt.getVertices();
        for(VerticesMap::const_iterator i= vertices.begin();
            i!= vertices.end();i++)
          {
            const Pos3d coo_vertice= (*i).second;
            B.add_vertex(coo_vertice.ToCGAL());        
          }
        const TriangleFaces &triangles= mt.getFaces();
        for(TriangleFaces::const_iterator j= triangles.begin();
            j!=triangles.end();j++)
          {
            B.begin_facet();
            B.add_vertex_to_facet((*j).V1()-1);
            B.add_vertex_to_facet((*j).V2()-1);
            B.add_vertex_to_facet((*j).V3()-1);
            B.end_facet();
          }
        B.end_surface();
      }
  };

Triang3dMesh::Triang3dMesh(const GTSSurface &gts_surf)
  {
    typedef CGPolyhedron_3::HalfedgeDS HalfedgeDS;
    Build_polyhedron<HalfedgeDS> bpoly(gts_surf);
    cgpolyhedron.delegate(bpoly);
    //CGAL_assertion( P.is_triangle( P.halfedges_begin()));
  }

Triang3dMesh GTSSurface2Triang3dMesh(const GTSSurface &gts_surf)
  { return Triang3dMesh(gts_surf); }
GTSSurface Triang3dMesh::get_gts_surface(void) const
  {
     GTSSurface retval;
//     Triang3dMesh tmp= Triangula();
//     typedef PolygonMap<CGTriangMesh_3> polygon_map;
//     polygon_map map= GetPolygonMap();
//     deque<GTSVertex> vertices;
//     for(polygon_map::vertices_const_iterator i= map.VerticesBegin();
//         i!= map.VerticesEnd(); i++)
//       {
//         Pos3d pos= (*i).second;
//         GTSVertex v(pos.x(),pos.y(),pos.z());
//         vertices.push_back(v);
//       }
//     typedef pair<size_t,size_t> verts_arista;
//     typedef set<verts_arista > set_edges;
//     set_edges s_edges;
//     deque<GTSEdge> edges;
//     for(polygon_map::caras_const_iterator i= map.CarasBegin();
//         i!= map.CarasEnd(); i++)
//       {
//         const size_t v1= (*i).Vertice(1);
//         const size_t v2= (*i).Vertice(2);
//         const size_t v3= (*i).Vertice(3);
//         const verts_arista lado1(v1,v2);
//         if(s_edges.find(lado1)== s_edges.end()) //Arista nueva.
//           {
// 	    s_edges.insert(lado1);
//             edges.push_back(GTSEdge(vertices[lado1.first],vertices[lado1.second]));
//           }
//         const verts_arista lado2(v2,v3);
//         if(s_edges.find(lado2)== s_edges.end()) //Arista nueva.
//           {
// 	    s_edges.insert(lado2);
//             edges.push_back(GTSEdge(vertices[lado2.first],vertices[lado2.second]));
//           }
//         const verts_arista lado3(v3,v1);
//         if(s_edges.find(lado3)== s_edges.end()) //Arista nueva.
//           {
// 	    s_edges.insert(lado3);
//             edges.push_back(GTSEdge(vertices[lado3.first],vertices[lado3.second]));
//           }
//         GTSEdge l1= XXX
//       }
    
     std::cerr << "Triang3dMesh::" << __FUNCTION__
               << " not implemented." << std::endl;
    return retval;
  }

Triangle3d Triang3dMesh::getFaceTriangle(const Facet_const_iterator &f) const
  {
    Facet::Halfedge_const_handle h = f->halfedge();
    return Triangle3d(Pos3d(h->vertex()->point()),
                       Pos3d(h->next()->vertex()->point()),
                       Pos3d(h->next()->next()->vertex()->point()));
  }

//! @brief Finds the trihedron formed by org and one of the facets
//! that contains p. 
Triang3dMesh::Facet_const_iterator Triang3dMesh::find_trihedron(const Pos3d &org,const Pos3d &p,const double &tol) const
  {
    const size_t nf= GetNumCaras();
    Facet_const_iterator retval= facets_end();
    if(nf<4) return retval;
    for(Facet_const_iterator i= facets_begin();i!=facets_end();i++)
      {
        Triangle3d tf(getFaceTriangle(i));
        Trihedron trihedron(org,tf.Vertice(1),tf.Vertice(2),tf.Vertice(3));
        if(trihedron.In(p,tol))
          {
            retval= i;
            break;
          }
      }
    return retval;
  }

//! @brief Finds the trihedron formed by org and one of the facets
//! that contains p. 
Triang3dMesh::Facet_const_iterator Triang3dMesh::findTrihedron(const Pos3d &org,const Pos3d &p,const double &tol) const
   { return find_trihedron(org,p,tol); }
