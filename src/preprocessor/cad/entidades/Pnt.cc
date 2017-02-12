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
//Pnt.cc

#include "Pnt.h"
#include "Edge.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/cad/SisRef.h"


//! @brief Constructor.
XC::Pnt::Pnt(Preprocessor *m,const Pos3d &pto)
  : EntMdlr(m), p(pto) {}

//! @brief Constructor.
XC::Pnt::Pnt(const std::string &nombre, Preprocessor *m,const Pos3d &pto)
  : EntMdlr(nombre,0,m), p(pto) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::Pnt::getCopy(void) const
  { return new Pnt(*this); }

//! Inserta la línea being passed as parameter en la lista
//! de líneas que tocan al punto.
void XC::Pnt::inserta_linea(Edge *l) const
  { lineas_pt.insert(l); }

//! Borra la línea being passed as parameter de la lista
//! de líneas que tocan al punto.
void XC::Pnt::borra_linea(Edge *l) const
  {
    std::set<const Edge *>::iterator i= lineas_pt.find(l);
    if(i!= lineas_pt.end()) //La ha encontrado.
      lineas_pt.erase(i);
  }
//! Devuelve el vector de posición del punto.
Vector3d XC::Pnt::VectorPos(void) const
  { return p.VectorPos();  }

//! @brief Actualiza la topología.
void XC::Pnt::actualiza_topologia(void)
  {}

//! @brief Devuelve el BND del objeto.
BND3d XC::Pnt::Bnd(void) const
  { return BND3d(p,p);  }

//! @brief Devuelve las líneas que comienzan o acaban en este punto.
std::set<const XC::Edge *> XC::Pnt::EdgesExtremo(void) const
  {
    std::set<const Edge *> retval;
    if(!lineas_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lineas_pt.begin();
        for(;i!=lineas_pt.end();i++)
          {
            const Edge *l= *i;
            if(Extremo(*l))
              retval.insert(l);
          }
      }
    return retval;
  }

//! @brief Devuelve los nombres de las lineas que tocan al punto.
const std::string &XC::Pnt::NombresEdgesTocan(void) const
  {
    static std::string retval;
    retval= "";
    if(!lineas_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lineas_pt.begin();
        retval+= (*i)->GetNombre();
        for(;i!=lineas_pt.end();i++)
          retval+= "," + (*i)->GetNombre();
      }
    return retval;
  }

//! @brief Devuelve verdadero si el punto toca a la línea.
bool XC::Pnt::Toca(const Edge &l) const
  {
    std::set<const Edge *>::const_iterator i= lineas_pt.find(&l);
    return (i!=lineas_pt.end());
  }

//! @brief Devuelve verdadero si el punto es un extremo de la línea.
bool XC::Pnt::Extremo(const Edge &l) const
  { return l.Extremo(*this); }

//! @brief Devuelve verdadero si el punto toca a la superficie.
bool XC::Pnt::Toca(const Face &s) const
  {
    for(std::set<const Edge *>::const_iterator i= lineas_pt.begin(); i!=lineas_pt.end();i++)
      { if((*i)->Toca(s)) return true; }
    return false;
  }

//! @brief Devuelve verdadero si el punto toca al cuerpo.
bool XC::Pnt::Toca(const Body &b) const
  {
    for(std::set<const Edge *>::const_iterator i= lineas_pt.begin(); i!=lineas_pt.end();i++)
      { if((*i)->Toca(b)) return true; }
    return false;
  }

//! @brief Devuelve el cuadrado de la distancia a la posición being passed as parameter.
double XC::Pnt::DistanciaA2(const Pos3d &pt) const
  { return dist2(p,pt);  }

//! @brief Creates nodes.
void XC::Pnt::crea_nodos(void)
  {
    if(getGenMesh() && (NumNodos()==0))
      {
        MatrizPos3d tmp(1,1,GetPos());
        TritrizPos3d ptos(1,tmp);
        EntMdlr::crea_nodos(ptos);
      }
  }

//! @brief Creates mesh.
void XC::Pnt::genMesh(meshing_dir dm)
  {
    crea_nodos();
  }

//! @brief Returns true if the point owns a node (is meshed).
bool XC::Pnt::tieneNodo(void) const
  {
    bool retval= false;
    if(NumNodos()>0)
      retval= (GetNodo()!= nullptr);
    return retval;      
  }

//! @brief Return node's tag.
int XC::Pnt::getTagNode(void) const
  {
    int retval= -1;
    const Node *nod= GetNodo();
    if(nod)
      retval= nod->getTag();
    else
      std::cerr << "Pnt::getTagNode; the point: '" << GetNombre()
                << "' has not a node (is not meshed)." << std::endl;
    return retval;
  }


//! @brief Return point's node.
XC::Node *XC::Pnt::getNode(void)
  {
    Node *nod= GetNodo();
    if(!nod)
      std::cerr << "Pnt::getTagNode; the point: '" << GetNombre()
                << "' has not a node (is not meshed)." << std::endl;
    return nod;
  }

//! @brief Devuelve los conjuntos a los que pertenece este punto.
std::set<XC::SetBase *> XC::Pnt::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Pnt::get_sets; no se ha definido el preprocesador." << std::endl;
    return retval;
  }

//! @brief Agrega el punto a los conjuntos que se pasan como parámetro.
void XC::Pnt::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetPuntos().push_back(this);
      }
  }

//! @brief Desplaza la posición del punto (sólo esta previsto que se use desde Set).
void XC::Pnt::Mueve(const Vector3d &desplaz)
  {
    p+=desplaz;
    return;
  }

//! @brief Aplica al punto la transformación cuyo índice se pasa como parámetro.
void XC::Pnt::Transforma(const TrfGeom &trf)
  { p= trf.Transforma(p); }

//! @brief Aplica al punto la transformación cuyo índice se pasa como parámetro.
void XC::Pnt::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= get_preprocessor()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

XC::Vector &XC::operator-(const Pnt &b,const Pnt &a)
  {
    static Vector retval(3);
    const Pos3d A= a.GetPos();
    const Pos3d B= b.GetPos();
    retval[0]= B.x()-A.x();
    retval[1]= B.y()-A.y();
    retval[2]= B.z()-A.z();
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::busca_edge_const_ptr_toca(const Pnt &pA,const Pnt &pB)
  {
    const Edge *retval= nullptr;
    const std::set<const Edge *> lineasPA= pA.EdgesTocan();
    for(std::set<const Edge *>::const_iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::busca_edge_const_ptr_toca(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    const Edge *retval= nullptr;
    const std::set<const Edge *> lineasPA= pA.EdgesTocan();
    for(std::set<const Edge *>::const_iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i) && pC.Toca(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::busca_edge_ptr_toca(const Pnt &pA,const Pnt &pB)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesTocan();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::busca_edge_ptr_toca(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesTocan();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i) && pC.Toca(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Busca una línea cuyos extremos son los puntos que se pasan como parámetro.
XC::Edge *XC::busca_edge_ptr_extremos(const Pnt &pA,const Pnt &pB)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesExtremo();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Extremo(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::busca_edge_ptr_extremos(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesExtremo();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i) && pC.Extremo(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Busca una línea cuyos extremos son los puntos que se pasan como parámetro.
const XC::Edge *XC::busca_edge_const_ptr_extremos(const Pnt &pA,const Pnt &pB)
  {
    const Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesExtremo();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Extremo(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::busca_edge_const_ptr_extremos(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    const Edge *retval= nullptr;
    std::set<const Edge *> lineasPA= pA.EdgesExtremo();
    for(std::set<const Edge *>::iterator i= lineasPA.begin();i!=lineasPA.end();i++)
      if(pB.Toca(**i) && pC.Extremo(**i))
        {
          retval= (*i);
          break;
        }
    return retval;
  }
