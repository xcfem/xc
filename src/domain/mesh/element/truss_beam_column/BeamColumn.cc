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
//BeamColumn.cc

#include "BeamColumn.h"


#include <material/section/PrismaticBarCrossSection.h>


//! @brief Constructor.
XC::BeamColumn::BeamColumn(int tag, int classTag)
  : Element1D(tag,classTag)
  {}

//! @brief Constructor.
XC::BeamColumn::BeamColumn(int tag, int classTag, int Nd1, int Nd2)
  : Element1D(tag,classTag, Nd1, Nd2)
  {}

//! @brief Returns a pointer to the i-th section of the element.
const XC::PrismaticBarCrossSection *XC::BeamColumn::getSectionPtr(const size_t &i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; not implemented yet.\n";
    return nullptr;
  }

