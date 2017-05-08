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
//KDTreeNodes.cc

#include "KDTreeNodes.h"
#include "Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
XC::NodePos::NodePos(const Node &n)
  : KDTreePos(n.getInitialPosition3d()), nodPtr(&n) {}

//! @brief Constructor.
XC::NodePos::NodePos(const Pos3d &p)
  : KDTreePos(p), nodPtr(nullptr) {}
  
//! @brief Constructor.
XC::KDTreeNodes::KDTreeNodes(void)
  : tree_type(std::ptr_fun(NodePos::tac)), pend_optimizar(0) {}


void XC::KDTreeNodes::insert(const Node &n)
  {
    tree_type::insert(n);
  }

void XC::KDTreeNodes::erase(const Node &n)
  {
    tree_type::erase(n);
    pend_optimizar++;
    if(pend_optimizar>=10)
      {
	tree_type::optimise();
        pend_optimizar= 0;
      }
  }

void XC::KDTreeNodes::clear(void)
  { tree_type::clear(); }
  
//! @brief Returns the node closest to the position being passed as parameter.
const XC::Node *XC::KDTreeNodes::getNearestNode(const Pos3d &pos) const
  {
    const Node *retval= nullptr;
    NodePos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target);
    if(found.first != end())
      retval= found.first->getNodePtr();
    return retval;
  }

//! @brief Returns the node closest to the position being passed as parameter.
const XC::Node *XC::KDTreeNodes::getNearestNode(const Pos3d &pos, const double &r) const
  {
    const Node *retval= nullptr;
    NodePos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target,r);
    if(found.first != end())
      retval= found.first->getNodePtr();
    return retval;
  }
