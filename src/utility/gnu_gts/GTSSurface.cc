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
//GTSSurface.cc

#include "GTSSurface.h"
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <iostream>
#include "GTSVertex.h"
#include "GTSFace.h"
#include "GTSSurfaceTraverse.h"
#include "GTSSurfaceIntersection.h"
#include "GTSGraph.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "TriangleMap.h"

using namespace std;


void GTSSurface::crea(void)
  {
    if(s) borra();
    s = gts_surface_new (gts_surface_class (),
	                 gts_face_class (),
	                 gts_edge_class (),
		         gts_vertex_class ());
    borrar= true;
  }
void GTSSurface::borra(void)
  {
    if(!borrar) return;
    if(s) gts_object_destroy(GTS_OBJECT(s));
    s= nullptr;
    borrar= false;
  }

GTSSurface::GTSSurface(GtsSurface *ptr)
  : s(ptr), borrar(false) {}
GTSSurface::GTSSurface(void)
  : s(nullptr), borrar(false)
  { crea(); }
GTSSurface::GTSSurface(const GTSSurface &other)
  : s(nullptr), borrar(false)
  {
    crea();
    gts_surface_copy(s,other.s);
  }
GTSSurface &GTSSurface::operator=(const GTSSurface &other)
  {
    crea();
    gts_surface_copy(s,other.s);
    return *this;
  }  
int GTSSurface::Read(GtsFile *fp)
  {
    if(gts_surface_read (s, fp))
      {
	std::cerr << "GTSSurface::" << __FUNCTION__
		  << "; volume: the file on standard input"
                  << " is not a valid GTS file" << std::endl;
	    std::cerr << "stdin:  " << fp->line << ':' 
		 << fp->pos  << ':' << fp->error;
        return 1; /* failure */
      }
    return 0;
  }
void GTSSurface::Write(FILE *fp)
  { gts_surface_write(s, fp); }
void GTSSurface::WriteOogl(FILE *fp)
  { gts_surface_write_oogl(s, fp); }


GTSSurface::~GTSSurface(void)
  { borra(); }

void GTSSurface::PrintStats(FILE* fp) const
  { gts_surface_print_stats (s, fp); }
bool GTSSurface::IsClosed(void) const
  { return gts_surface_is_closed(s);}
bool GTSSurface::IsOpen(void) const
  { return !IsClosed();}
bool GTSSurface::IsOrientable(void) const
  { return gts_surface_is_orientable(s); }
GTSSurface GTSSurface::selfIntersecting(void) const
  { return GTSSurface(gts_surface_is_self_intersecting(s)); }
bool GTSSurface::Vacia(void) const
  { return (s == nullptr); }
//! @brief Return true if the surface intersects itself.
bool GTSSurface::CheckSelfIntersection(bool verbose) const
  {
    GTSSurface self_intersects= selfIntersecting();
    if(!self_intersects.Vacia())
      {
	std::cerr << "GTSSurface::::" << __FUNCTION__
		  << "; surface intersects itself.." << std::endl;
        if(verbose)
          self_intersects.PrintStats(stderr);
        return true;
      }
    return false;
  }

void GTSSurface::appendFace(GTSFace &f)
  {
    gts_surface_add_face(s,f.get_ptr());
    f.borrar= false; //Al parecer, desde aqui ESTE objeto se encarga
                     //de borrar la cara.
  }
void GTSSurface::removeFace(GTSFace &f)
  { gts_surface_remove_face(s,f.get_ptr()); }

void GTSSurface::Merge(const GTSSurface &other)
//Añade a esta todas las caras de 'other' que no estén ya en esta.
  { gts_surface_merge(s,other.s); }

guint GTSSurface::GetNumVertices(void) const
  { return gts_surface_vertex_number(s); }
guint GTSSurface::getNumEdges(void) const
  { return gts_surface_edge_number(s); }
guint GTSSurface::GetNumCaras(void) const
  { return gts_surface_face_number(s); }

static void pick_first_face (GtsFace * f, GtsFace ** first)
  {
    if (*first == nullptr)
      *first = f;
  }
GTSSurfaceTraverse GTSSurface::Begin(void) const
  {
    GtsFace *first= nullptr;
    gts_surface_foreach_face (s, (GtsFunc) pick_first_face, &first);
    return GTSSurfaceTraverse(*this,GTSFace(first));
  }
GTSGraph GTSSurface::Graph(void) const
  { return GTSGraph(s); }
void GTSSurface::ForEachVertex(GtsFunc func,gpointer data) const
  { gts_surface_foreach_vertex(s,func,data); }
void GTSSurface::ForEachEdge(GtsFunc func,gpointer data) const
  { gts_surface_foreach_edge(s,func,data); }
void GTSSurface::ForEachFace(GtsFunc func,gpointer data) const
  { gts_surface_foreach_face(s,func,data); }

GNode *GTSSurface::BBoxTree(void) const
  { return gts_bb_tree_surface(s); }

double GTSSurface::GaussianCurvature(const GTSVertex &v) const
  { return v.GaussianCurvature(*this); }
Vector3d GTSSurface::MeanCurvatureNormal(const GTSVertex &v) const
  { return v.MeanCurvatureNormal(*this); }
void GTSSurface::PrincipalCurvatures(const GTSVertex &v,double &k1,double &k2) const
  { v.PrincipalCurvatures(*this,k1,k2); }
void GTSSurface::PrincipalDirections(const GTSVertex &v,Vector3d &e1,Vector3d &e2) const
  { v.PrincipalDirections(*this,e1,e2); }


GTSSurface Union(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection,bool verbose)
  {
    GTSSurface s1(a), s2(b);
    GTSSurfaceIntersection si(gts_surface_inter_class(),s1, s2);
    return BoolOp(si,s1,s2,"union",check_self_intersection,verbose);
  }
GTSSurface intersection(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection,bool verbose)
  {
    GTSSurface s1(a), s2(b);
    GTSSurfaceIntersection si(gts_surface_inter_class(),s1, s2);
    return BoolOp(si,s1,s2,"inter",check_self_intersection,verbose);
  }
GTSSurface Diferencia(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection,bool verbose)
  {
    GTSSurface s1(a), s2(b);
    GTSSurfaceIntersection si(gts_surface_inter_class(),s1, s2);
    return BoolOp(si,s1,s2,"diff",check_self_intersection,verbose);
  }

TriangleMap GTSSurface::GetTriangleMap(void) const
  { return getTriangleMap(*this); }

