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
//VertexMap.cc

#include "VertexMap.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3dList.h"
#include <stack>

VerticesArista::VerticesArista(const size_t &i,const size_t &j)
  : v0(i), v1(j) { assert(i!=j); }

void VerticesArista::swap(void)
  { std::swap(v0,v1); }

bool VerticesArista::comparteVertices(const VerticesArista &other) const
  {
    bool retval= false;
    if((*this)==other)
      retval= false;
    else if((v0== other.v1) && (v1== other.v0))
      retval= true;
    return retval;
  }

bool operator==(const VerticesArista &a,const VerticesArista &b)
  { return ((a.v0==b.v0) && (a.v1==b.v1)); }

VerticesCara::VerticesCara(void)
  : visitada(false) {}

void VerticesCara::setVisitada(const bool &b)
  { visitada= b; }

const bool &VerticesCara::yaVisitada(void) const
  { return visitada; }

void VerticesCara::swap(void)
  { reverse(); }



//! @brief Return the face vertex positions.
Pos3dList VerticesCara::getPosVertices(const std::vector<Pos3d> &vertices) const
  {
    Pos3dList retval;
    for(const_iterator i=begin();i!=end();i++)
      retval.appendPoint(vertices[*i]);
    return retval;
  }

//! @brief Return the face centroid.
Pos3d VerticesCara::getCentroide(const std::vector<Pos3d> &vertices) const
  { return getPosVertices(vertices).getCenterOfMass();  }

//! @brief Return the face edges.
std::list<VerticesArista> VerticesCara::getEdges(void) const
  {
    std::list<VerticesArista> retval;
    const_iterator i= begin();
    const_iterator j= i; j++;
    for(;j!=end();j++)
      {
        retval.push_back(VerticesArista(*i,*j));
        i=j;
      }
    retval.push_back(VerticesArista(*i,*begin()));
    return retval;
  }

//! @brief Return true if la cara contiene la arista
//! que se pasa como parámetro.
bool VerticesCara::tieneArista(const VerticesArista &arista) const
  {
    bool retval= false;
    const_iterator i= std::find(begin(),end(),arista.getVI());
    if(i!=end())
      {
        const_iterator j= std::find(begin(),end(),arista.getVJ());
        if(j!=end())
          {
            const_iterator k= i;k++;
            if(k==j)
              retval= true;
            else
              {
                k= i; k--;
                if(k==j)
                  retval= true;
              }
          }
      }
    return retval;
  }

//! @brief Return true if the face contains the edge
//! being passed as paramenter and the orientations match.
bool VerticesCara::tieneAristaOrientada(const VerticesArista &arista) const
  {
    bool retval= false;
    const_iterator i= std::find(begin(),end(),arista.getVI());
    if(i!=end())
      {
        const_iterator j= std::find(begin(),end(),arista.getVJ());
        if(j!=end())
          {
            const_iterator k= i;k++;
            if(k==j)
              retval= true;
          }
      }
    return retval;
  }

//! @brief Return the faces that share an edge with this one.
std::deque<VerticesCara> VerticesCara::compartenArista(const std::deque<VerticesCara> &caras) const
  {
    std::deque<VerticesCara> retval;
    std::list<VerticesArista> edges= getEdges();
    for(std::list<VerticesArista>::const_iterator i= edges.begin();i!=edges.end();i++)
      {
        const VerticesArista &arista= *i;
        for(std::deque<VerticesCara>::const_iterator j= caras.begin();j!=caras.end();j++)
          if(*j!=*this)
            if(j->tieneArista(arista))
              retval.push_back(*j);
      }
    return retval;
  }

//! @brief Imprime los índices de los vértices.
void VerticesCara::Print(std::ostream &os) const
  {
    for(const_iterator j=begin();j!=end();j++)
      os << " " << *j;
  }

std::ostream &operator<<(std::ostream &os,const VerticesCara &vc)
  {
    vc.Print(os);
    return os;
  }

//! @brief Constructor.
VertexMap::VertexMap(const TripletMap<Pos3d> &vertexMap)
  : vertices(vertexMap.size()) 
  {
    //Colocamos los vértices.
    TripletMap<size_t> indices;
    size_t conta= 0;
    for(TripletMap<Pos3d>::const_iterator i=vertexMap.begin();i!=vertexMap.end();i++,conta++)
      {
        Triplete t= (*i).first;
        indices[t]= conta;
        vertices[conta]= (*i).second;
      }

    std::set<size_t> planes;
    //Get the planes that contain the faces.
    for(TripletMap<Pos3d>::const_iterator i=vertexMap.begin();i!=vertexMap.end();i++)
      {
        Triplete t= (*i).first;
        planes.insert(t.get<0>());
        planes.insert(t.get<1>());
        planes.insert(t.get<2>());
      }

    //Get the faces for each plane.
    for(std::set<size_t>::const_iterator i=planes.begin();i!=planes.end();i++)
      {
        const size_t indice= *i;
        VerticesCara indicesCara;
        for(TripletMap<Pos3d>::const_iterator j=vertexMap.begin();j!=vertexMap.end();j++)
          {
            Triplete t= (*j).first;
            const size_t indiceVertice= indices[t];
            if(t.get<0>()==indice) indicesCara.push_back(indiceVertice);
            if(t.get<1>()==indice) indicesCara.push_back(indiceVertice);
            if(t.get<2>()==indice) indicesCara.push_back(indiceVertice);
          }
        assert(indicesCara.size()>=3);
        caras.push_back(indicesCara);
        indicesCara.clear();
      }
    //Hacemos consistente la ordenación de vértices en las caras.
    makeConsistent();
  }

//! @brief Return los vértices.
const std::vector<Pos3d> &VertexMap::getVertices(void) const
  { return vertices; }

//! @brief Return el número de vertices.
size_t VertexMap::getNumVertices(void) const
  { return vertices.size(); }

//! @brief Return el número de caras.
size_t VertexMap::getNumCaras(void) const
  { return caras.size(); }

//! @brief Return the cara cuyo índice se pasa como parámetro.
const VerticesCara &VertexMap::getCara(const size_t &i) const
  { return caras[i]; }

//! @brief Return las caras.
const std::deque<VerticesCara > &VertexMap::getCaras(void) const
  { return caras; }

//! @brief Return el centroide.
Pos3d VertexMap::getCentroide(void) const
  {
    const GEOM_FT sz= getNumVertices();
    Vector3d tmp;
    for(std::vector<Pos3d>::const_iterator i=vertices.begin();i!=vertices.end();i++)
      tmp= tmp + (*i).VectorPos();
    return Pos3d(tmp.x()/sz,tmp.y()/sz,tmp.z()/sz);
  }

std::deque<VerticesCara *> VertexMap::tienenArista(const VerticesArista &arista)
  {
    std::deque<VerticesCara *> retval;
    for(std::deque<VerticesCara>::iterator i= caras.begin();i!=caras.end();i++)
      if(i->tieneArista(arista))
        retval.push_back(&(*i));
    return retval;
  }

void VertexMap::makeConsistent(void)
  {
    for(std::deque<VerticesCara >::iterator i=caras.begin();i!=caras.end();i++)
      (*i).setVisitada(false);
    
    std::stack<VerticesCara> pilaCaras;
    std::deque<VerticesCara >::iterator i= caras.begin();
    pilaCaras.push(*i);
    i->setVisitada(true);

    while(!pilaCaras.empty())
      {
        VerticesCara f= pilaCaras.top();
        pilaCaras.pop();
        const std::list<VerticesArista> edges= f.getEdges();
        for(std::list<VerticesArista>::const_iterator i= edges.begin();i!=edges.end();i++)
          {
            std::deque<VerticesCara *> adyacentes= tienenArista(*i);
            for(std::deque<VerticesCara *>::iterator j= adyacentes.begin();j!=adyacentes.end();j++)
              if(!((*j)->yaVisitada()))
                {
                  if((*j)->tieneAristaOrientada(*i))
                    (*j)->swap();
                  (*j)->setVisitada(true);
                  pilaCaras.push(*(*j));
                }
          }
      }
  }

void VertexMap::Print(std::ostream &os) const
  {
    os << "vertices:" << std::endl;
    for(std::vector<Pos3d>::const_iterator i=vertices.begin();i!=vertices.end();i++)
      os << *i << std::endl;
    os << "caras:" << std::endl;
    for(std::deque<VerticesCara >::const_iterator i=caras.begin();i!=caras.end();i++)
      { os << *i << std::endl; }
  }

std::ostream &operator<<(std::ostream &os,const VertexMap &vm)
  {
    vm.Print(os);
    return os;
  }

//! @brief Constructor.
BuildFromVertexMap::BuildFromVertexMap(const TripletMap<Pos3d> &v)
      :vertex_map(v) {}

//! @brief Operador ().
void BuildFromVertexMap::operator()(Polyhedron3d::HalfedgeDS &hds)
  {
    // Postcondition: `hds' is a valid polyhedral surface.
    CGAL::Polyhedron_incremental_builder_3<Polyhedron3d::HalfedgeDS> B( hds, true);
    const std::vector<Pos3d> &vertices= vertex_map.getVertices();
    const size_t numVertices= vertices.size();
    const size_t numCaras= vertex_map.getNumCaras();
     
    B.begin_surface(numVertices,numCaras);
    //Agregamos vértices.
    typedef Polyhedron3d::HalfedgeDS::Vertex Vertex;
    typedef Vertex::Point Point;
    for(std::vector<Pos3d>::const_iterator i=vertices.begin();i!=vertices.end();i++)
      B.add_vertex(Point((*i).ToCGAL()));

    const std::deque<VerticesCara> &caras= vertex_map.getCaras();
    for(std::deque<VerticesCara>::const_iterator i=caras.begin();i!=caras.end();i++)
      {
        B.begin_facet();
        const VerticesCara &cara= *i;
        for(VerticesCara::const_iterator j=cara.begin();j!=cara.end();j++)
          B.add_vertex_to_facet(*j);
        B.end_facet();
      }
    B.end_surface();
  }
