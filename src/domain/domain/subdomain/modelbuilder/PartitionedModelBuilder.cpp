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

//! @brief Constructor.
//!
//! Typically, a PartitionedModelBuilder is associated with a PartitionedDomain,
//! this constructor sets up a link for the PartitionedModelBuilder and the
//! domain, setting its link to the Domain object \p domain. The Domain {\em
//! domain} is passed to the constructor for ModelBuilder, and the integer
//! \p classTag is passed to the MovableObject classes constructor.
XC::PartitionedModelBuilder::PartitionedModelBuilder(PartitionedDomain &aPartitionedDomain, int theClassTag)
  : ModelBuilder(aPartitionedDomain), MovableObject(theClassTag),
    thePartitionedDomain(&aPartitionedDomain)
  {}

//! This is the constructor that is called when a PartitionedModelBuilder
//! is to be created by an FE\_ObjectBroker. The only method that can be
//! invoked on such an object is buildSubdomain().
//!
//! @param classTag: identifier of the class.
XC::PartitionedModelBuilder::PartitionedModelBuilder(Subdomain &aSubdomain, int classTag)
  : ModelBuilder(aSubdomain), MovableObject(classTag),
    thePartitionedDomain(nullptr)
  {}

//! The PartitionedModelBuilder will first check that the
//! PartitionedModelBuilder was constructed with a PartitioneDomain, if
//! not a warning message is printed and a \f$-1\f$ is returned. If o.k. the
//! object then determines the number of Subdomains, \p numSub in the
//! PartitionedDomain. It then invokes {\em buildInterface(numSub)} on 
//! itself. Then for each Subdomain in the PartitionedDomain it invokes
//! {\em buildSubdomain(numSub, *this)}. If building the interface or any
//! of the subdomains fails, a warning message is printed and a negative
//! number is returned. Returns \f$0\f$ if successful.
int XC::PartitionedModelBuilder::buildFE_Model(void)
  {
    int result;

    if(thePartitionedDomain == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(void); "
	          << "no PartitionedDomain associated with this object.\n";
        return -1;
      }

    // we build the interface, i.e. nodes on boundaries and any constraints and loads
    int numSubdomains = thePartitionedDomain->getNumSubdomains();
    result = this->buildInterface(numSubdomains);
    if (result != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(void); buildInterface failed.\n";
        return result;
      }

    // now build the subdomains, stopping if an error in building any subdomain
    SubdomainIter &theSubs = thePartitionedDomain->getSubdomains();
    Subdomain *theSubdomain= nullptr;
    while((theSubdomain = theSubs()) != 0)
      {
        result = theSubdomain->buildSubdomain(numSubdomains, *this);
        if(result != 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "(void); "
		      << "buildSubdomain failed for subdomain: "
		      << theSubdomain->getTag()
		      << std::endl;
	    return result;
          }
      }

    // if got here a PartitiondDomain has been populated
    return 0;
  }

//! @brief Return a pointer to the PartitionedDomain object.
XC::PartitionedDomain *XC::PartitionedModelBuilder::getPartitionedDomainPtr(void) const 
  { return thePartitionedDomain; }
    

