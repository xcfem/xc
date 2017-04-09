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
//DqPtrsNode.h
//deque de pointers to node.


#ifndef DQPTRSNODE_H
#define DQPTRSNODE_H

#include "DqPtrs.h"
#include "domain/mesh/node/KDTreeNodes.h"

class Vector3d;
class ExprAlgebra;

namespace XC {
class TrfGeom;

//!  \ingroup Set
//! 
//!  \brief Contenedor de pointers to nodes.
//! 
class DqPtrsNode: public DqPtrs<Node>
  {
    KDTreeNodes kdtreeNodos; //!< space-partitioning data structure for organizing nodes.
  protected:
    void create_arbol(void);
  public:
    typedef DqPtrs<Node>::const_iterator const_iterator;
    typedef DqPtrs<Node>::iterator iterator;
    typedef DqPtrs<Node>::reference reference;
    typedef DqPtrs<Node>::const_reference const_reference;
    typedef DqPtrs<Node>::size_type size_type;

    DqPtrsNode(EntCmd *owr= nullptr);
    DqPtrsNode(const DqPtrsNode &otro);
    explicit DqPtrsNode(const std::deque<Node *> &ts);
    explicit DqPtrsNode(const std::set<const Node *> &ts);
    DqPtrsNode &operator=(const DqPtrsNode &otro);
    void agrega(const DqPtrsNode &otro);
    //void agrega_cond(const DqPtrsNode &otro,const std::string &cond);
    bool push_back(Node *);
    bool push_front(Node *);
    void clearAll(void);
    inline iterator begin(void)
      { return DqPtrs<Node>::begin(); }
    inline iterator end(void)
      { return DqPtrs<Node>::end(); }
    inline const_iterator begin(void) const
      { return DqPtrs<Node>::begin(); }
    inline const_iterator end(void) const
      { return DqPtrs<Node>::end(); }
    void mueve(const Vector3d &);
    void transforma(const TrfGeom &trf);

    size_t getNumLiveNodes(void) const;
    size_t getNumDeadNodes(void) const;

    bool InNodeTag(const int tag_nodo) const;
    bool InNodeTags(const ID &tag_nodos) const;
    std::set<int> getTags(void) const;

    Node *buscaNodo(const int &tag);
    const Node *buscaNodo(const int &tag) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;

    void numera(void);
  };

} //end of XC namespace
#endif

