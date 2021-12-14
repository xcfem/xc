// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//GTSSurfacetraverse.h
//C++ wrapper para la clase GtsSurfaceTraverse de la biblioteca GTS.

#ifndef GTS_SURFACETRAVERSE_H
#define GTS_SURFACETRAVERSE_H

#include "gts.h"

class GTSSurface;
class GTSFace;

class GTSSurfaceTraverse
  {
    GtsSurfaceTraverse *sft;

    bool borrar;

    void crea(const GTSSurface &s,const GTSFace &f);
    void borra(void);

  public:

    GTSSurfaceTraverse(const GTSSurface &s,const GTSFace &f);
    GTSSurfaceTraverse(const GTSSurfaceTraverse &);
    GTSSurfaceTraverse &operator=(const GTSSurfaceTraverse &);
    GTSFace Next(guint *level= nullptr) const;
    ~GTSSurfaceTraverse(void);
  };


#endif
