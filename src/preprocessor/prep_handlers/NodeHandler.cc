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
//NodeHandler.cc

#include "NodeHandler.h"
#include "preprocessor/multi_block_topology/ReferenceFrame.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"

#include "boost/any.hpp"

#include "domain/mesh/element/Element.h"
#include "utility/tagged/DefaultTag.h"

void XC::NodeHandler::free_mem(void)
  {
    if(seed_node) delete seed_node;
    seed_node= nullptr; 
  }

XC::NodeHandler::NodeHandler(Preprocessor *preprocessor)
  : PrepHandler(preprocessor), ndof_def_node(2),ncoo_def_node(3),seed_node(nullptr) {}

//! @brief Destructor.
XC::NodeHandler::~NodeHandler(void)
  { free_mem(); }

//! @brief Return the default value for next node.
int XC::NodeHandler::getDefaultTag(void) const
  { return Node::getDefaultTag().getTag(); }

//! @brief Set the default value for next node.
void XC::NodeHandler::setDefaultTag(const int &tag)
  { Node::getDefaultTag().setTag(tag); }

//! @brief Clear all nodes.
void XC::NodeHandler::clearAll(void)
  {
    free_mem();
    setDefaultTag(0);
  }

XC::Node *XC::NodeHandler::new_node(const int &tag,const size_t &dim,const int &ndof,const double &x,const double &y,const double &z)
  {
    Node *retval= nullptr;
    switch(dim)
      {
      case 1:
        retval= new Node(tag,ndof,x);
        break;
      case 2:
        retval= new Node(tag,ndof,x,y);
        break;
      default:
        retval= new Node(tag,ndof,x,y,z);
        break;
      }
    return retval;
  }

//! @brief Create a duplicate copy of the node whose tag is passed as parameter
XC::Node *XC::NodeHandler::duplicateNode(const int &orgNodeTag)
  {
    Node *retval= nullptr;
    Node *org_node_ptr= getDomain()->getNode(orgNodeTag);
    if(!org_node_ptr)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; node identified by:"
                << orgNodeTag << " not found." << std::endl;
    else
      {
        const int ndof= org_node_ptr->getNumberDOF();    
        retval= new Node(getDefaultTag(),ndof,org_node_ptr->getCrds());
        if(retval)
          {
            getDomain()->addNode(retval);
            getPreprocessor()->UpdateSets(retval);
          }
      }
    return retval;
  }

XC::Node *XC::NodeHandler::newNode(const double &x,const double &y,const double &z)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_node,ndof_def_node,0.0,0.0,0.0);

    const size_t dim= seed_node->getDim();
    const int ndof= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ndof,x,y,z);
    if(retval)
      {
        getDomain()->addNode(retval);
        getPreprocessor()->UpdateSets(retval);
      }
    return retval;
  }

XC::Node *XC::NodeHandler::newNode(const double &x,const double &y)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_node,ndof_def_node,0.0,0.0);

    const size_t dim= seed_node->getDim();
    const int ndof= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ndof,x,y);
    if(retval)
      {
        getDomain()->addNode(retval);
        getPreprocessor()->UpdateSets(retval);
      }
    return retval;
  }


XC::Node *XC::NodeHandler::newNode(const double &x)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!seed_node)
      seed_node= new_node(0,ncoo_def_node,ndof_def_node,0.0);

    const size_t dim= seed_node->getDim();
    const int ndof= seed_node->getNumberDOF();
    Node *retval= new_node(tg,dim,ndof,x);
    if(retval)
      {
        getDomain()->addNode(retval);
        getPreprocessor()->UpdateSets(retval);
      }
    return retval;
  }

//! @brief Create a nede at the position passed as parameter.
XC::Node *XC::NodeHandler::newNode(const Pos3d &p)
  { return newNode(p.x(),p.y(),p.z()); }

//! @brief Creates a nede at the position passed as parameter.
XC::Node *XC::NodeHandler::newNode(const Pos2d &p)
  { return newNode(p.x(),p.y()); }

//! @brief Create a node at the position pointed by the vector passed as parameter.
XC::Node *XC::NodeHandler::newNode(const Vector &coo)
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
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; vector empty."
                << std::endl;
    return retval;
  }

//! @brief Defines the seed node.
XC::Node *XC::NodeHandler::newSeedNode(void)
  {
    free_mem();
    seed_node= new_node(0,ncoo_def_node,ndof_def_node,0.0,0.0,0.0);
    return seed_node;
  }

XC::Node *XC::NodeHandler::newNodeIDV(const int &tag,const Vector &coo)
  { 
    setDefaultTag(tag);
    return newNode(coo);
  } 

//! @brief Create a node whose ID=tag from global coordinates (x,y,z).
XC::Node *XC::NodeHandler::newNodeIDXYZ(const int &tag,const double &x,const double &y,const double &z)
  { 
    setDefaultTag(tag);
    Node *retval= newNode(x,y,z);
    return retval;
  } 

XC::Node *XC::NodeHandler::newNodeIDXY(const int &tag,const double &x,const double &y)
  {
    setDefaultTag(tag);
    return newNode(x,y);
  } 


//! @brief Get the node whose ID is passed as parameter.
XC::Node *XC::NodeHandler::getNode(const int &tag)
  { return getDomain()->getNode(tag); }

//! @brief Calculate nodal reaction forces and moments.
//! @param inclInertia: if True, the unbalance load vector for each node is calculated including inertial forces.
void XC::NodeHandler::calculateNodalReactions(bool inclInertia)
  {
    getDomain()->calculateNodalReactions(inclInertia,1e-4);
  }

