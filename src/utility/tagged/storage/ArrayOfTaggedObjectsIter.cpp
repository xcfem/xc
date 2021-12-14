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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/ArrayOfTaggedObjectsIter.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/utility/tagged/storage/ArrayOfTaggedObjectsIter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// ArrayOfTaggedObjectsIter. ArrayOfTaggedObjectsIter is a class for iterating through the 
// elements of a domain. 

#include <utility/tagged/storage/ArrayOfTaggedObjectsIter.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>


// ArrayOfTaggedObjectsIter(SingleDomain &theDomain):
//	constructor that takes the model, just the basic iter

XC::ArrayOfTaggedObjectsIter::ArrayOfTaggedObjectsIter(ArrayOfTaggedObjects &theComponents)
  :myComponents(theComponents), currIndex(0), numDone(0) {}


void XC::ArrayOfTaggedObjectsIter::reset(void)
  {
    currIndex = 0;
    numDone = 0;
  }

XC::TaggedObject *XC::ArrayOfTaggedObjectsIter::operator()(void)
  {
    // check if we still have elements in the model
    // if not return 0, indicating we are done

    // have to remove this if delete from an iter
    // if (numDone >= myComponents.numComponents)
    //    return 0;

    TaggedObject *retval= nullptr;
    // search through component list till we find the next element
    while((currIndex <= myComponents.positionLastEntry) 
	 && (myComponents.theComponents[currIndex] == 0))
      currIndex++;

    // if not at the end of the list return the element
    // NOTE: a BAD type cast is needed here - Maybe change domain
    if(currIndex < myComponents.sizeComponentArray)
      {
        retval= myComponents.theComponents[currIndex];
        numDone++; currIndex++;
      }
    return retval;
  }

    
    
