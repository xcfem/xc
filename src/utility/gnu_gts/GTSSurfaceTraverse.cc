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
//GTSSurfaceTraverse.cc

#include "GTSSurfaceTraverse.h"
#include "GTSSurface.h"
#include "GTSFace.h"
#include <iostream>

using namespace std;

void GTSSurfaceTraverse::crea(const GTSSurface &s,const GTSFace &f)
  {
    if(sft) borra();
    sft = gts_surface_traverse_new(s.s,f.fc);
    borrar= true;
  }
void GTSSurfaceTraverse::borra(void)
  {
    if(!borrar) return;
    if(sft) gts_surface_traverse_destroy(sft);
    sft= nullptr;
    borrar= false;
  }
GTSSurfaceTraverse::GTSSurfaceTraverse(const GTSSurface &s,const GTSFace &f)
  : sft(nullptr), borrar(false)
  { crea(s,f); }
GTSSurfaceTraverse::GTSSurfaceTraverse(const GTSSurfaceTraverse &other)
  : sft(other.sft), borrar(false)
  {}
GTSSurfaceTraverse &GTSSurfaceTraverse::operator=(const GTSSurfaceTraverse &other)
  {
    sft= other.sft;
    borrar= false;
    return *this;
  }  
GTSSurfaceTraverse::~GTSSurfaceTraverse(void)
  { borra(); }
GTSFace GTSSurfaceTraverse::Next(guint *level) const
  { return GTSFace(gts_surface_traverse_next(sft,level)); }
