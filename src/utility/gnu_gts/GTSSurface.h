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
//GTSSuface.h
//C++ wrapper para la clase GtsSurface de la biblioteca GTS.

#ifndef GTS_SURFACE_H
#define GTS_SURFACE_H

#include "gts.h"

class GTSVertex;
class GTSFace;
class GTSSurfaceTraverse;
class GTSBBoxTree;
class GTSGraph;
class GTSSplit;
class Vector3d;
class TriangleMap;

class GTSSurface
  {
    GtsSurface *s;

    bool borrar;

    void crea(void);
    void borra(void);

  protected:
    GtsSurface const *get_const_ptr(void) const
      { return s; }
    GtsSurface *get_ptr(void)
      { return s; }
    GTSSurface(GtsSurface *ptr);
  public:
    friend class GTSVertex;
    friend class GTSSurfaceTraverse;
    friend class GTSSurfaceIntersection;
    friend class GTSGraph;

    GTSSurface(void);
    GTSSurface(const GTSSurface &);
    GTSSurface &operator=(const GTSSurface &);

    int Read(GtsFile *fp);
    void Write(FILE *fp);
    void WriteOogl(FILE *fp);

    void PrintStats(FILE* fp) const;
    bool IsClosed(void) const;
    bool IsOpen(void) const;
    bool IsOrientable(void) const;
    GTSSurface selfIntersecting(void) const;
    bool Vacia(void) const;
    bool CheckSelfIntersection(bool verbose) const;

    void appendFace(GTSFace &f);
    void removeFace(GTSFace &f);
    void Merge(const GTSSurface &);

    guint getNumVertices(void) const;
    guint getNumEdges(void) const;
    guint GetNumCaras(void) const;
    GTSSurfaceTraverse Begin(void) const;
    GTSGraph Graph(void) const;
    void ForEachVertex(GtsFunc func,gpointer data) const;
    void ForEachEdge(GtsFunc func,gpointer data) const;
    void ForEachFace(GtsFunc func,gpointer data) const;

    GNode *BBoxTree(void) const;

    double GaussianCurvature(const GTSVertex &v) const;
    Vector3d MeanCurvatureNormal(const GTSVertex &v) const;
    void PrincipalCurvatures(const GTSVertex &v,double &k1,double &k2) const;
    void PrincipalDirections(const GTSVertex &v,Vector3d &e1,Vector3d &e2) const;

    double Volume(void) const
      { return gts_surface_volume(s); }
    //! @brief Return the sum of the areas (with sign) of the faces.
    double getArea(void) const
      { return gts_surface_area(s); }

    TriangleMap GetTriangleMap(void) const;

    ~GTSSurface(void);
  };


GTSSurface Union(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection=false,bool verbose=false);
GTSSurface intersection(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection=false,bool verbose=false);
GTSSurface Diferencia(const GTSSurface &a,const GTSSurface &b,bool check_self_intersection=false,bool verbose=false);


#endif
