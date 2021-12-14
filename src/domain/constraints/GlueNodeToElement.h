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
                                                                        
#ifndef GlueNodeToElement_h
#define GlueNodeToElement_h

// File: ~/domain/constraints/GlueNodeToElement.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for GlueNodeToElement.
// GlueNodeToElement is a class which stores the information for a multiple
// retained nodes constraint. A multifreedom constraint relates certain dofs at 
// a constrained node to be related to certains dofs at multiple retained nodes:

// The GlueNodeToElement class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//

#include "MRMFreedom_Constraint.h"


namespace XC {

//! @ingroup CContMP
//
//! @brief Glue a node to an element.
class GlueNodeToElement: public MRMFreedom_Constraint
  {

  public:
    // constructors        
    GlueNodeToElement(int tag); // LCPT
    GlueNodeToElement(int tag, const Node &,const Element &, const ID &);

    int getVtkCellType(void) const;
  };
} // end of XC namespace

#endif

