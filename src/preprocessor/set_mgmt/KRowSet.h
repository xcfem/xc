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
//KRowSet.h

#ifndef KROWSET_H
#define KROWSET_H

#include "RowSet.h"
#include "preprocessor/multi_block_topology/matrices/NodePtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"

class RangoIndice;

namespace XC {

class EntMdlr;

//!  @ingroup Set
//! 
//!  @brief Set of objects in a row.
//! 
//!  A KRowSet object contains zero o more:
//!  - Nodes.
//!  - Finite elements.
//!  that correspond to a k_row of an EntMdlr object.
class KRowSet: public RowSet<NodePtrArray3d::var_ref_k_row,ElemPtrArray3d::var_ref_k_row>
  {
  public:
    typedef NodePtrArray3d::var_ref_k_row tNodeRow;
    typedef ElemPtrArray3d::var_ref_k_row tElemRow;
    KRowSet(EntMdlr &e,const size_t &f=1,const size_t &c=1,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    KRowSet(EntMdlr &e,const size_t &layer,const size_t &f,const RangoIndice &,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
  };
} //end of XC namespace
#endif
