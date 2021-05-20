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
//GTSGraph.cc

#include "GTSGraph.h"
#include "GTSSurface.h"
#include <iostream>

using namespace std;

void GTSGraph::crea(const GTSSurface &s)
  {
    if(gph) borra();
    gph = gts_surface_graph_new(gts_graph_class(),s.s);
    borrar= true;
  }
void GTSGraph::borra(void)
  {
    if(!borrar) return;
    if(gph) gts_object_destroy(GTS_OBJECT(gph));
    gph= nullptr;
    borrar= false;
  }
GTSGraph::GTSGraph(const GTSSurface &s)
  : gph(nullptr), borrar(false)
  { crea(s); }
GTSGraph::GTSGraph(const GTSGraph &other)
  : gph(other.gph), borrar(false)
  {}
GTSGraph &GTSGraph::operator=(const GTSGraph &other)
  {
    borra();
    gph= other.gph;
    borrar= false;
    return *this;
  }

void GTSGraph::PrintStats(FILE* fp) const
  { gts_graph_print_stats (gph, fp); }

void GTSGraph::Write(FILE *fp)
  { gts_graph_write(gph, fp); }

GTSGraph::~GTSGraph(void)
  { borra(); }



