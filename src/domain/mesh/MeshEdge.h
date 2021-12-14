// -*-c++-*-
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

#ifndef MeshEdge_h
#define MeshEdge_h

#include "domain/mesh/element/Element.h"

namespace XC {

class DqPtrsElem;
  
//! @ingroup Mesh
//
//! @brief Mesh edge.
class MeshEdge: public CommandEntity
  {
  public:
    typedef Element::NodesEdge NodesEdge; 
  private:
    NodesEdge nodes; //!< Nodes of a mesh edge.
    typedef std::set<const Element *> ElementConstPtrSet;
  public:
    MeshEdge(void);
    MeshEdge(const NodesEdge &);
    NodesEdge getNodes(void) const;
    inline size_t size(void) const
      { return nodes.size(); }
    const Node *getFirstNode(void) const;
    const Node *getLastNode(void) const;
    bool isConnected(const Node &) const;
    bool isConnected(const MeshEdge &) const;
    ElementConstPtrSet getConnectedElements(void) const;
    ElementConstPtrSet getConnectedElements(const DqPtrsElem &) const;
    bool operator==(const MeshEdge &) const;
    void print(std::ostream &) const;
  };

//! @brief Printing stuff.
inline std::ostream &operator<<(std::ostream &os, const MeshEdge &me)
  {
    me.print(os);
    return os;
  }

} // end of XC namespace

#endif

