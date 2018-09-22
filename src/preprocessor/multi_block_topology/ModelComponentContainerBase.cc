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
//ModelComponentContainerBase.cc

#include "ModelComponentContainerBase.h"

#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::ModelComponentContainerBase::ModelComponentContainerBase(MultiBlockTopology *mbt)
  : CommandEntity(mbt), tag(0) {}

//! @brief Return a pointer to MultiBlockTopology.
const XC::MultiBlockTopology *XC::ModelComponentContainerBase::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= dynamic_cast<const MultiBlockTopology *>(Owner());
    return retval;
  }

//! @brief Return a pointer to MultiBlockTopology.
XC::MultiBlockTopology *XC::ModelComponentContainerBase::getMultiBlockTopology(void)
  { 
    MultiBlockTopology *retval= const_cast<MultiBlockTopology *>(dynamic_cast<const MultiBlockTopology *>(Owner()));
    return retval;
  }

//! @brief Return a pointer to preprocessor.
const XC::Preprocessor *XC::ModelComponentContainerBase::getPreprocessor(void) const
  { return getMultiBlockTopology()->getPreprocessor(); }

//! @brief Return a pointer to preprocessor.
XC::Preprocessor *XC::ModelComponentContainerBase::getPreprocessor(void)
  { return getMultiBlockTopology()->getPreprocessor(); }

