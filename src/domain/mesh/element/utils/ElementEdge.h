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

#ifndef ElementEdge_h
#define ElementEdge_h

#include <cstdlib>
#include "domain/mesh/element/Element.h"

namespace XC {

class MeshEdge;

//! \ingroup FEMisc
//
//! @brief Element edge (TO DEPRECATE?? LP 7.02.2017).
class ElementEdge: public EntCmd
  {
    Element *elem; //! Element that owns this border.
    int iedge; //! edge index.
  public:
    typedef Element::NodesEdge NodesEdge; //!< Nodos de un borde del elemento.
    typedef std::set<const Element *> ElementConstPtrSet;

    ElementEdge(Element *eptr= nullptr,const int &i=-1);
    const Element *getElementPtr(void) const;
    const int &getEdgeIndex(void) const; 
    ID getLocalIndexNodes(void) const;
    NodesEdge getNodes(void) const;
    MeshEdge getMeshEdge(void) const;    
  };
} // end of XC namespace

#endif

