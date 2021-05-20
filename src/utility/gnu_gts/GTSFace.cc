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
//GTSFace.cc

#include "GTSFace.h"
#include "GTSEdge.h"

void GTSFace::crea(GTSEdge &e1,GTSEdge &e2,GTSEdge &e3)
  {
    if(fc) borra();
    fc = gts_face_new(gts_face_class(),e1.edg,e2.edg,e3.edg);
    e1.borrar= false; //Al parecer, desde aqui ESTE objeto se encarga
    e2.borrar= false; //de borrar los edges.
    e3.borrar= false;
    borrar= true;
  }
void GTSFace::borra(void)
  {
    if(!borrar) return;
    if(fc) gts_object_destroy(GTS_OBJECT(fc));
    fc= nullptr;
    borrar= false;
  }
GTSFace::GTSFace(GtsFace *f)
  :fc(f), borrar(false) {}
GTSFace::GTSFace(GTSEdge &e1,GTSEdge &e2,GTSEdge &e3)
  : fc(nullptr), borrar(false)
  { crea(e1,e2,e3); }
GTSFace::GTSFace(const GTSFace &other)
  :fc(other.fc), borrar(false) {}
GTSFace &GTSFace::operator=(const GTSFace &other)
  {
    borra();
    fc= other.fc;
    borrar= false;
    return *this;
  }
bool GTSFace::Null(void) const
  { return (fc==nullptr); }

GTSFace::~GTSFace(void)
  { borra(); }
