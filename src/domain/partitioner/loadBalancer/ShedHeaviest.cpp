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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/ShedHeaviest.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/ShedHeaviest.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for ShedHeaviest.
// A ShedHeaviest is an object used to partition a PartitionedDomain.
//
// What: "@(#) ShedHeaviest.C, revA"

#include "ShedHeaviest.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
 
//! @brief Default constructor.
XC::ShedHeaviest::ShedHeaviest(void)
 : LoadBalancer() {}

//! @brief Constructor.
//!
//! Sets the parameters used in the balance() method.
//!
//! @param fG: factor greater.
//! @param rels: number of releases. 
XC::ShedHeaviest::ShedHeaviest(double fG, int rels, bool disallowDisconnected)
 :LoadBalancer(fG,rels,disallowDisconnected){}

//! The heaviest loaded partition, \p max, is first determined by
//! iterating through the Graph \p theWeightedGraph looking at the
//! vertex weights. Then {\em releaseBoundary(max, theWieightedGraph,
//! true, factorGreater)} is invoked on the
//! DomainPartitioner \p numRelease times. Returns \f$0\f$ if successful,
//! otherwise a negative number and a warning message are returned if
//! either no link has been set to the DomainPartitioner or {\em
//! releaseBoundary()} returns a negative number.
int XC::ShedHeaviest::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = this->getDomainPartitioner();
    if(!thePartitioner)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no DomainPartitioner has been set.\n";
	return -1;
      }

    // determine the max loaded partition
    VertexIter &theVertices = theWeightedGraph.getVertices();
    Vertex *vertexPtr = theVertices();
    int maxPartition = vertexPtr->getTag();
    double maxLoad = vertexPtr->getWeight();
    while((vertexPtr = theVertices()) != 0)
      if(vertexPtr->getWeight() > maxLoad)
        {
	  maxLoad = vertexPtr->getWeight();
	  maxPartition = vertexPtr->getTag();
	}
    
    // release the boundary numReleases times
    int res = 0;
    for(int j=0; j<numReleases; j++)
      {
	res= thePartitioner->releaseBoundary(maxPartition,theWeightedGraph,true,factorGreater);  
	if(res < 0)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "WARNING; releaseBoundary returned: "
		      << res << std::endl;
	    j = numReleases;
	  }
      }
    return res;
  }
