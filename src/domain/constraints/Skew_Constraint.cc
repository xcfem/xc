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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/Skew_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints//Skew_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::Skew_Constraint.
//
// The class XC::Skew_Constraint interface:
//

#include <domain/constraints/Skew_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "utility/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"
#include "utility/utils/misc_utils/colormod.h"

//! !@brief Constructor.
//!
//! @param tag: constraint identifier.
//! @param classTag: constraint class identifier.
XC::Skew_Constraint::Skew_Constraint(int tag, int classTag)		
 : MFreedom_Constraint(tag, 0, classTag) {}

//! Constructor to be called from subclasses.
//!
//! @param tag: multi-freedom constraint identifier.
//! @param nodeConstr: identifier of the constrained node.
//! @param classTag: constraint class identifier.
XC::Skew_Constraint::Skew_Constraint(int tag, int nodeConstr, int classTag)
 : MFreedom_Constraint(tag, nodeConstr, classTag) {}

//! Constructor to be called from subclasses.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
//! @param classTag: identifief for the class.
XC::Skew_Constraint::Skew_Constraint(int tag, int nodeConstr, const ID &constrainedDOF,const ID &retainedDOF, int classTag)
  : MFreedom_Constraint(tag, nodeConstr, constrainedDOF, retainedDOF, classTag)
    {}

//! @brief Constructor.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeConstr: identifier of the constrained node.
//! @param constr: constraint matrix.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
XC::Skew_Constraint::Skew_Constraint(int tag, int nodeConstr, Matrix &constr,
			     ID &constrainedDOF, ID &retainedDOF, int classTag)
  : MFreedom_Constraint(tag, nodeConstr, constr, constrainedDOF, retainedDOF, classTag)
  {}
//! @brief Add to nodes the actions due to this constraint.
int XC::Skew_Constraint::addResistingForceToNodalReaction(bool inclInertia)
  {
    // This behave like single freedom constraints, so they don't need
    // to compute the forces between the nodes like multi-freedom
    // constraints.
    return 0;
  }

//! @brief Printing.
void XC::Skew_Constraint::Print(std::ostream &s, int flag) const
  {     
    s << "Skew_Constraint: " << this->getTag() << std::endl
      << "\tNode Constrained: " << getNodeConstrained()
      << " constrained dof: " << constrDOF
      << " retained dof: " << retainDOF
      << " constraint matrix: " << constraintMatrix
      << std::endl;
  }

//! @brief VTK interface.
int XC::Skew_Constraint::getVtkCellType(void) const
  { return VTK_VERTEX; }


