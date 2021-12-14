//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GTSEdge.cc

#include "GTSEdge.h"
#include "GTSVertex.h"
#include <iostream>

using namespace std;

void GTSEdge::crea(GTSVertex &v1,GTSVertex &v2)
  {
    if(edg) borra();
    edg = gts_edge_new(gts_edge_class(),v1.vt,v2.vt);
    v1.borrar= false; //Al parecer, desde aqui ESTE objeto se encarga
    v2.borrar= false; //de borrar los vértices.
    borrar= true;
  }
void GTSEdge::borra(void)
  {
    if(!borrar) return;
    if(edg) gts_object_destroy(GTS_OBJECT(edg));
    edg= nullptr;
    borrar= false;
  }
GTSEdge::GTSEdge(GTSVertex &v1,GTSVertex &v2)
  : edg(nullptr), borrar(false)
{ crea(v1,v2); }
GTSEdge::GTSEdge(const GTSEdge &other)
  : edg(other.edg), borrar(false)
  {}
GTSEdge &GTSEdge::operator=(const GTSEdge &other)
  {
    borra();
    edg= other.edg;
    borrar= false;
    return *this;
  }
GTSEdge::~GTSEdge(void)
  { borra(); }

GTSVertex GTSEdge::V1(void) const
  { return GTSVertex(GTS_SEGMENT(edg)->v1); }
GTSVertex GTSEdge::V2(void) const
  { return GTSVertex(GTS_SEGMENT(edg)->v2); }


bool GTSEdge::CollapseIsValid(void) const
//TRUE if e can be collapsed without violation of the topological constraints, FALSE otherwise.
  { return gts_edge_collapse_is_valid(edg); }
GTSVertex GTSEdge::VolumeOptimizedVertex(GtsVolumeOptimizedParams &params) const
//TRUE if e can be collapsed without violation of the topological constraints, FALSE otherwise.
  { return GTSVertex(gts_volume_optimized_vertex(edg, gts_vertex_class (), &params)); }
