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

#include "ElementEdge.h"
#include "Element.h"
#include "domain/mesh/node/Node.h"
#include <iostream>
#include "domain/mesh/MeshEdge.h"

//! @brief Constructor.
XC::ElementEdge::ElementEdge(Element *eptr,const int &i)
  :elem(eptr),iedge(i) {}

//! @brief Returns a pointer to the element that "owns" the edge.
const XC::Element *XC::ElementEdge::getElementPtr(void) const
  { return elem; }

//! @brief Returns the index of the edge in its owner element.
const int &XC::ElementEdge::getEdgeIndex(void) const
  { return iedge; }

//! @brief Get the the element indexes of the edge nodes. 
XC::ID XC::ElementEdge::getLocalIndexNodes(void) const
  { return elem->getLocalIndexNodesEdge(iedge); }

//! @brief Returns the edge nodes. 
XC::ElementEdge::NodesEdge XC::ElementEdge::getNodes(void) const
  { return elem->getNodesEdge(iedge); }

//! @brief Returns the corresponding mesh edge. 
XC::MeshEdge XC::ElementEdge::getMeshEdge(void) const
  { return MeshEdge(getNodes()); }

