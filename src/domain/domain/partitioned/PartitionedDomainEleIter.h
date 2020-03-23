// -*-c++-*-
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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/partitioned/PartitionedDomainEleIter.h,v $
                                                                        
                                                                        
// File: ~/domain/domain/partitioned/PartitionedDomainEleIter.h
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// PartitionedDomainEleIter. PartitionedDomainEleIter is an iter 
// for returning the elements of an object of class
// PartitionedDomain.
//
// What: "@(#) PartitionedDomainEleIter.h, revA"


#ifndef PartitionedDomainEleIter_h
#define PartitionedDomainEleIter_h


#include "domain/mesh/element/ElementIter.h"

namespace XC {
class PartitionedDomain;
 class SingleDomEleIter;
class ArrayOfTaggedObjectsIter;
class Subdomain;
class Domain;

class PartitionedDomainEleIter: public ElementIter
  {
  private:
    SingleDomEleIter	        *mainEleIter;
    ArrayOfTaggedObjectsIter 	*subdomainIter;
    ElementIter 	     	*currentIter;
    Subdomain 	 	     	*currentSubdomain;    
    PartitionedDomain	        *thePartitionedDomain;
    bool			mainDomain;
  public:
    PartitionedDomainEleIter(PartitionedDomain *theDomain);
    virtual ~PartitionedDomainEleIter();

    virtual void reset(void);
    virtual Element *operator()(void);
    
  };
} // end of XC namespace

#endif









