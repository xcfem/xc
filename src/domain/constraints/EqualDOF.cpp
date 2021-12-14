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
//EqualDOF.cpp

#include <domain/constraints/EqualDOF.h>
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include "vtkCellType.h"

//! @brief Constructor.
XC::EqualDOF::EqualDOF(int tag)
  : MFreedom_Constraint(tag) {}

//! @brief Constructor.
XC::EqualDOF::EqualDOF(int tag,const int &retainedNode,const int &constrainedNode,const ID &dofs)
  : MFreedom_Constraint(tag,retainedNode,constrainedNode,0)
  { setDofs(dofs); }

//! @brief Sets retained degrees of fredom.
void XC::EqualDOF::setDofs(const ID &dofs)
  {
     set_retained_dofs(dofs);
     set_constrained_dofs(retainDOF);
  }

//! @brief Computes constraint matrix.
void XC::EqualDOF::setup_matrix(void)
  {
    // The number of DOF to be coupled
    const int numDOF= retainDOF.Size();

    // The constraint matrix ... U_c = C_cr * U_r
    Matrix Ccr(numDOF, numDOF);
    Ccr.Zero();

    for(int j=0;j<numDOF;j++) 
      Ccr(j,j) = 1.0;
    set_constraint(Ccr);
  }

//! @brief Constraint setup.
void XC::EqualDOF::setup(Domain *theDomain)
  {  setup_matrix(); }

//! @brief VTK interface.
int XC::EqualDOF::getVtkCellType(void) const
  { return VTK_LINE; }
