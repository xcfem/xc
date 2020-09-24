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

//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the implementation of class XC::GlueNodeToElement.

#include <domain/constraints/GlueNodeToElement.h>
#include "vtkCellType.h"

//! @brief Constructor.
//! @param tag: tag for the constraint.
XC::GlueNodeToElement::GlueNodeToElement(int tag)		
 : MRMFreedom_Constraint(tag,CNSTRNT_TAG_GlueNodeToElement) {}

//! @brief Constructor.
//! @param tag: tag for the constraint.
//! @param node: node to be glued.
//! @param elem: element to glue to.
//! @param constrainedDOF: constrained degrees of freedom.
XC::GlueNodeToElement::GlueNodeToElement(int tag, const Node &node,const Element &elem, const ID &constrainedDOF)
  : MRMFreedom_Constraint(tag, elem, node, constrainedDOF) {}

//! @brief VTK interface.
int XC::GlueNodeToElement::getVtkCellType(void) const
  { return VTK_VERTEX; }
