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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/LoadBalancer.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/LoadBalancer.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for XC::LoadBalancer.
// A XC::LoadBalancer is an object used to partition a XC::PartitionedDomain.
//
// What: "@(#) LoadBalancer.C, revA"

#include "LoadBalancer.h"
#include "domain/partitioner/DomainPartitioner.h"

//! @brief Default constructor.
XC::LoadBalancer::LoadBalancer(void)
  : theDomainPartitioner(nullptr),
    numReleases(1), factorGreater(1.0), disallowDisconnectedGraphs(true){}

//! @brief Constructor.
//!
//! Sets the parameters used in the balance() method.
//!
//! @param factGreater: factor greater.
//! @param releases: number of releases.
XC::LoadBalancer::LoadBalancer(double factGreater, int releases, bool disallowDisconnected)
 :theDomainPartitioner(nullptr),numReleases(releases),factorGreater(factGreater), disallowDisconnectedGraphs(disallowDisconnected)  
  {
    if(releases < 0)
      numReleases= 0;
  }

//! @brief Sets the pointer to the DomainPartitioner object associated with the
//! LoadBalancer to point to \p thePartitioner.
void XC::LoadBalancer::setLinks(DomainPartitioner &thePartitioner)
  { theDomainPartitioner = &thePartitioner; }

//! @brief Returns a pointer to the DomainPartitioner. If no DomainPartitioner
//! has been set, a warning message is printed and \f$0\f$ returned.
XC::DomainPartitioner *XC::LoadBalancer::getDomainPartitioner(void)
  {
    if(!theDomainPartitioner)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING, no DomainPartitioner is set,"
	          << " has setLinks() been called?\n";
      }
    return theDomainPartitioner;
  }

