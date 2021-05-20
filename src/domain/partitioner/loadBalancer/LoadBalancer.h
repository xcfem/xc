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
// $Date: 2000/09/15 08:23:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/LoadBalancer.h,v $
                                                                        
                                                                        
// File: ~/domain/loadBalancer/LoadBalancer.h
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for LoadBalancer.
// A LoadBalancer is an object used to partition a PartitionedDomain.
//
// What: "@(#) LoadBalancer.h, revA"

#ifndef LoadBalancer_h
#define LoadBalancer_h

#include "utility/kernel/CommandEntity.h"

namespace XC {
class Vector;
class DomainPartitioner;
class Graph;

//! @ingroup Partitioner
//!
//! @defgroup LoadBalancers Partitioned domains balancers.
//
//! A LoadBalancer is an object used to balance a
//! PartitionedDomain. The LoadBalancer does this by invoking methods in
//! the DomainPartitioner object with which it is associated.
class LoadBalancer: public CommandEntity
  {
  private:
    DomainPartitioner *theDomainPartitioner;    
  protected:
    int numReleases; //! number of releases.
    double factorGreater;
    bool disallowDisconnectedGraphs;

    DomainPartitioner *getDomainPartitioner(void);
  public:
    LoadBalancer(void);
    LoadBalancer(double factGreater, int releases, bool disallowDisconnected= true);

    virtual void setLinks(DomainPartitioner &thePartitioner);
    
    //! Each subclass must provide an implementation of this method. This
    //! method is invoked to balance the PartitionedDomain. The Graph {\em
    //! theWeightedGraph} is a weighted graph in which the vertices represent
    //! the Subdomains, the edges Subdomains sharing common boundary nodes and
    //! the weight the cost of the previous Subdomain calculations (determined
    //! by invoking getCost() on the Subdomains).
    virtual int balance(Graph &theWeightedGraph) =0;
  };
} // end of XC namespace

#endif


