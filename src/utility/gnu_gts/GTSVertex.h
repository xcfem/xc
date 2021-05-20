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
//GTSVertex.h
//C++ wrapper para la clase GtsVertex de la biblioteca GTS.

#ifndef GTS_VERTEX_H
#define GTS_VERTEX_H

#include "gts.h"

class GTSSurface;
class Vector3d;
class Pos3d;
class GTSSplit;

class GTSVertex
  {
    GtsVertex *vt;

    bool borrar;

    void crea(const double &x,const double &y,const double &z);
    void borra(void);

  public:

    friend class GTSEdge;
    friend class GTSSplit;

    GTSVertex(const Pos3d &);
    GTSVertex(const double &x= 0.0,const double &y= 0.0,const double &z= 0.0);
    GTSVertex(GtsVertex *pvt);
    GTSVertex(const GTSVertex &);
    GTSVertex &operator=(const GTSVertex &);

    Pos3d Posicion(void) const;

    double GaussianCurvature(const GTSSurface &surf) const;
    Vector3d MeanCurvatureNormal(const GTSSurface &surf) const;
    void PrincipalCurvatures(const GTSSurface &surf,double &k1,double &k2) const;
    void PrincipalDirections(const GTSSurface &surf,Vector3d &e1,Vector3d &e2) const;

    GTSSplit GetSplit(const GTSVertex &v1,const GTSVertex &v2);

    ~GTSVertex(void);
  };


#endif
