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
                                                                        
// File: ~/domain/domain/single/SingleDomMRMFreedom_Iter.h
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for 
// SingleDomMRMFreedom_Iter. SingleDomMRMFreedom_Iter is an iter for returning 
// the multiple retained node constraints  of an object of class SingleDomain. 
// SingleDomMRMFreedom_Iters must be written for each subclass of SingleDomain.
// where the storage of the MRMFreedom_Constraints changes.

#ifndef SingleDomMRMFreedom_Iter_h
#define SingleDomMRMFreedom_Iter_h

#include <domain/constraints/MRMFreedom_ConstraintIter.h>
#include <domain/domain/single/SingleDomTIter.h>

namespace XC {

class SingleDomMRMFreedom_Iter: public SingleDomTIter<MRMFreedom_ConstraintIter>
  {
  public:
    SingleDomMRMFreedom_Iter(TaggedObjectStorage *theStorage);
    virtual MRMFreedom_Constraint *operator()(void);
  };
} // end of XC namespace

#endif

