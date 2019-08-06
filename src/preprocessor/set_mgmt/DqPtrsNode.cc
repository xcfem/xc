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

#include "DqPtrsNode.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "xc_utils/src/functions/algebra/ExprAlgebra.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"

//! @brief Constructor.
XC::DqPtrsNode::DqPtrsNode(CommandEntity *owr)
  : DqPtrsKDTree<Node,KDTreeNodes>(owr) {}

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const DqPtrsNode &other)
  : DqPtrsKDTree<Node,KDTreeNodes>(other)
  { }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::deque<Node *> &ts)
  : DqPtrsKDTree<Node,KDTreeNodes>(ts)
  { }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::set<const Node *> &st)
  : DqPtrsKDTree<Node,KDTreeNodes>(st)
  {}

//! @brief Assignment operator.
XC::DqPtrsNode &XC::DqPtrsNode::operator=(const DqPtrsNode &other)
  {
    DqPtrsKDTree<Node,KDTreeNodes>::operator=(other);
    return *this;
  }

//! @brief += operator.
XC::DqPtrsNode &XC::DqPtrsNode::operator+=(const DqPtrsNode &other)
  {
    extend(other);
    return *this;
  }

//! @brief Desplaza los nodes of the set.
void XC::DqPtrsNode::move(const Vector3d &desplaz)
  {
    for(iterator i= begin();i!=end();i++)
      (*i)->Move(desplaz);
    create_tree();
  }

//! @brief Applies the transformation to the elements of the set.
void XC::DqPtrsNode::transforma(const TrfGeom &trf)
  {
    //Transforma 
    for(iterator i= begin();i!=end();i++)
      (*i)->Transform(trf);
    create_tree();
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
XC::Node *XC::DqPtrsNode::findNode(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
const XC::Node *XC::DqPtrsNode::findNode(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Returns the number of nodes of the set which are active.
size_t XC::DqPtrsNode::getNumLiveNodes(void) const
  {
    size_t retval= 0;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isAlive()) retval++;
      }
    return retval;
  }

//! @brief Returns the number of nodes of the set which are inactive.
size_t XC::DqPtrsNode::getNumDeadNodes(void) const
  {
    size_t retval= 0;
    const Node *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isDead()) retval++;
      }
    return retval;
  }

//!  @brief Set indices to the objects to allow its use in VTK.
void XC::DqPtrsNode::numera(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Node *ptr= *i;
        ptr->set_index(idx);
      }
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void  XC::DqPtrsNode::createInertiaLoads(const Vector &accel)
  {
    for(iterator i= begin();i!=end();i++)
      (*i)->createInertiaLoad(accel);
  }

//! @brief Returns true if the node identified by the tag
//! being passed as parameter, belongs to the set.
bool XC::DqPtrsNode::InNodeTag(const int tag_node) const
  {
    for(const_iterator i= begin();i!=end();i++)
      if(tag_node == (*i)->getTag()) return true;
    return false;
  }

//! @brief Returns true if the nodes, with the tags
//! are being passed as parameter, belong to the set.
bool XC::DqPtrsNode::InNodeTags(const ID &tag_nodes) const
  {
    const int sz= tag_nodes.Size();
    for(int i=0;i<sz;i++)
      if(!InNodeTag(tag_nodes(i))) return false;
    return true;
  }

//! @brief Returns the tags de los nodes.
std::set<int> XC::DqPtrsNode::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Return a container with the nodes that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::DqPtrsNode XC::DqPtrsNode::pickNodesInside(const GeomObj3d &geomObj, const double &tol)
  {
    DqPtrsNode retval;
    for(iterator i= begin();i!=end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(n->In(geomObj,tol))
	  retval.push_back(n);
      }
    return retval;    
  }

//! @brief Return the nodes current position boundary.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
BND3d XC::DqPtrsNode::Bnd(const double &factor= 1.0) const
  {
    BND3d retval;
    if(!empty())
      {
	const_iterator i= begin();
	const Node *n= (*i);
	const Pos3d p= n->getCurrentPosition3d(factor);
	retval= BND3d(p,p);
	i++;
	for(;i!=end();i++)
	  {
	    const Node *n= (*i);
	    assert(n);
	    retval+= n->getCurrentPosition3d(factor);
	  }
      }
    return retval;    
  }

//! @brief Returns the centroid of the nodes.
//!
//! @param factor: scale factor for the current position
//!                initPos+ factor * nodDisplacement.
Pos3d XC::DqPtrsNode::getCentroid(const double &factor= 1.0) const
  {
    Pos3d retval;
    if(!empty())
      {
	const size_t sz= size();
        Vector3d vectorPos;
	for(const_iterator i= begin();i!=end();i++)
	  {
	    const Node *n= (*i);
	    assert(n);
	    vectorPos+= n->getCurrentPosition3d(factor).VectorPos();
	  }
        retval+= (vectorPos*1/sz);
      }
    return retval;
  }

//! @brief Return the union of both containers.
XC::DqPtrsNode XC::operator+(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the nodes in a that are not in b.
XC::DqPtrsNode XC::operator-(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval;
    for(DqPtrsNode::const_iterator i= a.begin();i!=a.end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(!b.in(n)) //If not in b.
	  retval.push_back(n);
      }
    return retval;
  }

//! @brief Return the nodes in a that are also in b.
XC::DqPtrsNode XC::operator*(const DqPtrsNode &a,const DqPtrsNode &b)
  {
    DqPtrsNode retval;
    for(DqPtrsNode::const_iterator i= a.begin();i!=a.end();i++)
      {
        Node *n= (*i);
        assert(n);
	if(b.in(n)) //If also in b.
	  retval.push_back(n);
      }
    return retval;    
  }
