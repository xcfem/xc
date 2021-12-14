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
//SFreedom_ConstraintIter.cc


#include "SFreedom_ConstraintIter.h"
#include <domain/constraints/SFreedom_Constraint.h>

//! @brief Search on the container the constraint with the node and degree
//! of freedom being passed as parameter.
XC::SFreedom_Constraint *XC::SFreedom_ConstraintIter::search(int theNode, int theDOF)
  {
    SFreedom_Constraint *retval= nullptr;
    SFreedom_Constraint *theSP= nullptr;
    bool found= false;
    int spTag= 0;
    while((found == false) && ((theSP= (*this)()) != nullptr))
      {
        int nodeTag= theSP->getNodeTag();
        int dof= theSP->getDOF_Number();
        if(nodeTag == theNode && dof == theDOF)
          {
            spTag= theSP->getTag();
	    retval= theSP;
            found= true;
          }
      }
    return retval;
  }


//! @brief Search on the container all the constraints with the node and
//! degree of freedom being passed as parameter.
std::deque<int> XC::SFreedom_ConstraintIter::searchAll(int theNode, int theDOF) const  
  {
    std::deque<int> retval;

    SFreedom_Constraint *theSP= nullptr;
    SFreedom_ConstraintIter *this_no_const= const_cast<SFreedom_ConstraintIter *>(this);
    while(((theSP= (*this_no_const)()) != nullptr))
      if(theSP->affectsNodeAndDOF(theNode,theDOF))
        retval.push_back(theSP->getTag());
    return retval;
  }

//! @brief Search on the container all the constraints that affect the node being passed as parameter.
std::deque<int> XC::SFreedom_ConstraintIter::searchAll(int theNode) const
  {
    std::deque<int> retval;

    SFreedom_Constraint *theSP= nullptr;
    SFreedom_ConstraintIter *this_no_const= const_cast<SFreedom_ConstraintIter *>(this);
    while(((theSP= (*this_no_const)()) != nullptr))
      if(theSP->affectsNode(theNode))
        retval.push_back(theSP->getTag());
    return retval;
  }
