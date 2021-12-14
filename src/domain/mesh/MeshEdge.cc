//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#include "MeshEdge.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/set_mgmt/DqPtrsElem.h"
#include <iostream>

//! @brief Constructor.
XC::MeshEdge::MeshEdge(void)
  :nodes() {}

//! @brief Constructor.
XC::MeshEdge::MeshEdge(const NodesEdge &ns)
  :nodes(ns) {}

//! @brief Returns the edge nodes. 
XC::MeshEdge::NodesEdge XC::MeshEdge::getNodes(void) const
  { return nodes; }

//! @brief Returns the first node from the edge:
//      x-----+-------+--------x
//      ^
//      |
const XC::Node *XC::MeshEdge::getFirstNode(void) const
  { return nodes.front(); }

//! @brief Returns the last node from the edge:
//      x-----+-------+--------x
//                             ^
//                             |
const XC::Node *XC::MeshEdge::getLastNode(void) const
  { return nodes.back(); }

//! @brief Returns true if the edge is connected to the node.
bool XC::MeshEdge::isConnected(const Node &n) const
  {
    bool retval= false;
    if((getFirstNode()== &n) || (getLastNode()== &n))
      retval= true;
    return retval;
  }

//! @brief Returns true if the edge is connected to the edge being passed as parameter.
bool XC::MeshEdge::isConnected(const MeshEdge &edge) const
  {
    bool retval= false;
    if(isConnected(*edge.getFirstNode()) || isConnected(*edge.getLastNode()))
      retval= true;
    return retval;
  }

//! @brief Returns a deque<Element *> with the elements that share the edge.
XC::MeshEdge::ElementConstPtrSet XC::MeshEdge::getConnectedElements(void) const
  {
    ElementConstPtrSet retval;
    ElementConstPtrSet elementsNode0= getFirstNode()->getConnectedElements();
    ElementConstPtrSet elementsNode1= getLastNode()->getConnectedElements();
    set_intersection(elementsNode0.begin(),elementsNode0.end(),elementsNode1.begin(),elementsNode1.end(),std::inserter(retval,retval.begin()));
    return retval;
  }

//! @brief Returns a deque<Element *> with the elements
//! from the set that share the edge.
XC::MeshEdge::ElementConstPtrSet XC::MeshEdge::getConnectedElements(const DqPtrsElem &elemSet) const
  {
    ElementConstPtrSet retval;
    ElementConstPtrSet tmp= getConnectedElements();
    for(ElementConstPtrSet::const_iterator i= tmp.begin();i!=tmp.end();i++)
      {
	const Element *elemPtr= elemSet.findElement((*i)->getTag());
	if(elemPtr)
	  retval.insert(elemPtr);
      }
    return retval;
  }

//! @brief Prints edge information.
void XC::MeshEdge::print(std::ostream &os) const
  {
    os << "mesh edge; from node: " << getFirstNode()->getTag() 
       << " to node: " << getLastNode()->getTag() << std::endl;
  }

//! @brief Equal operator
bool XC::MeshEdge::operator==(const MeshEdge &other) const
  {
    bool retval= true;
    const size_t sz= size();
    if(sz!= other.size())
      retval= false;
    else
      for(size_t i= 0;i<sz;i++)
        if(nodes[i]!=other.nodes[i])
          {
            retval= false;
            break;
          }
    return retval; 
  }
