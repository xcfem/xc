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
                                                                        
// $Revision: 1.3 $
// $Date: 2006/01/10 00:31:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/partitioner/DomainPartitioner.h,v $
                                                                        
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for DomainPartitioner.
// A DomainPartitioner is an object used to partition a PartitionedDomain.
//
// What: "@(#) DomainPartitioner.h, revA"

#ifndef DomainPartitioner_h
#define DomainPartitioner_h

#include <utility/matrix/ID.h>
#include "utility/kernel/CommandEntity.h"

namespace XC {
class GraphPartitioner;
class LoadBalancer;
class PartitionedDomain;
class Vector;
class Graph;
class TaggedObjectStorage;

//! @ingroup Dom
//!
//! @defgroup Partitioner Domain partitioners.
//
//! @ingroup Partitioner
//
//! @brief Object that can partition and load balance a PartitionedDomain.
//!
//! A DomainPartitioner is an object used to partition and load balance
//! a PartitionedDomain. The DomainPartitioner uses the element graph of
//! the domain to partition and load balance. Derived types can use the
//! node graph of the domain. The partitioner uses a GraphPartitioner and
//! a LoadBalancingAlgo to partition and load balance the domain. 
class DomainPartitioner: public CommandEntity
  {
  private:
    PartitionedDomain *myDomain; 
    GraphPartitioner  &thePartitioner;
    LoadBalancer      *theBalancer;    

    Graph *theElementGraph;
    std::vector<Graph> theBoundaryElements; 
    
    TaggedObjectStorage *theNodeLocations;
    ID *elementPlace;
    int numPartitions;
    ID primes;
    bool partitionFlag;
    
    bool usingMainDomain;
    int mainPartition;

    int inic(const size_t &);
  public:   
    DomainPartitioner(GraphPartitioner &theGraphPartitioner,
    		      LoadBalancer &theLoadBalancer);
    
    DomainPartitioner(GraphPartitioner &theGraphPartitioner);

    virtual void setPartitionedDomain(PartitionedDomain &theDomain);
    virtual int partition(int numParts, bool useMainDomain = false, int mainPartition = 0);

    virtual int balance(Graph &theWeightedSubdomainGraph);

    // public member functions needed by the load balancer
    virtual int getNumPartitions(void) const;
    virtual Graph &getPartitionGraph(void);
    virtual Graph &getColoredGraph(void);
    
    virtual int  swapVertex(int from, 
			    int to, 
			    int vertexTag,
			    bool adjacentVertexNotInOther = true);

    virtual int	 swapBoundary(int from, 
			      int to,
			      bool adjacentVertexNotInOther = true);

    virtual int  releaseVertex(int from, 
			       int vertexTag, 
			       Graph &theWeightedPartitionGraph, 
			       bool mustReleaseToLighter = true,
			       double factorGreater = 1.0,
			       bool adjacentVertexNotInOther = true);

    virtual int releaseBoundary(int from, 
			       Graph &theWeightedPartitionGraph, 
			       bool mustReleaseToLighter = true,
			       double factorGreater = 1.0,
			       bool adjacentVertexNotInOther = true);
  };
} // end of XC namespace

#endif


