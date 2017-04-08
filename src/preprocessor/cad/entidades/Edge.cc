//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Edge.cc

#include "Edge.h"
#include "Face.h"
#include "Pnt.h"
#include "domain/mesh/node/Node.h"
#include "utility/matrix/util_matrix.h"
#include "xc_basic/src/matrices/m_int.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"


#include "boost/any.hpp"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::Edge::Edge(Preprocessor *m,const size_t &nd)
  : EntMdlr(m), ndiv(nd) {}

//! @brief Constructor.
//! @param nombre: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param nd: Number of divisions.
XC::Edge::Edge(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : EntMdlr(nombre,0,m), ndiv(nd) {}

//! @brief Inserts a surface in contact with the line (neighbour).
void XC::Edge::inserta_surf(Face *s)
  { sups_linea.insert(s); }

//! @brief Updates topology.
void XC::Edge::actualiza_topologia(void)
  {
    if(P1()) P1()->inserta_linea(this);
    if(P2()) P2()->inserta_linea(this);
  }

//! @brief Returns a pointer to start point.
XC::Pnt *XC::Edge::P1(void)
  {
    std::cerr << "Edge::P1; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Returns a constant pointer to start point.
const XC::Pnt *XC::Edge::P1(void) const
  {
    std::cerr << "Edge::P1; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Returns a pointer to start point.
XC::Pnt *XC::Edge::P2(void)
  {
    std::cerr << "Edge::P2; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Returns a constant pointer to end point.
const XC::Pnt *XC::Edge::P2(void) const
  {
    std::cerr << "Edge::P2; this function must be redefined on"
              << " derived classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Returns true if the points being passed as parameters
//! are the ends of the edge.
bool XC::Edge::ExtremosEn(const Pnt *p1,const Pnt *p2) const
  {
    bool retval= false;
    const Pnt *e1= P1();
    const Pnt *e2= P2();
    if(p1==e1)
      retval= (p2==e2);
    else if(p1==e2)
      retval= (p2==e1);
    return retval;
  }

//! @brief Asigna el number of divisions.
void XC::Edge::SetNDiv(const size_t &nd)
  {
    if(ndiv!=nd) //Si cambia el number of divisions
      {
        if(nodos.empty()) //Not meshed.
          {
            const size_t ns= sups_linea.size();
            if(ns>1)
              {
// 	        std::clog << "Edge::SetNDiv; " << GetNombre()
//                           << " is an edge of the surfaces: "
//                           << NombresSupsTocan()
//                           << ". Number of divisions"
//                           << " was " << ndiv
//                           << " we adopt: " << nd << std::endl;
                ndiv= std::max(ndiv,nd);
              }
            else
              ndiv= nd;
          }
        else
          std::cerr << "Edge::SetNDiv; " << GetNombre()
                    << " edge is already meshed and keeps its number of divisions"
                    << " ndiv= " << ndiv << ".\n";
      }
  }

//! @brief Computes the number of divisions necessary to 
//! have the element size being passed as parameter.
void XC::Edge::SetElemSize(const double &sz)
  {
    const double l= getLongitud();
    const size_t n= ceil(l/sz);
    SetNDiv(n);
  }

//! @brief Creates points along the line.
void XC::Edge::divide(void)
  {
    const MatrizPos3d posiciones= get_posiciones();
    crea_puntos(posiciones);
  }

//! @brief Returns a pointer to node which indices are being passed as parameters.
XC::Node *XC::Edge::GetNodo(const size_t &i,const size_t &j,const size_t &k)
  { return EntMdlr::GetNodo(i,j,k); }

//! @brief Returns a pointer to node which indices are being passed as parameters.
const XC::Node *XC::Edge::GetNodo(const size_t &i,const size_t &j,const size_t &k) const
  { return EntMdlr::GetNodo(i,j,k); }

//! @brief Returns a pointer to node which index is being passed as parameter.
XC::Node *XC::Edge::GetNodo(const size_t &i)
  {  return const_cast<Node *>(static_cast<const Edge &>(*this).GetNodo(i)); }

//! @brief Returns a pointer to node which index is being passed as parameter.
const XC::Node *XC::Edge::GetNodo(const size_t &i) const
  { return nodos.getAtI(i); }

//! @brief Returns the nodo which index is being passed as parameter empezando por el principio.
XC::Node *XC::Edge::GetNodoDir(const size_t &i)
  { return GetNodo(i); }

//! @brief Returns the nodo which index is being passed as parameter empezando por el principio.
const XC::Node *XC::Edge::GetNodoDir(const size_t &i) const
  { return GetNodo(i); }

//! @brief Returns the nodo which index is being passed as parameter empezando por el final.
XC::Node *XC::Edge::GetNodoInv(const size_t &i)
  { return GetNodo(NumNodos()-i+1); }

//! @brief Returns the nodo which index is being passed as parameter empezando por el final.
const XC::Node *XC::Edge::GetNodoInv(const size_t &i) const
  { return GetNodo(NumNodos()-i+1); }

//! @brief Returns the first node of the line.
const XC::Node *XC::Edge::GetPrimerNodo(void) const
  { 
    const Node *retval= GetNodo(1);
    if(!retval)
      {
        const Pnt *p1= P1();
        if(p1)
          retval= p1->GetNodo();
      }
    return retval;
  }

//! @brief Returns the first node of the line.
XC::Node *XC::Edge::GetPrimerNodo(void)
  { 
    Node *retval= GetNodo(1);
    if(!retval)
      {
        Pnt *p1= P1();
        if(p1)
          retval= p1->GetNodo();
      }
    return retval;
  }

//! @brief Returns the último nodo of the line.
const XC::Node *XC::Edge::GetUltimoNodo(void) const
  { 
    const Node *retval= GetNodoInv(1);
    if(!retval)
      {
        const Pnt *p2= P2();
        if(p2)
          retval= p2->GetNodo();
      }
    return retval;
  }

//! @brief Returns the último nodo of the line.
XC::Node *XC::Edge::GetUltimoNodo(void)
  { 
    Node *retval= GetNodoInv(1);
    if(!retval)
      {
        Pnt *p2= P2();
        if(p2)
          retval= p2->GetNodo();
      }
    return retval;
  }

//! @brief Returns the identificadores de los nodos en sentido directo.
std::vector<int> XC::Edge::GetTagsNodosDir(void) const
  {
    const size_t nn= NumNodos();
    std::vector<int> retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= GetNodo(i)->getTag();
    return retval;
  }

//! @brief Returns the identificadores de los nodos en sentido directo.
std::vector<int> XC::Edge::GetTagsNodosInv(void) const
  {
    const size_t nn= NumNodos();
    std::vector<int> retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= GetNodo(nn-i+1)->getTag();
    return retval;
  }

//! @brief Returns the posiciones de los nodos en sentido directo.
MatrizPos3d XC::Edge::GetPosNodosDir(void) const
  {
    const size_t nn= NumNodos();
    MatrizPos3d retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval(i,1)= pos_nodo(*GetNodo(i));
    return retval;
  }

//! @brief Returns the posiciones de los nodos en sentido inverso.
MatrizPos3d XC::Edge::GetPosNodosInv(void) const
  {
    const size_t nn= NumNodos();
    MatrizPos3d retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval(i,1)= pos_nodo(*GetNodo(nn-i+1));
    return retval;
  }

//! @brief Returns the surface names that touch the line (neighbors).
const std::string &XC::Edge::NombresSupsTocan(void) const
  {
    static std::string retval;
    retval= "";
    if(!sups_linea.empty())
      {
        std::set<const Face *>::const_iterator i= sups_linea.begin();
        retval+= (*i)->GetNombre();
        i++;
        for(;i!=sups_linea.end();i++)
          retval+= "," + (*i)->GetNombre();
      }
    return retval;
  }

//! @brief Returns the lados homólogos a éste.
std::set<const XC::Edge *> XC::Edge::GetLadosHomologos(const std::set<const XC::Edge *> &lh) const
  {
    std::set<const Edge *> retval;
    std::set<const Edge *> nuevos_adyacentes;

    if(!sups_linea.empty())
      {
        for(std::set<const Face *>::const_iterator i= sups_linea.begin();i!=sups_linea.end();i++)
          {
            const Edge *h= (*i)->get_lado_homologo(this);
            if(h!=this)
              {
                std::set<const XC::Edge *>::const_iterator k= lh.find(h);
                if(k==lh.end()) //Not already added
                  nuevos_adyacentes.insert(h);
              }
          }
      }
    for(std::set<const XC::Edge *>::const_iterator i= nuevos_adyacentes.begin();i!=nuevos_adyacentes.end();i++)
      retval.insert(*i);
    if(retval.size()>lh.size()) //Hay lados nuevos.
      {
        for(std::set<const XC::Edge *>::const_iterator i= nuevos_adyacentes.begin();i!=nuevos_adyacentes.end();i++)
          {
            std::set<const XC::Edge *> tmp= (*i)->GetLadosHomologos(retval);
            for(std::set<const XC::Edge *>::const_iterator j= tmp.begin();j!=tmp.end();j++)
              if(this!=*j) retval.insert(*j);
          }
        nuevos_adyacentes.erase(nuevos_adyacentes.begin(),nuevos_adyacentes.end());
      }
    return retval;
  }

//! @brief Computes number of divisions for each line
//! to make it compatible with adjacent surface meshing.
size_t XC::calcula_ndiv_lados(const std::set<const XC::Edge *> &lados)
  {
    size_t nd= 0;
    for(std::set<const XC::Edge *>::const_iterator i= lados.begin();i!=lados.end();i++)
      nd= std::max(nd,(*i)->NDiv());
    return nd;
  }

//! @brief Returns true if the line touches the surface (neighbor).
bool XC::Edge::Toca(const Face &s) const
  {
    std::set<const Face *>::const_iterator i= sups_linea.find(&s);
    return (i!=sups_linea.end());
  }

//! @brief Returns true if the line touches the body.
bool XC::Edge::Toca(const Body &b) const
  {
    for(std::set<const Face *>::const_iterator i= sups_linea.begin(); i!=sups_linea.end();i++)
      { if((*i)->Toca(b)) return true; }
    return false;
  }

//! @brief Returns verdadero el punto es un extremo of the line.
bool XC::Edge::Extremo(const Pnt &p) const
  { return ((&p == P1()) || (&p == P2()));  }

//! @brief Return the lista of edges that begin or end in this point.
std::set<const XC::Edge *> XC::GetLineasTocan(const Pnt &p)
  { return p.EdgesTocan(); }

//! @brief Returns an matrix of positions along the line.
MatrizPos3d XC::Edge::get_pos_nodos(void) const
  { return get_posiciones(); }

//! @brief Creates the nodes for both end points of the edge.
void XC::Edge::crea_nodos_en_extremos(void)
  {
    if(verborrea>4)
      std::clog << "Creating nodes for '" << GetNombre() << "' edge ends...";   
    if(!P1())
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; start point undefined." << std::endl;
        return;
      }
    else
      P1()->crea_nodos();

    Node *nodo_p1= P1()->GetNodo(1,1,1);
    assert(nodo_p1);
    nodos(1,1,1)= nodo_p1; //Nodo del start point.
      
    if(!P2())
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; end point undefined." << std::endl;
        return;
      }
    else
      P2()->crea_nodos();

    Node *nodo_p2= P2()->GetNodo(1,1,1);
    assert(nodo_p2);
    const size_t filas= nodos.getNumFilas();
    const size_t cols= nodos.getNumCols();
    nodos(1,filas,cols)= nodo_p2; //Nodo del end point.

    if(verborrea>4)
     std::cerr << "Edge::crea_nodos_en_extremos(); creados." << std::endl;
  }

//! @brief Cretes nodes of the object.
void XC::Edge::crea_nodos(void)
  {
    if(verborrea>4)
      std::clog << "Creating nodes for edge: '" << GetNombre() << "'...";

 
    if(nodos.Null())
      {
        if(!get_preprocessor())
          std::cerr << "Edge::crea_nodos; preprocessor undefined." << std::endl;
        else
          {
            const MatrizPos3d posiciones= get_pos_nodos();
            const size_t filas= posiciones.getNumFilas();
            const size_t cols= posiciones.getNumCols();
            nodos= TritrizPtrNod(1,filas,cols);

            crea_nodos_en_extremos();


            if((filas*cols)>2) //Si tiene nodos intermedios...
              {
                const size_t fila_fin= std::max(filas-1,size_t(1));
                const size_t fila_ini= (fila_fin == 1 ? 1 : 2);
                const size_t col_fin= std::max(cols-1,size_t(1));
                const size_t col_ini= (col_fin == 1 ? 1 : 2);
                for(register size_t j= fila_ini;j<=fila_fin;j++)
                  for(register size_t k= col_ini;k<=col_fin;k++)
                    crea_nodo(posiciones(j,k),1,j,k);
              }
          }
        if(verborrea>4)
	  std::clog << "Edge::crea_nodos(); creados " << nodos.NumPtrs() << " nodo(s)." << std::endl;
      }
    else
      {
        if(verborrea>2)
          std::clog << "Edge::crea_nodos; los nodos of the line: '" << GetNombre() << "' already exist." << std::endl;
      }
    if(verborrea>4)
      std::clog << "creados." << std::endl;
  }

//! @brief Triggers mesh generation.
void XC::Edge::genMesh(meshing_dir dm)
  {
    if(verborrea>3)
      std::clog << "Meshing edge...(" << GetNombre() << ")...";   
    crea_nodos();
    crea_elementos(dm);
    if(verborrea>3)
      std::clog << "done." << std::endl;
  }

//! @brief Returns a vector tangent to the line in point at parameter s.
const XC::Vector &XC::Edge::getTang(const double &s) const
  {
    static XC::Vector retval(1);
    std::cerr << "Edge::getTang; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the sets to wich this edge belongs.
std::set<XC::SetBase *> XC::Edge::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Adds the «edge» a the sets being passed as parameters.
void XC::Edge::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetLineas().push_back(this);
      }
  }

//! Return indices of the vertices.
std::vector<int> XC::Edge::getIndicesVertices(void) const
  {
    const size_t nv= NumVertices();
    std::vector<int> retval(nv);
    if(nv>=1)
      {
        for(size_t i=0;i<nv;i++)
          retval[i]= GetVertice(i+1)->getIdx();
      }
    return retval;
  }

//! @brief Returns k-points.
XC::ID XC::Edge::getKPoints(void) const
  {
    ID retval(2);
    retval[0]= P1()->GetTag();
    retval[1]= P2()->GetTag();
    return retval;
  }



