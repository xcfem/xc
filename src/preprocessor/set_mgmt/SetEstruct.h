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
//----------------------------------------------------------------------------
//SetEstruct.h

#ifndef SETESTRUCT_H
#define SETESTRUCT_H

#include "SetBase.h"
#include <map>
#include "domain/mesh/node/NodeTopology.h"

namespace XC {
class ElemPtrArray3d;
class Node;
class Element;

//!  @ingroup Set
//! 
//!  @brief structured set, i. e. a set that can return 
//!  a pointer a to a node or an element from its indices i,j and k.
class SetEstruct: public SetBase
  {
  protected:
    friend class Preprocessor;
    void add_elements_to_handler(const ElemPtrArray3d &);

    NodePtrSet getNodePtrSet(void);
  public:
    SetEstruct(const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    SetEstruct(const SetEstruct &);
    SetEstruct &operator=(const SetEstruct &);
    virtual SetEstruct *getCopy(void) const= 0;
    virtual size_t getNumNodeLayers(void) const= 0;
    virtual size_t getNumNodeRows(void) const= 0;
    virtual size_t getNumNodeColumns(void) const= 0;
    virtual size_t getNumElementLayers(void) const= 0;
    virtual size_t getNumElementRows(void) const= 0;
    virtual size_t getNumElementColumns(void) const= 0;
    size_t getNumberOfNodes(void) const;
    size_t getNumberOfElements(void) const;
    bool In(const Node *) const;
    bool In(const Element *) const;

    bool isIRow(void) const;
    bool isJRow(void) const;
    bool isKRow(void) const;
    bool isIConstantLayer(void) const;
    bool isJConstantLayer(void) const;
    bool isKConstantLayer(void) const;
    size_t Dimension(void) const;
    std::string getStrType(void) const;

    virtual Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1)= 0;
    virtual const Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1) const= 0;

    Node *getNodeI(const size_t &i);
    Node *getNodeIJ(const size_t &i,const size_t &j);
    inline Node *getNodeIJK(const size_t &i,const size_t &j,const size_t &k)
      { return getNode(i,j,k); }

    virtual Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1)= 0;
    virtual const Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1) const= 0;

    inline Element *getElementI(const size_t &i)
      { return getElement(i,1,1); }
    inline Element *getElementIJ(const size_t &i,const size_t &j)
      { return getElement(i,j,1); }
    inline Element *getElementIJK(const size_t &i,const size_t &j,const size_t &k)
      { return getElement(i,j,k); }

    std::set<int> getNodeTags(void) const;
    boost::python::list getNodes(void);
    std::set<int> getElementTags(void) const;
    boost::python::list getElements(void);
    ElementEdges getElementEdges(void);
    
    void createInertiaLoads(const Vector &);
  };
} //end of XC namespace
#endif
