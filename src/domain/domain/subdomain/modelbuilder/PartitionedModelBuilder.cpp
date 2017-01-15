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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/modelbuilder/PartitionedModelBuilder.cpp,v $
                                                                        
                                                                        
// File: ~/model/PartitionedModelBuilder.C
//
// Written: fmk 
// Created: 03/98
// Revision: A
//

#include "PartitionedModelBuilder.h"
#include "domain/domain/partitioned/PartitionedDomain.h"
#include "domain/domain/subdomain/Subdomain.h"
#include "domain/domain/SubdomainIter.h"

//  PartitionedModelBuilderModel(Domain &theDomain, int theClassTag);
//	constructor
XC::PartitionedModelBuilder::PartitionedModelBuilder(PartitionedDomain &aPartitionedDomain,
						 int theClassTag)
:ModelBuilder(aPartitionedDomain), MovableObject(theClassTag),
thePartitionedDomain(&aPartitionedDomain)
{

}

XC::PartitionedModelBuilder::PartitionedModelBuilder(Subdomain &aSubdomain,
						 int theClassTag)
:ModelBuilder(aSubdomain), MovableObject(theClassTag),
thePartitionedDomain(0)
{

}

int XC::PartitionedModelBuilder::buildFE_Model(void)
{
  int result;

  if (thePartitionedDomain == 0) {
    std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
    std::cerr << "No XC::PartitionedDomain associated with this object\n";
    return -1;
  }

  // we build the interface, i.e. nodes on boundaries and any constraints and loads
  int numSubdomains = thePartitionedDomain->getNumSubdomains();
  result = this->buildInterface(numSubdomains);
  if (result != 0) {
    std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
    std::cerr << "buildInterface failed\n";
    return result;
  }

  // now build the subdomains, stopping if an error in building any subdomain
  SubdomainIter &theSubs = thePartitionedDomain->getSubdomains();
  Subdomain *theSubdomain;
  while ((theSubdomain = theSubs()) != 0) {
    result = theSubdomain->buildSubdomain(numSubdomains, *this);
    if (result != 0) {
	std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
	std::cerr << "buildSubdomain failed for XC::Subdomain " << theSubdomain->getTag();
	std::cerr << std::endl;
	return result;
    }

  }

  // if got here a PartitiondDomain has been populated
  return 0;
}

 XC::PartitionedDomain *
XC::PartitionedModelBuilder::getPartitionedDomainPtr(void) const 
{
    return thePartitionedDomain;
}
    

