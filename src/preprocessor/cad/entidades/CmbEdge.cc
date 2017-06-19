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
//CmbEdge.cc

#include "CmbEdge.h"
#include "Pnt.h"
#include "Linea.h"
#include "ArcoCircunf.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Polilinea3d.h"

#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::CmbEdge::Lado::Lado(Edge *ptr,const bool &s)
  : edge(ptr), directo(s) {}

//! @brief Returns a pointer to the la linea.
XC::Edge *XC::CmbEdge::Lado::Borde(void)
  { return edge; }
//! @brief Returns a pointer constante a la linea.
const XC::Edge *XC::CmbEdge::Lado::Borde(void) const
  { return edge; }
//! @brief Assigns the line.
void XC::CmbEdge::Lado::SetEdge(Edge *l)
  { edge= l; }
//! @brief Returns a constant pointer to point origen of the line.
const XC::Pnt *XC::CmbEdge::Lado::P1(void) const
  {
    if(!edge) return nullptr;
    if(directo)
      return edge->P1();
    else
      return edge->P2();
  }
//! @brief Returns a constant pointer to point destino of the line.
const XC::Pnt *XC::CmbEdge::Lado::P2(void) const
  {
    if(!edge) return nullptr;
    if(directo)
      return edge->P2();
    else
      return edge->P1();
  }


//! @brief Returns true if the points being passed as parameters
//! son los extremos del lado.
bool XC::CmbEdge::Lado::ExtremosEn(const Pnt *p1,const Pnt *p2) const
  {
    bool retval= false;
    if(edge)
      retval= edge->ExtremosEn(p1,p2);
    const Pnt *e1= P1();
    const Pnt *e2= P2();
    if(p1==e1)
      retval= (p2==e2);
    else if(p1==e2)
      retval= (p2==e1);
    return retval;
  }

//! @brief Returns the nombre of the line.
const std::string &XC::CmbEdge::Lado::getName(void) const
  { return edge->getName(); }

//! @brief Return the longitud del lado.
double XC::CmbEdge::Lado::getLongitud(void) const
  { return edge->getLongitud(); }

//! @brief Returns a vector tangente al lado en el punto s
const XC::Vector &XC::CmbEdge::Lado::getTang(const double &s) const
  { return edge->getTang(s); }

//! @brief Returns the posiciones de la linea.
MatrizPos3d XC::CmbEdge::Lado::get_posiciones(void) const
  {
    if(edge)
      return edge->get_posiciones();
    else
      return MatrizPos3d();
  }

//! @brief Returns the identificadores of the nodes en sentido directo.
std::vector<int> XC::CmbEdge::Lado::GetTagsNodosDir(void) const
  {
    if(directo)
      return edge->GetTagsNodosDir();
    else
      return edge->GetTagsNodosInv();
  }

std::vector<int> XC::CmbEdge::Lado::GetTagsNodosInv(void) const
  {
    if(directo)
      return edge->GetTagsNodosInv();
    else
      return edge->GetTagsNodosDir();
  }

//! @brief Returns the posiciones of the nodes en sentido directo.
MatrizPos3d XC::CmbEdge::Lado::GetPosNodosDir(void) const
  {
    if(directo)
      return edge->GetPosNodosDir();
    else
      return edge->GetPosNodosInv();
  }

//! @brief Returns the posiciones of the nodes en sentido inverso.
MatrizPos3d XC::CmbEdge::Lado::GetPosNodosInv(void) const
  {
    if(directo)
      return edge->GetPosNodosInv();
    else
      return edge->GetPosNodosDir();
  }

//! @brief Returns the nodo which index is being passed as parameter empezando por el principio.
XC::Node *XC::CmbEdge::Lado::GetNodoDir(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(directo)
        return edge->GetNodoDir(i);
      else
        return edge->GetNodoInv(i);
  }

//! @brief Returns the nodo which index is being passed as parameter empezando por el final.
XC::Node *XC::CmbEdge::Lado::GetNodoInv(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(directo)
        return edge->GetNodoInv(i);
      else
        return edge->GetNodoDir(i);
  }

//! @brief Returns the nodo which index is being passed as parameter.
XC::Node *XC::CmbEdge::Lado::GetNodo(const size_t &i)
  {
    XC::Node *retval= nullptr;
    if(edge)
      {
        if(directo)
          retval=  edge->GetNodo(i);
        else
          {
            const size_t n= edge->getNumberOfNodes();
            retval=  edge->GetNodo(n-i+1);
          }
      }
    return retval;
  }

//! @brief Returns the nodo which index is being passed as parameter.
const XC::Node *XC::CmbEdge::Lado::GetNodo(const size_t &i) const
  {
    if(!edge) return nullptr;
    const size_t n= edge->getNumberOfNodes();
    if(directo)
      return edge->GetNodo(i);
    else
      return edge->GetNodo(n-i);
  }

//! @brief Genertes a mesh from the corresponding line.
void XC::CmbEdge::Lado::genMesh(meshing_dir dm)
  { edge->genMesh(dm); }

//! @brief Asigna el number of divisions of the line.
void XC::CmbEdge::Lado::SetNDiv(const size_t &nd)
  { edge->SetNDiv(nd); }

//! @brief Returns the number of divisions of the line.
size_t XC::CmbEdge::Lado::NDiv(void) const
  { return edge->NDiv(); }

//! @brief Returns the identificador of the line.
size_t XC::CmbEdge::Lado::GetTag(void) const
  { return edge->GetTag(); }

//! @brief Operador de igualdad.
bool XC::operator==(const XC::CmbEdge::Lado &il1,const XC::CmbEdge::Lado &il2)
  {
    if(il1.edge != il2.edge) return false;
    if(il1.directo != il2.directo) return false;
    return true;
  }

//! @brief Constructor.
XC::CmbEdge::CmbEdge(void)
  : Edge(),lineas(0) {}

//! @brief Constructor.
XC::CmbEdge::CmbEdge(Preprocessor *m,const size_t &nd)
  : Edge(m,nd), lineas(0) {}

//! @brief Constructor.
//! @param nombre: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param nd: number of divisions.
XC::CmbEdge::CmbEdge(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : Edge(nombre,m,nd), lineas(0) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::CmbEdge::getCopy(void) const
  { return new CmbEdge(*this); }


//! @brief Returns a pointer to the first lado.
XC::CmbEdge::Lado *XC::CmbEdge::primera_linea(void)
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.begin());
  }
//! @brief Returns a pointer to the first lado.
const XC::CmbEdge::Lado *XC::CmbEdge::primera_linea(void) const
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.begin());
  }
//! @brief Returns a pointer to the last edge.
XC::CmbEdge::Lado *XC::CmbEdge::ultima_linea(void)
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.rbegin());
  }
//! @brief Returns a pointer to the last edge.
const XC::CmbEdge::Lado *XC::CmbEdge::ultima_linea(void) const
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.rbegin());
  }

//! @brief Returns a pointer to the first punto.
const XC::Pnt *XC::CmbEdge::first_point(void) const
  {
    const Lado *pl= primera_linea();
    if(pl)
      return pl->P1();
    else
      return nullptr;
   }

//! @brief Returns a pointer to the last point.
const XC::Pnt *XC::CmbEdge::last_point(void) const
  {
    const Lado *ul= ultima_linea();
    if(ul)
      return ul->P2();
    else
      return nullptr;
   }

//! @brief Returns a constant pointer to start point.
const XC::Pnt *XC::CmbEdge::P1(void) const
  { return first_point(); }
//! @brief Returns a constant pointer to end point.
const XC::Pnt *XC::CmbEdge::P2(void) const
  { return last_point(); }

XC::Pnt *XC::CmbEdge::P1(void)
  { return const_cast<Pnt *>(first_point()); }
XC::Pnt *XC::CmbEdge::P2(void)
  { return const_cast<Pnt *>(last_point()); }

void XC::CmbEdge::reverse(void)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).reverse();
  }

//! @brief Return the longitud of the line.
double XC::CmbEdge::getLongitud(void) const
  {
    double retval= 0;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      retval+= (*i).getLongitud();
    return retval;
  }

//! @brief Returns the number of divisions total.
size_t XC::CmbEdge::NDiv(void) const
  {
    size_t &nd= const_cast<size_t &>(ndiv);
    nd= 0;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      nd+= (*i).NDiv();
    return ndiv;
  }

//! @brief Establece el number of divisions total.
void XC::CmbEdge::SetNDiv(const size_t &nd)
  {
    const size_t nl= NumEdges();
    if(nl>0)
      {
        const size_t resto= nd % nl;
        if(resto != 0)
          std::clog << nombre_clase() << "::" << __FUNCTION__
	            << "; the number of divisions ("
                    << nd << ") is not a multiple of the number of segments ("
                    << nl << ")." << std::endl;
        const size_t q= nd/nl;
        for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
          (*i).SetNDiv(q);
      }
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; no segments defined." << std::endl;
  }

MatrizPos3d XC::CmbEdge::get_posiciones(void) const
  {
    const size_t npos= NDiv()+1; //Number of positions.
    MatrizPos3d retval(npos);
    if(!lineas.empty())
      {
        size_t cont= 1;
        for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
          {
            const Edge *e= (*i).Borde();
            MatrizPos3d tmp= e->get_posiciones();
            const size_t sz= tmp.size()-1; //The last one is not added.
            for(size_t i=1;i<sz;i++)
              {
                retval(cont)= tmp(i);
                cont++;
              }
          }
        retval(npos)= last_point()->GetPos();
      }
    return retval;
  }


//! @brief Triggers node creation on the edges.
void XC::CmbEdge::create_nodes_lineas(void)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).Borde()->create_nodes();
  }

//! @brief Triggers meshing of lines.
void XC::CmbEdge::line_meshing(meshing_dir dm)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).genMesh(dm);
  }

//! @brief Creates the mesh.
void XC::CmbEdge::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing CmbEdge...(" << getName() << ")...";
    line_meshing(dm);
    
    //pointers to nodes.
    ttzNodes= TritrizPtrNod(1,NDiv()+1,1);
    size_t offset_j= 0;// Columna inicial.
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        ttzNodes.PutCaja(0,offset_j,0,(*i).Borde()->getTtzNodes());
        offset_j+= (*i).Borde()->getNumNodeRows()-1;
      }
    //pointers to elements.
    ttzElements= TritrizPtrElem(1,NDiv(),1);
    offset_j= 0;// Columna inicial.
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        ttzElements.PutCaja(0,offset_j,0,(*i).Borde()->getTtzElements());
        offset_j+= (*i).Borde()->getNumElementRows()-1;
      }
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }

//! @brief Creates and inserts the edges from the indexes
//! being passed as parameter.
void XC::CmbEdge::addLines(const ID &indices_lineas)
  {
    const size_t nl= indices_lineas.Size(); //Number of indexes.
    for(size_t i= 0;i<nl;i++)
      inserta(indices_lineas(i));
  }
 
//! @brief Creates and inserts the the edges that link the points
//! from the indexes being passed as parameter.
void XC::CmbEdge::addPoints(const ID &indices_ptos)
  {
    const size_t np= indices_ptos.Size(); //Number of indexes.
    if(np>=2)
      for(size_t i= 1;i<np;i++)
        {
          Pnt *pA= BuscaPnt(indices_ptos(i-1));
          if(!pA)
	    std::cerr << "CmbEdge::addPoints; point: "
                      << indices_ptos(i-1) << " not found in definition of surface: '"
                      << getName() << "'" << std::endl;
          Pnt *pB= BuscaPnt(indices_ptos(i));
          if(!pB)
	    std::cerr << "CmbEdge::addPoints; point: "
                      << indices_ptos(i) << " not found in definition of surface: '"
                      << getName() << "'" << std::endl;
          NuevaLinea(pA,pB);
        }
    else
      std::cerr << "CmbEdge::puntos; error se necesitan al menos dos puntos." << std::endl;
  }

//! @brief Closes the line.
void XC::CmbEdge::cierra(void)
  {
    //Cierra la polilinea.
    Pnt *pA= P2();
    if(!pA)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; last point not found." << std::endl;
    Pnt *pB= P1();
    if(!pB)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; first point not found." << std::endl;
    if(pA!=pB)
      NuevaLinea(pA,pB);
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; line is already closed." << std::endl;
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the edge set set.
XC::Edge *XC::CmbEdge::NuevaLinea(Pnt *pA,Pnt *pB)
  {
    Linea *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<Linea *>(getPreprocessor()->getCad().getLineas().createLine(pA,pB));
    if(retval)
      {
        inserta(retval);
        if(lineas.size()==1) //Is the first one.
          {
            if(pA!=lineas[0].P1())
              lineas[0].reverse();
          }
      }    
    else
       std::cerr << "CmbEdge::NuevaLinea; line between points: "
                 << pA->getName()
                 << " and " << pB->getName() << " not found in definition of surface: '"
                 << getName() << "'" << std::endl;
    return retval;
  }

//! @brief Creates a new circle arc between the points being passed
//! as parameters and put it in the edge set.
XC::Edge *XC::CmbEdge::NuevaLinea(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    ArcoCircunf *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<ArcoCircunf *>(getPreprocessor()->getCad().getLineas().createArc(pA,pB,pC));
    if(retval)
      inserta(retval);
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; arc between points: "
                << pA->getName() << ", " << pB->getName()
                << " and " << pC->getName()
		<< " not found in definition of surface: '"
                << getName() << "'" << std::endl;
    return retval;
  }

//! @brief Inserts (if found) the line which index is being passed as parameter.
void XC::CmbEdge::inserta(const size_t &i)
  {
    Edge *tmp= BuscaEdge(i);
    if(!tmp)
      std::cerr << "CmdEdge; line identified by: '" 
                    << i << "' not found.\n";
    else
      inserta(tmp);
  }

//! @brief Inserts the line which pointer is being passed as parameter.
void XC::CmbEdge::inserta(Edge *l)
  {
    if(IndiceEdge(l)!= 0) //Line already belongs to the set.
      std::cerr << "CmdEdge; Line already belongs to the set, insertion ignored.\n";
    else
      {
        if(lineas.empty())
          lineas.push_back(Lado(l,true));
        else
          {
            if(l->P1()== P2()) //directo.
              lineas.push_back(Lado(l,true));
            else if(l->P2()== P2()) //inverso.
              lineas.push_back(Lado(l,false));
            else if(l->P1()== P1()) //inverso
              lineas.push_front(Lado(l,false));
            else if(l->P2()== P1()) //directo
              lineas.push_front(Lado(l,true));
            else
              std::cerr << "CmbEdge::inserta; line: '" << l->getName()
			<< "' which ends are: '" << l->P1()->getName() << "' y '"
                        << l->P2()->getName() 
                        << "' can't connect to the combination: " << Nombre() 
			<< "' which ends are: '" << P1()->getName() << "' y '"
                        << P2()->getName() 
                        << " because they don't have shared ends." << std::endl;
          }
        actualiza_topologia();
      }
  }

//! @brief Searchs the line l from those of this compound line.
size_t XC::CmbEdge::IndiceEdge(const Edge *l) const
  {
    size_t retval= 1;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        if((*i).Borde() == l)
          return retval;
        else
          retval++;
      }
    return 0; //No la encuentra.
  }

//! @brief Returns a lado of the line compuesta.
const XC::CmbEdge::Lado *XC::CmbEdge::GetLado(const size_t &i) const
  { return &lineas[i-1]; }

//! @brief Returns a lado of the line compuesta.
XC::CmbEdge::Lado *XC::CmbEdge::GetLado(const size_t &i)
  { return &lineas[i-1]; }

//! @brief Returns the lado que tiene por extremos los puntos
//! being passed as parameters.
const XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const Pnt *p1,const Pnt *p2) const
  {
    const Lado *retval= nullptr;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      if((*i).ExtremosEn(p1,p2))
        {
          retval= &(*i);
          break;
        }
    return retval; //No la encuentra.
  }

//! @brief Returns the lado que tiene por extremos los puntos
//! being passed as parameters.
XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const Pnt *p1,const Pnt *p2)
  {
    Lado *retval= nullptr;
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      if((*i).ExtremosEn(p1,p2))
        {
          retval= &(*i);
          break;
        }
    return retval;
  }

//! @brief Returns the lado que tiene por extremos los puntos
//! being passed as parameters.
const XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const size_t &idP1,const size_t &idP2) const
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return GetLadoPorPuntos(p1,p2);
  }

//! @brief Returns the lado que tiene por extremos los puntos
//! being passed as parameters.
XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const size_t &idP1,const size_t &idP2)
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return GetLadoPorPuntos(p1,p2);
  }

//! @brief Return the deque of lines that define the edges.
std::deque<XC::Edge *> XC::CmbEdge::GetEdges(void)
  {
    std::deque<XC::Edge *> retval;
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      retval.push_back((*i).Borde());
    return retval;
  }

//! @brief Returns the i-th vertex.
const XC::Pnt *XC::CmbEdge::GetVertice(const size_t &i) const
  {
    if(i<NumVertices())
      return GetLado(i)->P1();
    else
      return P2();
  }


//! @brief Sets the i-th vertex.
void XC::CmbEdge::SetVertice(const size_t &,Pnt *)
  {
    std::cerr << "Function XC::CmbEdge::SetVertice not implemented." << std::endl;
  }

//! @brief Returns object k-points.
XC::ID XC::CmbEdge::getKPoints(void) const
  {
    const size_t numVertices= NumVertices();
    ID retval(numVertices);
    for(size_t i= 0;i<numVertices;i++)
      retval[i]= GetLado(i+1)->P1()->GetTag();
    return retval;
  }

Polilinea3d XC::CmbEdge::getPolyline(void) const
  {
    Polilinea3d retval;
    retval.push_back(P1()->GetPos());
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      retval.push_back((*i).P2()->GetPos());
    return retval;
  }


//! @brief Returns object BND.
BND3d XC::CmbEdge::Bnd(void) const
  { 
    BND3d retval;
    const size_t nv= NumVertices();
    if(nv<1) //the set is empty.
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; polyline empty." << std::endl;
        return retval;
      }
    if(nv<2)
      {
	std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; the polyline has only a point." << std::endl;
        retval= BND3d(GetVertice(1)->GetPos(),GetVertice(1)->GetPos());
        return retval;
      }
    retval= BND3d(GetVertice(1)->GetPos(),GetVertice(2)->GetPos());
    for(size_t i=3;i<=nv;i++)
      retval+= GetVertice(i)->GetPos();
    return retval;
  }
