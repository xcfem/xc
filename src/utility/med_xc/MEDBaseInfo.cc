//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDBaseInfo

#include "MEDBaseInfo.h"



#include "domain/mesh/Mesh.h"
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! Indices deletion.
void XC::MEDBaseInfo::clear(void)
  { indices.clear(); }

//! @brief Appends i to the indices.
void XC::MEDBaseInfo::new_tag(size_t i)
  { indices.new_tag(i); }

//! @brief Returns a const reference to the indices container.
const XC::MEDMapIndices &XC::MEDBaseInfo::getMapIndices(void) const
  { return indices; }

