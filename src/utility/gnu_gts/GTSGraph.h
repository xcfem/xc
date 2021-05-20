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
//GTSGraph.h
//C++ wrapper para la clase GtsGraph de la biblioteca GTS.

#ifndef GTS_GRAPH_H
#define GTS_GRAPH_H

#include "gts.h"

class GTSSurface;

class GTSGraph
  {
    GtsGraph *gph;

    bool borrar;

    void crea(const GTSSurface &s);
    void borra(void);

  public:

    GTSGraph(const GTSSurface &s);
    GTSGraph(const GTSGraph &);
    GTSGraph &operator=(const GTSGraph &);

    void PrintStats(FILE* fp) const;

    void Write(FILE *fp);

    ~GTSGraph(void);
  };


#endif
