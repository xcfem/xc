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
//GTSSplit.cc

#include "GTSSplit.h"
#include "GTSVertex.h"
#include <iostream>

using namespace std;

void GTSSplit::crea(const GTSVertex &v,const GTSVertex &v1,const GTSVertex &v2)
  {
    if(spt) borra();
    spt = gts_split_new(gts_split_class(),v.vt,GTS_OBJECT(v1.vt),GTS_OBJECT(v2.vt));
    borrar= true;
  }
void GTSSplit::borra(void)
  {
    if(!borrar) return;
    if(spt) gts_object_destroy(GTS_OBJECT(spt));
    spt= nullptr;
    borrar= false;
  }
GTSSplit::GTSSplit(const GTSVertex &v,const GTSVertex &v1,const GTSVertex &v2)
  : spt(nullptr), borrar(false)
  { crea(v,v1,v2); }
GTSSplit::GTSSplit(const GTSSplit &other)
  : spt(other.spt), borrar(false)
  {}
GTSSplit &GTSSplit::operator=(const GTSSplit &other)
  {
    borra();
    spt= other.spt;
    borrar= false;
    return *this;
  }

void GTSSplit::Collapse(void)
//Collapses the vertex split vs. Any new edge created during the process will be of class klass. If heap is not nullptr, the new edges will be inserted into it and the destroyed edges will be removed from it.
  {
    gts_split_collapse(spt,gts_edge_class (),nullptr);
  }

  
GTSSplit::~GTSSplit(void)
  { borra(); }
