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
//JRowSet.h

#ifndef JROWSET_H
#define JROWSET_H

#include "RowSet.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrNod.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrElem.h"

class RangoIndice;

namespace XC {

class EntMdlr;

//!  @ingroup Set
//! 
//!  @brief Objects in a row.
//! 
//!  A JRowSet objects contains 0 or more:
//!  - Nodes.
//!  - Finite elements.
//!  that correspond to a row of an EntMdlr object.
class JRowSet: public RowSet<TritrizPtrNod::var_ref_j_row,TritrizPtrElem::var_ref_j_row>
  {
  public:
    typedef TritrizPtrNod::var_ref_j_row tNodeRow;
    typedef TritrizPtrElem::var_ref_j_row tElemRow;
    JRowSet(EntMdlr &e,const size_t &f=1,const size_t &c=1,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    JRowSet(EntMdlr &e,const size_t &layer,const RangoIndice &,const size_t &c,const std::string &nmb="",Preprocessor *preprocessor= nullptr);  };
} //end of XC namespace
#endif
