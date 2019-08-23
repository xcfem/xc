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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/single/SingleDomAllSFreedom_Iter.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/domain/domain/SingleDomAllSFreedom_Iter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// SingleDomAllSFreedom_Iter. SingleDomAllSFreedom_Iter is a class for iterating through the 
// elements of a domain. 

#include "SingleDomAllSFreedom_Iter.h"

#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLocker.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>

// SingleDomAllSFreedom_Iter(SingleDomAllain &theDomain):
//	constructor that takes the model, just the basic iter

XC::SingleDomAllSFreedom_Iter::SingleDomAllSFreedom_Iter(Domain &domain)
  :theDomain(&domain), doneDomainSPs(false) {}


void XC::SingleDomAllSFreedom_Iter::reset(void)
  {
    theDomainSPs = &(theDomain->getConstraints().getSPs());

    theLoadPatterns= &(theDomain->getConstraints().getLoadPatterns());
    currentLoadPattern= theLoadPatterns->begin();
    if(currentLoadPattern!=theLoadPatterns->end())
      { theLoadPatternSPs= &(currentLoadPattern->second->getSPs()); }

    theNodeLockers= &(theDomain->getConstraints().getNodeLockers());
    currentNodeLocker= theNodeLockers->begin();
    if(currentNodeLocker!=theNodeLockers->end())
      { theNodeLockerSPs = &(currentNodeLocker->second->getSPs()); }
    doneDomainSPs = false;
  }


XC::SFreedom_Constraint *XC::SingleDomAllSFreedom_Iter::operator()(void)
  {
    SFreedom_Constraint *sFreedomConstraintPtr= nullptr;

    if(doneDomainSPs == false)
      {
        sFreedomConstraintPtr= (*theDomainSPs)();
        if(sFreedomConstraintPtr)
          return sFreedomConstraintPtr;
        else
          doneDomainSPs = true;
      }

    while(currentLoadPattern!=theLoadPatterns->end())
      {
        sFreedomConstraintPtr= (*theLoadPatternSPs)();
        if(!sFreedomConstraintPtr)
          {
            currentLoadPattern++;
            if(currentLoadPattern!=theLoadPatterns->end())
              theLoadPatternSPs= &(currentLoadPattern->second->getSPs());
          }
        else
	  return sFreedomConstraintPtr;
      }

    while(currentNodeLocker!=theNodeLockers->end())
      {
        sFreedomConstraintPtr= (*theNodeLockerSPs)();
        if(!sFreedomConstraintPtr)
          {
            currentNodeLocker++;
            if(currentNodeLocker!=theNodeLockers->end())
              theNodeLockerSPs = &(currentNodeLocker->second->getSPs());
          }
        else
          return sFreedomConstraintPtr;
      }
    return 0;
  }
    
    


    
    
