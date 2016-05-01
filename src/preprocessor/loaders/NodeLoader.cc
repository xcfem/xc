//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//NodeLoader.cc

#include "NodeLoader.h"
#include "preprocessor/cad/SisRef.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

#include "boost/any.hpp"

#include "domain/mesh/element/Element.h"
#include "utility/tagged/DefaultTag.h"

void XC::NodeLoader::libera(void)
  {
    if(nodo_semilla) delete nodo_semilla;
    nodo_semilla= nullptr; 
  }

XC::NodeLoader::NodeLoader(Preprocessor *preprocessor)
  : Loader(preprocessor), ngdl_def_nodo(2),ncoo_def_nodo(3),nodo_semilla(nullptr) {}

//! @brief Destructor.
XC::NodeLoader::~NodeLoader(void)
  { libera(); }

//! @brief Devuelve el tag por defecto para el siguiente nodo.
int XC::NodeLoader::getDefaultTag(void) const
  { return Node::getDefaultTag().getTag(); }

//! @brief Asigna el tag por defecto para el siguiente nodo.
void XC::NodeLoader::setDefaultTag(const int &tag)
  { Node::getDefaultTag().setTag(tag); }

//! @brief Borra todos los nodos y 
void XC::NodeLoader::clearAll(void)
  {
    libera();
    setDefaultTag(0);
  }

XC::Node *XC::NodeLoader::new_node(const int &tag,const size_t &dim,const int &ngdl,const double &x,const double &y,const double &z)
  {
    Node *retval= nullptr;
    switch(dim)
      {
      case 1:
        retval= new Node(tag,ngdl,x);
        break;
      case 2:
        retval= new Node(tag,ngdl,x,y);
        break;
      default:
        retval= new Node(tag,ngdl,x,y,z);
        break;
      }
    return retval;
  }

XC::Node *XC::NodeLoader::duplicateNode(const int &tagNodoOrg)
  {
    Node *retval= nullptr;
    Node *ptr_nodo_org= getDomain()->getNode(tagNodoOrg);
    if(!ptr_nodo_org)
      std::cerr << "Preprocessor::nuevoNodo; no se encontró el nodo de tag:"
                << tagNodoOrg << std::endl;
    else
      {
        const int ngdl= ptr_nodo_org->getNumberDOF();    
        retval= new Node(getDefaultTag(),ngdl,ptr_nodo_org->getCrds());
        if(retval)
          {
            getDomain()->addNode(retval);
            preprocessor->UpdateSets(retval);
          }
      }
    return retval;
  }

XC::Node *XC::NodeLoader::nuevoNodo(const double &x,const double &y,const double &z)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y,z);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }

XC::Node *XC::NodeLoader::nuevoNodo(const double &x,const double &y)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }


XC::Node *XC::NodeLoader::nuevoNodo(const double &x)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Pos3d &p)
  { return nuevoNodo(p.x(),p.y(),p.z()); }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Pos2d &p)
  { return nuevoNodo(p.x(),p.y()); }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Vector &coo)
  {
    int sz= coo.Size();
    Node *retval= nullptr;
    if(sz==1)
      retval= nuevoNodo(coo[0]);
    else if(sz==2)
      retval= nuevoNodo(coo[0],coo[1]);
    else if(sz>=3)
      retval= nuevoNodo(coo[0],coo[1],coo[2]);
    else
      std::cerr << "NodeLoader::newNodeIDV; vector empty."
                << std::endl;
    return retval;
  }

//! @brief Defines the seed node.
XC::Node *XC::NodeLoader::newSeedNode(void)
  {
    libera();
    nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);
    return nodo_semilla;
  }

XC::Node *XC::NodeLoader::newNodeIDV(const int &tag,const Vector &coo)
  { 
    setDefaultTag(tag);
    return nuevoNodo(coo);
  } 

XC::Node *XC::NodeLoader::newNodeIDXYZ(const int &tag,const double &x,const double &y,const double &z)
  { 
    setDefaultTag(tag);
    Node *retval= nuevoNodo(x,y,z);
    return retval;
  } 

XC::Node *XC::NodeLoader::newNodeIDXY(const int &tag,const double &x,const double &y)
  {
    setDefaultTag(tag);
    return nuevoNodo(x,y);
  } 


//! @brief Crea el nodo cuyo identificador se pasa como parámetro.
XC::Node *XC::NodeLoader::getNode(const int &tag)
  { return getDomain()->getNode(tag); }

//! @brief Calcula las reacciones en los nodos.
void XC::NodeLoader::calculateNodalReactions(bool inclInertia)
  {
    getDomain()->calculateNodalReactions(inclInertia,1e-4);
  }

