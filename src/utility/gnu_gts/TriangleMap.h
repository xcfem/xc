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
//TriangleMap.h

#ifndef TRIANGLE_MAP_H
#define TRIANGLE_MAP_H


#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <unistd.h>
#include "gts.h"
#include "utility/gnu_gts/GTSVertex.h"
#include "utility/gnu_gts/GTSFace.h"
#include "utility/gnu_gts/GTSSurface.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/KDTreePos.h"
#include "utility/kdtree++/kdtree.hpp"
#include <iostream>
#include <map>

Pos3d Vertex2Pos3d(const GtsVertex &v);

//! \ingroup Nod
//
//! @brief Vertex position wrapper for KDTree.
class VertexPos: public KDTreePos
  {
  private:
    int index;
  public:
    VertexPos(const int &i, const Pos3d &);
    explicit VertexPos(const Pos3d &p);
    inline const int &getIndex(void) const
      { return index; }
    static inline double tac( VertexPos p, size_t k ) { return p[k]; }
  };

inline bool operator==(const VertexPos &A,const VertexPos &B)
  { return ((A.getIndex()== B.getIndex()) && (A[0] == B[0]) && (A[1] == B[1]) && (A[2] == B[2])); }

class VerticesKDTree: protected kd_tree::KDTree<3, VertexPos, std::pointer_to_binary_function<VertexPos,size_t,double> >
  {
    size_t pend_optimizar;
  public:
    typedef kd_tree::KDTree<3, VertexPos, std::pointer_to_binary_function<VertexPos,size_t,double> > tree_type;
    VerticesKDTree(void);

    void insert(const int &,const Pos3d &);
    //void erase(const VertexPos &);
    void clear(void);

    int getNearest(const Pos3d &pos) const;
    int getNearestBallPoint(const Pos3d &pos, const double &r) const;
  };

class VerticesMap: public std::map<size_t,Pos3d>
  {

    static bool chequea_vertice(GtsVertex *v);

    friend class TriangleMap;

  public:
    void insert(GtsVertex *v);
    void insert(const size_t &i,const Pos3d &p);
    void Print(std::ostream &os) const;
    void Borra(void);
  };

std::ostream &operator<<(std::ostream &os, const VerticesMap &mv);

class TriangleVerticesIndexes
//Almacena tres indices que resultan de convertir
//los punteros a los vértices en enteros
  {
    size_t v1,v2,v3;

    bool chequea_edges(GtsTriangle *t);

  public:
    TriangleVerticesIndexes(void);
    TriangleVerticesIndexes(const size_t &,const size_t &,const size_t &);
    TriangleVerticesIndexes(GtsTriangle *t);
    inline const size_t &V1(void) const
      { return v1; }
    inline const size_t &V2(void) const
      { return v2; }
    inline const size_t &V3(void) const
      { return v3; }
    inline size_t &V1(void)
      { return v1; }
    inline size_t &V2(void)
      { return v2; }
    inline size_t &V3(void)
      { return v3; }
    inline bool operator==(const TriangleVerticesIndexes &other) const
      { return (v1==other.v1) && (v2==other.v2) && (v3==other.v3); }
    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os, const TriangleVerticesIndexes &ivc);

class TriangleFaces: public std::deque<TriangleVerticesIndexes>
  {
  public:
    void Append(const size_t &,const size_t &,const size_t &);
    void Print(std::ostream &os) const;
  };

class TriangleMap
  {
    VerticesMap mv;
    TriangleFaces faces;
  public:
    inline const VerticesMap &getVertices(void) const
      { return mv; }
    inline VerticesMap &getVertices(void)
      { return mv; }
    inline const TriangleFaces &getFaces(void) const
      { return faces; }
    inline TriangleFaces &getFaces(void)
      { return faces; }

    void insertVertex(GtsVertex *v);
    void AppendFace(GtsTriangle *t);
    void Renumera(void);
    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os, const TriangleMap &mt);

TriangleMap getTriangleMap(const GTSSurface &sf);

#endif
