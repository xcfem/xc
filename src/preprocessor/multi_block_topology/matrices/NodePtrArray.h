// -*-c++-*-
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
//NodePtrArray.h
//Node matrix

#ifndef NODEPTRARRAY_H
#define NODEPTRARRAY_H

#include "PtrArrayBase.h"
#include "utility/matrices/m_int.h"
#include <vector>

class Pos3d;

namespace XC{
class Node;
class SFreedom_Constraint;

//! @ingroup MultiBlockTopologyMR
//! 
//! @brief Two-dimensional array of pointers to nodes.
class NodePtrArray: public PtrArrayBase<Node>
  {
  protected:

  public:
    //! @brief Constructor.
    NodePtrArray(const size_t &f=0,const size_t &c=0)
      : PtrArrayBase<Node>(f,c) {}
    m_int getTags(void) const;
    void fix(const SFreedom_Constraint &) const;

    Node *findNode(const int &tag);
    const Node *findNode(const int &tag) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;


  };

} //end of XC namespace

#endif
