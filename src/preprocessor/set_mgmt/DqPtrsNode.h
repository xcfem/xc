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
//DqPtrsNode.h
//deque de pointers to node.


#ifndef DQPTRSNODE_H
#define DQPTRSNODE_H

#include "DqPtrsKDTree.h"
#include "domain/mesh/node/KDTreeNodes.h"

class Vector3d;
class ExprAlgebra;
class GeomObj3d;
class BND3d;

namespace XC {
class TrfGeom;

//!  @ingroup Set
//! 
//!  \brief Pointers to node container.
//! 
class DqPtrsNode: public DqPtrsKDTree<Node,KDTreeNodes>
  {
  public:
    DqPtrsNode(CommandEntity *owr= nullptr);
    DqPtrsNode(const DqPtrsNode &);
    explicit DqPtrsNode(const std::deque<Node *> &);
    explicit DqPtrsNode(const std::set<const Node *> &);
    DqPtrsNode &operator=(const DqPtrsNode &);
    DqPtrsNode &operator+=(const DqPtrsNode &);
    void move(const Vector3d &);
    void transforma(const TrfGeom &trf);

    size_t getNumLiveNodes(void) const;
    size_t getNumDeadNodes(void) const;

    bool InNodeTag(const int ) const;
    bool InNodeTags(const ID &) const;
    std::set<int> getTags(void) const;
    DqPtrsNode pickNodesInside(const GeomObj3d &, const double &tol= 0.0);
    BND3d Bnd(const double &) const;
    Pos3d getCentroid(const double &) const;

    Node *findNode(const int &tag);
    const Node *findNode(const int &tag) const;
    void numera(void);

    void createInertiaLoads(const Vector &);
    
    // mass distribution
    Matrix getTotalMass(void) const;
    double getTotalMassComponent(const int &) const;
  };

DqPtrsNode operator+(const DqPtrsNode &a,const DqPtrsNode &b);
DqPtrsNode operator-(const DqPtrsNode &a,const DqPtrsNode &b);
DqPtrsNode operator*(const DqPtrsNode &a,const DqPtrsNode &b);

} //end of XC namespace
#endif

