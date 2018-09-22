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
//ElementEdges.h
                                                                        
#ifndef ElementEdges_h
#define ElementEdges_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include "domain/mesh/element/utils/ElementEdge.h"

namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup FEMisc
//
//! @brief Element edge container.
 class ElementEdges: public CommandEntity, public std::vector<ElementEdge>
  {
  public:
    ElementEdges(void);

    virtual void setPtrs(Domain *theDomain, const ID &theElementTags,const ID &theEdgeIndexes);

    virtual size_t removeElement(const int &tag);

    iterator find(const int &tag);
    const_iterator find(const int &tag) const;
    int getLocElement(const Element *) const;

    const_reference operator()(const size_t &i) const;
    reference operator()(const size_t &i);
  };
} // end of XC namespace

#endif

