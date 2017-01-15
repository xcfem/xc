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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/partitioned/PartitionedDomainEleIter.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/domain/domain/partitioned/PartitionedDomainEleIter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// PartitionedDomainEleIter. PartitionedDomainEleIter is a 
// class for iterating through the elements of a ParitionedDomain
// domain. 
//
// What: "@(#) PartitionedDomainEleIter.C, revA"

#include <domain/domain/partitioned/PartitionedDomainEleIter.h>

#include "domain/domain/subdomain/Subdomain.h"
#include <domain/mesh/element/Element.h>
#include <domain/domain/single/SingleDomEleIter.h>
#include <domain/domain/partitioned/PartitionedDomain.h>
#include <utility/tagged/storage/ArrayOfTaggedObjectsIter.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>



// PartitionedDomainEleIter(SingleDomain &theDomain):
//	constructor that takes the model, just the basic iter

XC::PartitionedDomainEleIter::PartitionedDomainEleIter(PartitionedDomain *partitionedDomain)
  :subdomainIter(0), currentIter(0), currentSubdomain(0),
   thePartitionedDomain(partitionedDomain)
  {
    mainEleIter= new SingleDomEleIter(thePartitionedDomain->elements); 
    subdomainIter= new ArrayOfTaggedObjectsIter(*(thePartitionedDomain->theSubdomains));
  }


XC::PartitionedDomainEleIter::~PartitionedDomainEleIter()
  {
    delete mainEleIter; 
    delete subdomainIter;
  }    

void XC::PartitionedDomainEleIter::reset(void)
  {
    mainDomain = true;
    mainEleIter->reset();
    subdomainIter->reset();
    currentIter = mainEleIter;

    TaggedObject *currentObject = (*subdomainIter)();
    if (currentObject != 0)
	currentSubdomain = (Subdomain *)currentObject;
    else
	currentSubdomain = 0;
  }

XC::Element *XC::PartitionedDomainEleIter::operator()(void)
  {
    Element *theEle= nullptr;

    while((currentSubdomain != 0 || mainDomain == true))
      {
	if(mainDomain == true)
          {
	    theEle= (*currentIter)();

	    if(theEle)
              { return theEle; }
	    else
              {
		mainDomain = false;
		Element *res = currentSubdomain;
		TaggedObject *currentObject = (*subdomainIter)();
		currentSubdomain = (Subdomain *)currentObject;
		return res;
	      }
	  }
        else
          {
	    Element *res = currentSubdomain;
	    TaggedObject *currentObject = (*subdomainIter)();
	    currentSubdomain = (Subdomain *)currentObject;
	    return res;
	  }
      }
    // we will only get here if we are done 
    return nullptr;
  }
