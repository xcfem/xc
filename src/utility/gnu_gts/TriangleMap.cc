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
//TriangleMap.cc

#include "TriangleMap.h"

Pos3d Vertex2Pos3d(const GtsVertex &v)
  { return Pos3d(v.p.x,v.p.y,v.p.z); }

//! @brief Constructor.
VertexPos::VertexPos(const int &i,const Pos3d &p)
  : KDTreePos(p), index(i) {}

//! @brief Constructor.
VertexPos::VertexPos(const Pos3d &p)
  : KDTreePos(p), index(0) {}

//! @brief Constructor.
VerticesKDTree::VerticesKDTree(void)
  : tree_type(std::ptr_fun(VertexPos::tac)), pend_optimizar(0) {}


void VerticesKDTree::insert(const int &i,const Pos3d &p)
  {
    tree_type::insert(VertexPos(i,p));
  }

// void VerticesKDTree::erase(const VertexPos &vp)
//   {
//     tree_type::erase(vp);
//     pend_optimizar++;
//     if(pend_optimizar>=10)
//       {
// 	tree_type::optimise();
//         pend_optimizar= 0;
//       }
//   }

void VerticesKDTree::clear(void)
  { tree_type::clear(); }
  
//! @brief Query the kd-tree for nearest neighbor.
int VerticesKDTree::getNearest(const Pos3d &pos) const
  {
    int retval= -1; //Negative if not found.
    VertexPos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target);
    if(found.first != end())
      retval= found.first->getIndex();
    return retval;
  }

//! @brief Query the kd-tree for nearest neighbor within distance r of point p.
int VerticesKDTree::getNearestBallPoint(const Pos3d &p, const double &r) const
  {
    int retval= -1; //Negative if not found.
    VertexPos target(p);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target,r);
    if(found.first != end())
      retval= found.first->getIndex();
    return retval;
  }

static gboolean vertex_add(GtsVertex *v,TriangleMap *mt)
  {
    mt->insertVertex(v);
    return TRUE;
  }
static gboolean triangle_add(GtsTriangle *t,TriangleMap *mt)
  {
    mt->AppendFace(t);
    return TRUE;
  }

bool VerticesMap::chequea_vertice(GtsVertex *v)
  {
    g_return_val_if_fail (v != nullptr, FALSE);
    g_return_val_if_fail (GTS_OBJECT (v)->reserved == nullptr, FALSE);
    g_assert (GTS_OBJECT (v)->reserved == nullptr);
    return true;
  }

void VerticesMap::insert(GtsVertex *v)
  {
    if(!chequea_vertice(v))
      std::cerr << "VerticesMap::" << __FUNCTION__
	        << ": triying to insert an erroneous vertex."
		<< std::endl;
    insert(size_t(v),Vertex2Pos3d(*v));
  }
void VerticesMap::insert(const size_t &i,const Pos3d &p)
  { (*this)[i]= p; }
void VerticesMap::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << "Vertice: " << (*i).first << ": " << (*i).second << std::endl;
  }
void VerticesMap::Borra(void)
  { erase(begin(),end()); }

std::ostream &operator<<(std::ostream &os, const VerticesMap &mv)
  {
    mv.Print(os);
    return os;
  }

//! @brief Constructor.
TriangleVerticesIndexes::TriangleVerticesIndexes(void)
  : v1(0), v2(0), v3(0)
  {
    std::cerr << "Default constructor creates a degenerated triangle." << std::endl;
  }

//! @brief Constructor.
TriangleVerticesIndexes::TriangleVerticesIndexes(const size_t &i,const size_t &j,const size_t &k)
  : v1(i), v2(j), v3(k)
  {
    if((i==j) || (i==k) || (j==k))
      std::cerr << "TriangleVerticesIndexes: Degenerated triangle." << std::endl;
  }

TriangleVerticesIndexes::TriangleVerticesIndexes(GtsTriangle *t)
  : v1(size_t(gts_triangle_vertex_opposite(t,t->e1))),
    v2(size_t(gts_triangle_vertex_opposite(t,t->e2))),
    v3(size_t(gts_triangle_vertex_opposite(t,t->e3)))
  {
    if(!chequea_edges(t))
      std::cerr << "TriangleVerticesIndexes: Error creating face." << std::endl;
  }
bool TriangleVerticesIndexes::chequea_edges(GtsTriangle *t)
  {
    g_return_val_if_fail(t != nullptr, false);
    g_return_val_if_fail(t->e1 != nullptr, false);
    g_return_val_if_fail(t->e2 != nullptr, false);
    g_return_val_if_fail(t->e3 != nullptr, false);
    g_return_val_if_fail(t->e1 != t->e2 && t->e1 != t->e3 && t->e2 != t->e3, false);
    g_assert(gts_segments_touch (GTS_SEGMENT (t->e1), GTS_SEGMENT (t->e2)));
    g_return_val_if_fail (gts_segments_touch (GTS_SEGMENT (t->e1), GTS_SEGMENT (t->e3)), FALSE);
    g_return_val_if_fail (gts_segments_touch (GTS_SEGMENT (t->e2), GTS_SEGMENT (t->e3)), FALSE);
    g_return_val_if_fail (GTS_SEGMENT (t->e1)->v1 != GTS_SEGMENT (t->e1)->v2, FALSE);
    g_return_val_if_fail (GTS_SEGMENT (t->e2)->v1 != GTS_SEGMENT (t->e2)->v2, FALSE);
    g_return_val_if_fail (GTS_SEGMENT (t->e3)->v1 != GTS_SEGMENT (t->e3)->v2, FALSE);
    g_return_val_if_fail (GTS_OBJECT (t)->reserved == nullptr, FALSE);
    return true;
  }
void TriangleVerticesIndexes::Print(std::ostream &os) const
  {
    os << "Cara: " << v1 << ", " << v2 << ", " << v3;
  }
std::ostream &operator<<(std::ostream &os, const TriangleVerticesIndexes &ivc)
  {
    ivc.Print(os);
    return os;
  }

void TriangleFaces::Append(const size_t &i,const size_t &j,const size_t &k)
  { push_back(TriangleVerticesIndexes(i,j,k)); }

void TriangleFaces::Print(std::ostream &os) const
  {
    os << "Face list: " << std::endl;
    for(const_iterator i= begin();i!= end();i++)
      os << *i << std::endl;
  }


void TriangleMap::insertVertex(GtsVertex *v)
  { mv.insert(v); }

void TriangleMap::AppendFace(GtsTriangle *t)
  { faces.push_back(TriangleVerticesIndexes(t)); }

void TriangleMap::Renumera(void)
  {
    VerticesMap mv_renum; //Vertices renumerados.
    size_t contador= 1;
    for(VerticesMap::iterator i= mv.begin();i!=mv.end();i++)
      {
        const size_t ind_vertice= (*i).first;
        const Pos3d coo_vertice= (*i).second;
        for(std::deque<TriangleVerticesIndexes>::iterator j= faces.begin();j!=faces.end();j++)
          {
            TriangleVerticesIndexes &cara_j= *j;
            if(cara_j.V1() == ind_vertice) cara_j.V1()= contador;
            if(cara_j.V2() == ind_vertice) cara_j.V2()= contador;
            if(cara_j.V3() == ind_vertice) cara_j.V3()= contador;
          }
        mv_renum[contador]= coo_vertice;
        contador++;
      }
    mv.Borra();
    mv= mv_renum;
  }
void TriangleMap::Print(std::ostream &os) const
  {
    os << "Vertex list: " << std::endl;
    mv.Print(os);
    faces.Print(os);
  }
std::ostream &operator<<(std::ostream &os, const TriangleMap &mt)
  {
    mt.Print(os);
    return os;
  }

TriangleMap getTriangleMap(const GTSSurface &sf)
  {
    TriangleMap retval;
    sf.ForEachVertex((GtsFunc)vertex_add,&retval);
    sf.ForEachFace((GtsFunc)triangle_add,&retval);
    retval.Renumera();
    return retval;
  }


