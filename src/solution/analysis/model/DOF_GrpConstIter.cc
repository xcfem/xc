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
//DOF_GrpConstIter.cpp

#include "solution/analysis/model/DOF_GrpConstIter.h"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>


XC::DOF_GrpConstIter::DOF_GrpConstIter(TaggedObjectStorage *theStorage)
  :myIter(&(theStorage->getComponents())) {}


void XC::DOF_GrpConstIter::reset(void)
  { myIter->reset(); }    


const XC::DOF_Group *XC::DOF_GrpConstIter::operator()(void)
  {
    const DOF_Group *retval= nullptr;
    const TaggedObject *theComponent= (*myIter)();
    if(theComponent)
      retval= dynamic_cast<const DOF_Group *>(theComponent);
    return retval;
  }

    
    
