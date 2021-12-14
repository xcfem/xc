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
//MFreedom_ConstraintIter.cc


#include "MFreedom_ConstraintIter.h"
#include "domain/constraints/MFreedom_Constraint.h"

//! @brief Search on the container all the constraints with the node and
//! degree of freedom being passed as parameter.
std::deque<int> XC::MFreedom_ConstraintIter::searchAll(int theNode, int theDOF) const  
  {
    std::deque<int> retval;

    MFreedom_Constraint *theMFreedom= nullptr;
    MFreedom_ConstraintIter *this_no_const= const_cast<MFreedom_ConstraintIter *>(this);
    while(((theMFreedom= (*this_no_const)()) != nullptr))
      if(theMFreedom->affectsNodeAndDOF(theNode,theDOF))
        retval.push_back(theMFreedom->getTag());
    return retval;
  }

//! @brief Search on the container all the constraints that affect the node being passed as parameter.
std::deque<int> XC::MFreedom_ConstraintIter::searchAll(int theNode) const
  {
    std::deque<int> retval;

    MFreedom_Constraint *theMFreedom= nullptr;
    MFreedom_ConstraintIter *this_no_const= const_cast<MFreedom_ConstraintIter *>(this);
    while(((theMFreedom= (*this_no_const)()) != nullptr))
      if(theMFreedom->affectsNode(theNode))
        retval.push_back(theMFreedom->getTag());
    return retval;
  }
