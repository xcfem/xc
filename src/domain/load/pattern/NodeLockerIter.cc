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
//NodeLockerIter.cpp

#include "domain/load/pattern/NodeLockerIter.h"

#include <domain/load/pattern/NodeLocker.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>


// NodeLockerIter(SingleDomain &theDomain):
//	constructor that takes the model, just the basic iter

XC::NodeLockerIter::NodeLockerIter(TaggedObjectStorage *theStorage)
  :myIter(theStorage->getComponents()) {}


void XC::NodeLockerIter::reset(void)
  { myIter.reset(); }    


XC::NodeLocker *XC::NodeLockerIter::operator()(void)
  {
    // check if we still have elements in the model
    // if not return 0, indicating we are done
    TaggedObject *theComponent = myIter();
    if(theComponent == 0)
      return 0;
    else
      {
	NodeLocker *result= dynamic_cast<NodeLocker *>(theComponent);
	return result;
      }
  }

