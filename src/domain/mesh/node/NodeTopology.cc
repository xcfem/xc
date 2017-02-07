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

#include "NodeTopology.h"
#include "Node.h"
#include "domain/mesh/element/utils/ElementEdges.h"
#include "domain/mesh/element/Element.h"

//! @brief Returns the elements that are connected to both nodes.
std::set<XC::Element *> XC::getElementosEntreNodos(Node &n1, Node &n2)
  {
    const std::set<Element *> set1= n1.getConnectedElements();
    const std::set<Element *> set2= n2.getConnectedElements();
    std::set<Element *> retval;
    //std::set<const Element *>::const_iterator iter= set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),retval.begin());
    for(std::set<Element *>::const_iterator i= set1.begin();i!=set1.end();i++)
      {
        std::set<Element *>::const_iterator j= set2.find(*i);
        if(*i == *j)
          retval.insert(*i);
      }
    return retval;
  }

//! @brief Devuelve los bordes de elemento que tienen por extremos
//! a ambos nodos.
XC::ElementEdges XC::getElementEdgesEntreNodos(Node *n1, Node *n2)
  {
    ElementEdges retval;
    if(n1 && n2)
      {
        const std::set<Element *> elems= getElementosEntreNodos(*n1,*n2);
        for(std::set<Element *>::const_iterator i= elems.begin();i!=elems.end();i++)
          {
            const int edge= (*i)->getEdgeNodes(n1,n2);
            if(edge>=0)
              retval.push_back(ElementEdge(*i,edge));
          }
      }
    return retval;
  }

//! @brief Returns the elements connected to any of the set nodes.
std::set<XC::Element *> XC::getConnectedElements(const NodePtrSet &nodos)
  {
    std::set<Element *> retval;
    for(NodePtrSet::const_iterator i= nodos.begin();i!=nodos.end();i++)
      {
        std::set<Element *> tmp= (*i)->getConnectedElements();
        retval.insert(tmp.begin(),tmp.end());
      }
    return retval;
  }
//! @brief Devuelve los bordes de elemento que tienen ambos extremos
//! en el conjunto de nodos being passed as parameter.
XC::ElementEdges XC::getElementEdgesEntreNodos(const NodePtrSet &nodos)
  {
    ElementEdges retval;
    std::set<Element *> elems= getConnectedElements(nodos);
    for(std::set<Element *>::const_iterator i= elems.begin();i!=elems.end();i++)
      {
        Element *elem_ptr= *i;
        const std::set<int> idEdges= elem_ptr->getEdgesNodes(nodos);
        for(std::set<int>::const_iterator j= idEdges.begin();j!=idEdges.end();j++)
          retval.push_back(ElementEdge(elem_ptr,*j));
      }
    return retval;
  }

//! @brief Devuelve verdadero si los punteros a nodo del vector
//! están contenidos en el conjunto being passed as parameter.
bool XC::in(const NodePtrSet &nodos,const std::vector<const Node *> &nodos_edge)
  {
    bool retval= true;
    for(std::vector<const Node *>::const_iterator i= nodos_edge.begin();i!=nodos_edge.end();i++)
      {
        if(find(nodos.begin(),nodos.end(),*i)==nodos.end())
          {
            retval= false;
            break;
          }
      }
    return retval;
  }
