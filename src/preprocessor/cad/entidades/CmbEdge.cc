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

#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::CmbEdge::Lado::Lado(Edge *ptr,const bool &s)
  : edge(ptr), directo(s) {}

//! @brief Devuelve un puntero a la linea.
XC::Edge *XC::CmbEdge::Lado::Borde(void)
  { return edge; }
//! @brief Devuelve un puntero constante a la linea.
const XC::Edge *XC::CmbEdge::Lado::Borde(void) const
  { return edge; }
//! @brief Asigna la línea.
void XC::CmbEdge::Lado::SetEdge(Edge *l)
  { edge= l; }
//! @brief Devuelve un puntero (constante) al punto origen de la línea.
const XC::Pnt *XC::CmbEdge::Lado::P1(void) const
  {
    if(!edge) return nullptr;
    if(directo)
      return edge->P1();
    else
      return edge->P2();
  }
//! @brief Devuelve un puntero (constante) al punto destino de la línea.
const XC::Pnt *XC::CmbEdge::Lado::P2(void) const
  {
    if(!edge) return nullptr;
    if(directo)
      return edge->P2();
    else
      return edge->P1();
  }


//! @brief Devuelve verdadero si los puntos que se pasan como parámetro
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

//! @brief Devuelve el nombre de la línea.
const std::string &XC::CmbEdge::Lado::GetNombre(void) const
  { return edge->GetNombre(); }

//! @brief Devuelve la longitud del lado.
double XC::CmbEdge::Lado::getLongitud(void) const
  { return edge->getLongitud(); }

//! @brief Devuelve un vector tangente al lado en el punto s
const XC::Vector &XC::CmbEdge::Lado::getTang(const double &s) const
  { return edge->getTang(s); }

//! @brief Devuelve las posiciones de la linea.
MatrizPos3d XC::CmbEdge::Lado::get_posiciones(void) const
  {
    if(edge)
      return edge->get_posiciones();
    else
      return MatrizPos3d();
  }

//! @brief Devuelve los identificadores de los nodos en sentido directo.
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

//! @brief Devuelve las posiciones de los nodos en sentido directo.
MatrizPos3d XC::CmbEdge::Lado::GetPosNodosDir(void) const
  {
    if(directo)
      return edge->GetPosNodosDir();
    else
      return edge->GetPosNodosInv();
  }

//! @brief Devuelve las posiciones de los nodos en sentido inverso.
MatrizPos3d XC::CmbEdge::Lado::GetPosNodosInv(void) const
  {
    if(directo)
      return edge->GetPosNodosInv();
    else
      return edge->GetPosNodosDir();
  }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el principio.
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

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el final.
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

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro.
XC::Node *XC::CmbEdge::Lado::GetNodo(const size_t &i)
  {
    XC::Node *retval= nullptr;
    if(edge)
      {
        if(directo)
          retval=  edge->GetNodo(i);
        else
          {
            const size_t n= edge->NumNodos();
            retval=  edge->GetNodo(n-i+1);
          }
      }
    return retval;
  }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro.
const XC::Node *XC::CmbEdge::Lado::GetNodo(const size_t &i) const
  {
    if(!edge) return nullptr;
    const size_t n= edge->NumNodos();
    if(directo)
      return edge->GetNodo(i);
    else
      return edge->GetNodo(n-i);
  }

//! @brief Genertes a mesh from the corresponding line.
void XC::CmbEdge::Lado::genMesh(meshing_dir dm)
  { edge->genMesh(dm); }

//! @brief Asigna el número de divisiones de la línea.
void XC::CmbEdge::Lado::SetNDiv(const size_t &nd)
  { edge->SetNDiv(nd); }

//! @brief Devuelve el número de divisiones de la línea.
size_t XC::CmbEdge::Lado::NDiv(void) const
  { return edge->NDiv(); }

//! @brief Devuelve el identificador de la línea.
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
//! @param nombre: Identificador del objeto.
//! @param m: Apuntador al preprocesador.
//! @param nd: Número de divisiones.
XC::CmbEdge::CmbEdge(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : Edge(nombre,m,nd), lineas(0) {}

//! @brief Constructor virtual.
XC::SetEstruct *XC::CmbEdge::getCopy(void) const
  { return new CmbEdge(*this); }


//! @brief Devuelve un puntero al primer lado.
XC::CmbEdge::Lado *XC::CmbEdge::primera_linea(void)
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.begin());
  }
//! @brief Devuelve un puntero al primer lado.
const XC::CmbEdge::Lado *XC::CmbEdge::primera_linea(void) const
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.begin());
  }
//! @brief Devuelve un puntero al último lado.
XC::CmbEdge::Lado *XC::CmbEdge::ultima_linea(void)
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.rbegin());
  }
//! @brief Devuelve un puntero al último lado.
const XC::CmbEdge::Lado *XC::CmbEdge::ultima_linea(void) const
  {
    if(lineas.empty())
      return nullptr;
    else
      return &(*lineas.rbegin());
  }

//! @brief Devuelve un puntero al primer punto.
const XC::Pnt *XC::CmbEdge::primer_punto(void) const
  {
    const Lado *pl= primera_linea();
    if(pl)
      return pl->P1();
    else
      return nullptr;
   }

//! @brief Devuelve un puntero al último punto.
const XC::Pnt *XC::CmbEdge::ultimo_punto(void) const
  {
    const Lado *ul= ultima_linea();
    if(ul)
      return ul->P2();
    else
      return nullptr;
   }

//! @brief Devuelve un apuntador (constante) al punto inicial.
const XC::Pnt *XC::CmbEdge::P1(void) const
  { return primer_punto(); }
//! @brief Devuelve un apuntador (constante) al punto final.
const XC::Pnt *XC::CmbEdge::P2(void) const
  { return ultimo_punto(); }

XC::Pnt *XC::CmbEdge::P1(void)
  { return const_cast<Pnt *>(primer_punto()); }
XC::Pnt *XC::CmbEdge::P2(void)
  { return const_cast<Pnt *>(ultimo_punto()); }

void XC::CmbEdge::reverse(void)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).reverse();
  }

//! @brief Devuelve la longitud de la línea.
double XC::CmbEdge::getLongitud(void) const
  {
    double retval= 0;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      retval+= (*i).getLongitud();
    return retval;
  }

//! @brief Devuelve el número de divisiones total.
size_t XC::CmbEdge::NDiv(void) const
  {
    size_t &nd= const_cast<size_t &>(ndiv);
    nd= 0;
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      nd+= (*i).NDiv();
    return ndiv;
  }

//! @brief Establece el número de divisiones total.
void XC::CmbEdge::SetNDiv(const size_t &nd)
  {
    const size_t nl= NumEdges();
    if(nl>0)
      {
        const size_t resto= nd % nl;
        if(resto != 0)
          std::clog << "XC::CmbEdge::SetNDiv; el número de divisiones ("
                    << nd << ") no es múltiplo del número de segmentos ("
                    << nl << ")." << std::endl;
        const size_t q= nd/nl;
        for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
          (*i).SetNDiv(q);
      }
    else
      std::cerr << "XC::CmbEdge::SetNDiv; no se han definidos segmentos." << std::endl;
  }

MatrizPos3d XC::CmbEdge::get_posiciones(void) const
  {
    const size_t npos= NDiv()+1; //Número de posiciones.
    MatrizPos3d retval(npos);
    if(!lineas.empty())
      {
        size_t cont= 1;
        for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
          {
            const Edge *e= (*i).Borde();
            MatrizPos3d tmp= e->get_posiciones();
            const size_t sz= tmp.size()-1; //La última no se agrega.
            for(size_t i=1;i<sz;i++)
              {
                retval(cont)= tmp(i);
                cont++;
              }
          }
        retval(npos)= ultimo_punto()->GetPos();
      }
    return retval;
  }


//! @brief Lanza la creación de nodos de las líneas.
void XC::CmbEdge::crea_nodos_lineas(void)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).Borde()->crea_nodos();
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
    if(verborrea>3)
      std::clog << "Meshing CmbEdge...(" << GetNombre() << ")...";
    line_meshing(dm);
    
    //Punteros a nodos.
    nodos= TritrizPtrNod(1,NDiv()+1,1);
    size_t offset_j= 0;// Columna inicial.
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        nodos.PutCaja(0,offset_j,0,(*i).Borde()->GetTtzNodos());
        offset_j+= (*i).Borde()->GetNumFilasNodos()-1;
      }
    //Punteros a elementos.
    elementos= TritrizPtrElem(1,NDiv(),1);
    offset_j= 0;// Columna inicial.
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        elementos.PutCaja(0,offset_j,0,(*i).Borde()->GetTtzElementos());
        offset_j+= (*i).Borde()->GetNumFilasElementos()-1;
      }
    if(verborrea>3)
      std::clog << "hecho." << std::endl;
  }

//! @brief Crea e inserta las líneas a partir de los índices que se pasan
//! como parámetro.
void XC::CmbEdge::addLines(const ID &indices_lineas)
  {
    const size_t nl= indices_lineas.Size(); //No. de índices leídos.
    for(size_t i= 0;i<nl;i++)
      inserta(indices_lineas(i));
  }
 
//! @brief Crea e inserta las líneas a partir de los puntos cuyos índices se pasan
//! como parámetro.
void XC::CmbEdge::addPoints(const ID &indices_ptos)
  {
    const size_t np= indices_ptos.Size(); //No. de índices leídos.
    if(np>=2)
      for(size_t i= 1;i<np;i++)
        {
          Pnt *pA= BuscaPnt(indices_ptos(i-1));
          if(!pA)
	    std::cerr << "CmbEdge::addPoints; no se encontró el punto de índice: "
                      << indices_ptos(i-1) << " para definir la superficie: '"
                      << GetNombre() << "'" << std::endl;
          Pnt *pB= BuscaPnt(indices_ptos(i));
          if(!pB)
	    std::cerr << "CmbEdge::addPoints; no se encontró el punto de índice: "
                      << indices_ptos(i) << " para definir la superficie: '"
                      << GetNombre() << "'" << std::endl;
          NuevaLinea(pA,pB);
        }
    else
      std::cerr << "CmbEdge::puntos; error se necesitan al menos dos puntos." << std::endl;
  }

//! @brief Cierra la línea.
void XC::CmbEdge::cierra(void)
  {
    //Cierra la polilinea.
    Pnt *pA= P2();
    if(!pA)
      std::cerr << "CmbEdge::cierra; no se encontró el último punto." << std::endl;
    Pnt *pB= P1();
    if(!pB)
      std::cerr << "CmbEdge::cierra; no se encontró el primer punto " << std::endl;
    if(pA!=pB)
      NuevaLinea(pA,pB);
    else
      std::cerr << "CmbEdge::cierra; la línea ya está cerrada." << std::endl;
  }

//! @brief Crea una nueva línea entre los puntos que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::CmbEdge::NuevaLinea(Pnt *pA,Pnt *pB)
  {
    Linea *retval= nullptr;
    assert(get_preprocessor());
    retval= dynamic_cast<Linea *>(get_preprocessor()->getCad().getLineas().Line(pA,pB));
    if(retval)
      {
        inserta(retval);
        if(lineas.size()==1) //Es la primera línea.
          {
            if(pA!=lineas[0].P1())
              lineas[0].reverse();
          }
      }    
    else
       std::cerr << "CmbEdge::NuevaLinea; no se pudo obtener una linea"
                 << " entre los puntos: " << pA->GetNombre()
                 << " y " << pB->GetNombre() << " para definir la superficie: '"
                 << GetNombre() << "'" << std::endl;
    return retval;
  }

//! @brief Crea un nuevo arco de circunferencia entre los puntos que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::CmbEdge::NuevaLinea(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    ArcoCircunf *retval= nullptr;
    assert(get_preprocessor());
    retval= dynamic_cast<ArcoCircunf *>(get_preprocessor()->getCad().getLineas().Arc(pA,pB,pC));
    if(retval)
      inserta(retval);
    else
      std::cerr << "CmbEdge::NuevaLinea; no se pudo obtener un arco de circunferencia"
                << " entre los puntos: " << pA->GetNombre() << ", " << pB->GetNombre()
                << " y " << pC->GetNombre() << " para definir la superficie: '"
                << GetNombre() << "'" << std::endl;
    return retval;
  }

//! @brief Inserta (si la encuentra) la línea cuyo índice se pasa como parámetro.
void XC::CmbEdge::inserta(const size_t &i)
  {
    Edge *tmp= BuscaEdge(i);
    if(!tmp)
      std::cerr << "CmdEdge; no se encontró la línea de índice: '" 
                    << i << "' \n";
    else
      inserta(tmp);
  }

//! @brief Inserta la línea cuyo apuntador se pasa como parámetro.
void XC::CmbEdge::inserta(Edge *l)
  {
    if(IndiceEdge(l)!= 0) //La línea ya pertenece al conjunto.
      std::cerr << "CmdEdge; la línea ya pertenece al conjunto, no se vuelve a agregar.\n";
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
              std::cerr << "CmbEdge::inserta; line: '" << l->GetNombre()
			<< "' which ends are: '" << l->P1()->GetNombre() << "' y '"
                        << l->P2()->GetNombre() 
                        << "' can't connect to the combination: " << Nombre() 
			<< "' which ends are: '" << P1()->GetNombre() << "' y '"
                        << P2()->GetNombre() 
                        << " because they don't have shared ends." << std::endl;
          }
        actualiza_topologia();
      }
  }

//! @brief Busca la línea l entre las de esta línea compuesta.
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

//! @brief Devuelve un lado de la línea compuesta.
const XC::CmbEdge::Lado *XC::CmbEdge::GetLado(const size_t &i) const
  { return &lineas[i-1]; }

//! @brief Devuelve un lado de la línea compuesta.
XC::CmbEdge::Lado *XC::CmbEdge::GetLado(const size_t &i)
  { return &lineas[i-1]; }

//! @brief Devuelve el lado que tiene por extremos los puntos
//! que se pasan como parámetros.
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

//! @brief Devuelve el lado que tiene por extremos los puntos
//! que se pasan como parámetros.
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

//! @brief Devuelve el lado que tiene por extremos los puntos
//! que se pasan como parámetros.
const XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const size_t &idP1,const size_t &idP2) const
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return GetLadoPorPuntos(p1,p2);
  }

//! @brief Devuelve el lado que tiene por extremos los puntos
//! que se pasan como parámetros.
XC::CmbEdge::Lado *XC::CmbEdge::GetLadoPorPuntos(const size_t &idP1,const size_t &idP2)
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return GetLadoPorPuntos(p1,p2);
  }

//! @brief Devuelve la lista de líneas que definen los lados.
std::deque<XC::Edge *> XC::CmbEdge::GetEdges(void)
  {
    std::deque<XC::Edge *> retval;
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      retval.push_back((*i).Borde());
    return retval;
  }

//! @brief Devuelve el vértice cuyo índice se pasa como parámetro.
const XC::Pnt *XC::CmbEdge::GetVertice(const size_t &i) const
  {
    if(i<NumVertices())
      return GetLado(i)->P1();
    else
      return P2();
  }


void XC::CmbEdge::SetVertice(const size_t &,Pnt *)
  {
    std::cerr << "Función XC::CmbEdge::SetVertice no implementada." << std::endl;
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


//! @brief Devuelve el BND del objeto.
BND3d XC::CmbEdge::Bnd(void) const
  { 
    BND3d retval;
    const size_t nv= NumVertices();
    if(nv<1) //El conjunto está vacío.
      {
	std::cerr << "XC::CmbEdge::Bnd(); la polilinea está vacia." << std::endl;
        return retval;
      }
    if(nv<2)
      {
	std::cerr << "XC::CmbEdge::Bnd(); la polilinea sólo tiene un punto." << std::endl;
        retval= BND3d(GetVertice(1)->GetPos(),GetVertice(1)->GetPos());
        return retval;
      }
    retval= BND3d(GetVertice(1)->GetPos(),GetVertice(2)->GetPos());
    for(size_t i=3;i<=nv;i++)
      retval+= GetVertice(i)->GetPos();
    return retval;
  }
