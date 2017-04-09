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
    if(seed_node) delete seed_node;
    seed_node= nullptr; 
  }

XC::NodeLoader::NodeLoader(Preprocessor *preprocessor)
  : Loader(preprocessor), ngdl_def_nodo(2),ncoo_def_nodo(3),seed_node(nullptr) {}

//! @brief Destructor.
XC::NodeLoader::~NodeLoader(void)
  { libera(); }

//! @brief Returns the default value for next node.
int XC::NodeLoader::getDefaultTag(void) const
  { return Node::getDefaultTag().getTag(); }

//! @brief Sets the default value for next node.
void XC::NodeLoader::setDefaultTag(const int &tag)
  { Node::getDefaultTag().setTag(tag); }

//! @brief Clears all. los nodos y 
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
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; node identified by:"
                << tagNodoOrg << " not found." << std::endl;
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

XC::Node *XC::NodeLoader::newNode(const double &x,const double &y,const double &z)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);

    const size_t dim= seed_node->getDim();
    const int ngdl= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y,z);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }

XC::Node *XC::NodeLoader::newNode(const double &x,const double &y)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0);

    const size_t dim= seed_node->getDim();
    const int ngdl= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }


XC::Node *XC::NodeLoader::newNode(const double &x)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0);

    const size_t dim= seed_node->getDim();
    const int ngdl= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x);
    if(retval)
      {
        getDomain()->addNode(retval);
        preprocessor->UpdateSets(retval);
      }
    return retval;
  }

//! @brief Creates a nede at the position being passed as parameter.
XC::Node *XC::NodeLoader::newNode(const Pos3d &p)
  { return newNode(p.x(),p.y(),p.z()); }

//! @brief Creates a nede at the position being passed as parameter.
XC::Node *XC::NodeLoader::newNode(const Pos2d &p)
  { return newNode(p.x(),p.y()); }

//! @brief Creates a nede at the position being passed as parameter.
XC::Node *XC::NodeLoader::newNode(const Vector &coo)
  {
    int sz= coo.Size();
    Node *retval= nullptr;
    if(sz==1)
      retval= newNode(coo[0]);
    else if(sz==2)
      retval= newNode(coo[0],coo[1]);
    else if(sz>=3)
      retval= newNode(coo[0],coo[1],coo[2]);
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__ << "; vector empty."
                << std::endl;
    return retval;
  }

//! @brief Defines the seed node.
XC::Node *XC::NodeLoader::newSeedNode(void)
  {
    libera();
    seed_node= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);
    return seed_node;
  }

XC::Node *XC::NodeLoader::newNodeIDV(const int &tag,const Vector &coo)
  { 
    setDefaultTag(tag);
    return newNode(coo);
  } 

XC::Node *XC::NodeLoader::newNodeIDXYZ(const int &tag,const double &x,const double &y,const double &z)
  { 
    setDefaultTag(tag);
    Node *retval= newNode(x,y,z);
    return retval;
  } 

XC::Node *XC::NodeLoader::newNodeIDXY(const int &tag,const double &x,const double &y)
  {
    setDefaultTag(tag);
    return newNode(x,y);
  } 


//! @brief Crea el nodo cuyo identificador being passed as parameter.
XC::Node *XC::NodeLoader::getNode(const int &tag)
  { return getDomain()->getNode(tag); }

//! @brief Calcula las reacciones en los nodos.
void XC::NodeLoader::calculateNodalReactions(bool inclInertia)
  {
    getDomain()->calculateNodalReactions(inclInertia,1e-4);
  }

