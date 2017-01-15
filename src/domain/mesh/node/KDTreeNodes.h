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
//KDTreeNodes.h
#ifndef KDTreeNodes_h
#define KDTreeNodes_h

#include "xc_utils/src/geom/pos_vec/KDTreePos.h"
#include "xc_basic/src/kdtree++/kdtree.hpp"

class Pos3d;

namespace XC {
class Node;

//! \ingroup Nod
//
//! @brief Posición de un nodo para el KDTree.
class NodePos: public KDTreePos
  {
  private:
    const Node *nodPtr;
  public:
    NodePos(const Node &);
    explicit NodePos(const Pos3d &p);
    inline const Node *getNodePtr(void) const
      { return nodPtr; }
    static inline double tac( NodePos p, size_t k ) { return p[k]; }
  };

inline bool operator==(const NodePos &A,const NodePos &B)
  { return ((A.getNodePtr()== B.getNodePtr()) && (A[0] == B[0]) && (A[1] == B[1]) && (A[2] == B[2])); }


class KDTreeNodes: protected kd_tree::KDTree<3, NodePos, std::pointer_to_binary_function<NodePos,size_t,double> >
  {
    size_t pend_optimizar;
  public:
    typedef kd_tree::KDTree<3, NodePos, std::pointer_to_binary_function<NodePos,size_t,double> > tree_type;
    KDTreeNodes(void);

    void insert(const Node &);
    void erase(const Node &);
    void clear(void);

    const Node *getNearestNode(const Pos3d &pos) const;
    const Node *getNearestNode(const Pos3d &pos, const double &r) const;
  };

} // end of XC namespace 


#endif
