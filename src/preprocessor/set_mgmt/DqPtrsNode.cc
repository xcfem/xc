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
#include "preprocessor/cad/trf/TrfGeom.h"
#include "xc_basic/src/funciones/algebra/ExprAlgebra.h"

void XC::DqPtrsNode::crea_arbol(void)
  {
    kdtreeNodos.clear();
    for(iterator i= begin();i!=end();i++)
      {
        Node *nPtr= *i;
        assert(nPtr);
        kdtreeNodos.insert(*nPtr);
      }
  }

//! @brief Constructor.
XC::DqPtrsNode::DqPtrsNode(EntCmd *owr)
  : DqPtrs<Node>(owr) {}

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const DqPtrsNode &otro)
  : DqPtrs<Node>(otro)
  { crea_arbol(); }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::deque<Node *> &ts)
  : DqPtrs<Node>(ts)
  { crea_arbol(); }

//! @brief Copy constructor.
XC::DqPtrsNode::DqPtrsNode(const std::set<const Node *> &st)
  : DqPtrs<Node>()
  {
    std::set<const Node *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      push_back(const_cast<Node *>(*k));
  }

//! @brief Assignment operator.
XC::DqPtrsNode &XC::DqPtrsNode::operator=(const DqPtrsNode &otro)
  {
    DqPtrs<Node>::operator=(otro);
    kdtreeNodos= otro.kdtreeNodos;
    return *this;
  }

//! @brief Agrega a ésta lista los elementos de la que se le being passed as parameter.
void XC::DqPtrsNode::agrega(const DqPtrsNode &otro)
  {
    for(register const_iterator i= otro.begin();i!=otro.end();i++)
      push_back(*i);
  }

// //! @brief Adds to this list the elements from the list being passed
// //! as parameter, that fulfill the condition.
// void XC::DqPtrsNode::agrega_cond(const DqPtrsNode &otro,const std::string &cond)
//   {
//     bool result= false;
//     for(register const_iterator i= otro.begin();i!=otro.end();i++)
//       {
//         result= (*i)->interpretaBool(cond);
//         if(result)
// 	  push_back(*i);
//       }
//   }

//! @brief Vacía la lista de pointers y elimina las propiedades que pudiera tener el objeto.
void XC::DqPtrsNode::clearAll(void)
  {
    DqPtrs<Node>::clear();
    kdtreeNodos.clear();
  }

bool XC::DqPtrsNode::push_back(Node *n)
  {
    bool retval= DqPtrs<Node>::push_back(n);
    if(retval)
      kdtreeNodos.insert(*n);
    return retval;
  }

bool XC::DqPtrsNode::push_front(Node *n)
  {
    bool retval= DqPtrs<Node>::push_front(n);
    if(retval)
      kdtreeNodos.insert(*n);
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::DqPtrsNode::getNearestNode(const Pos3d &p)
  {
    Node *retval= const_cast<Node *>(kdtreeNodos.getNearestNode(p));
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::DqPtrsNode::getNearestNode(const Pos3d &p) const
  {
    DqPtrsNode *this_no_const= const_cast<DqPtrsNode *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Desplaza los nodes of the set.
void XC::DqPtrsNode::mueve(const Vector3d &desplaz)
  {
    for(iterator i= begin();i!=end();i++)
      (*i)->Mueve(desplaz);
    crea_arbol();
  }

//! @brief Applies the transformation to the elements of the set.
void XC::DqPtrsNode::transforma(const TrfGeom &trf)
  {
    //Transforma 
    for(iterator i= begin();i!=end();i++)
      (*i)->Transforma(trf);
    crea_arbol();
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
XC::Node *XC::DqPtrsNode::buscaNodo(const int &tag)
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
const XC::Node *XC::DqPtrsNode::buscaNodo(const int &tag) const
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
        ptr->set_indice(idx);
      }
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
