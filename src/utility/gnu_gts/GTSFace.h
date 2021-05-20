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
//GTSFace.h
//C++ wrapper para la clase GtsFace de la biblioteca GTS.

#ifndef GTS_FACE_H
#define GTS_FACE_H

#include "gts.h"

class GTSEdge;

class GTSFace
  {
    GtsFace *fc;

    bool borrar;
    void crea(GTSEdge &e1,GTSEdge &e2,GTSEdge &e3);
    void borra(void);
  protected:
    GtsFace const *get_const_ptr(void) const
      { return fc; }
    GtsFace *get_ptr(void)
      { return fc; }

  public:
    friend class GTSSurface;
    friend class GTSSurfaceTraverse;

    GTSFace(GtsFace *f= nullptr);
    GTSFace(GTSEdge &e1,GTSEdge &e2,GTSEdge &e3);
    GTSFace(const GTSFace &);
    GTSFace &operator=(const GTSFace &);
    bool Null(void) const;
    ~GTSFace(void);
  };


#endif
