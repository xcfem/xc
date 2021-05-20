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
//GTSEdge.h
//C++ wrapper para la clase GtsEdge de la biblioteca GTS.

#ifndef GTS_EDGE_H
#define GTS_EDGE_H

#include "gts.h"

class GTSVertex;

class GTSEdge
  {
    GtsEdge *edg;

    bool borrar;

    void crea(GTSVertex &v1,GTSVertex &v2);
    void borra(void);

    friend class GTSFace;

  public:

    GTSEdge(GTSVertex &v1,GTSVertex &v2);
    GTSEdge(const GTSEdge &);
    GTSEdge &operator=(const GTSEdge &);

    GTSVertex V1(void) const;
    GTSVertex V2(void) const;

    bool CollapseIsValid(void) const;
    GTSVertex VolumeOptimizedVertex(GtsVolumeOptimizedParams &params) const;

    ~GTSEdge(void);
  };


#endif
