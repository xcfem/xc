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
// $Date: 2003/02/14 23:00:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/SwapHeavierToLighterNeighbours.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/HeavierToSmallerNeighbours.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for 
// HeavierToSmallerNeighbours. A HeavierToSmallerNeighbours is an object 
// is used to partition a XC::PartitionedDomain.
//
// What: "@(#) HeavierToSmallerNeighbours.C, revA"

#include "SwapHeavierToLighterNeighbours.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
#include <utility/matrix/ID.h> 

//! @brief Default constructor.
XC::SwapHeavierToLighterNeighbours::SwapHeavierToLighterNeighbours(void)
 :LoadBalancer() {}

//! @brief Constructor.
//!
//! Sets the parameters used in the balance() method.
//!
//! @param fG: factor greater.
//! @param rels: number of releases. 
XC::SwapHeavierToLighterNeighbours::SwapHeavierToLighterNeighbours(double fG, int rels)
  : LoadBalancer(fG,rels){}

//! For \p numRelease times the Vertices of \p theWeightedGraph are
//! iterated through. For each Vertex, \f$i\f$, the weight of the Vertex is
//! compared to those of it's adjacent Vertices, \f$other\f$, (this is done by
//! looping through the adjacency ID of the Vertex), if the vertex weight is
//! {\em factorGreater} times greater than the other Vertices load then {\em
//! swapBoundary(i, other)} is invoked on the DomainPartitioner. Returns
//! \f$0\f$ if successful, otherwise a negative number and a warning message
//! are returned if either no link has been set to the DomainPartitioner
//! or swapBoundary() returns a negative number.
int XC::SwapHeavierToLighterNeighbours::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = this->getDomainPartitioner();
    if(!thePartitioner)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no DomainPartitioner has been set.\n"; 
        return -1;
      }

    int res = 0;

    for(int ii=0; ii<numReleases; ii++)
      {
        VertexIter &theVertices = theWeightedGraph.getVertices();
        Vertex *vertexPtr;
        while((vertexPtr = theVertices()) != 0)
          {
            int vertexTag = vertexPtr->getTag();
            double vertexLoad = vertexPtr->getWeight();
            const std::set<int> &adjacency= vertexPtr->getAdjacency();
            for(std::set<int>::const_iterator j= adjacency.begin(); j!=adjacency.end(); j++)
              {
                const int otherVertexTag= *j;
                Vertex *otherVertexPtr= theWeightedGraph.getVertexPtr(otherVertexTag);
                const double otherVertexLoad= otherVertexPtr->getWeight();
                
                if(vertexLoad > otherVertexLoad && otherVertexLoad != 0) 
                  if(vertexLoad/otherVertexLoad > factorGreater)
                    {
                      res = thePartitioner->swapBoundary(vertexTag,otherVertexTag);
                        if(res < 0)
                          {
                            std::cerr << getClassName() << "::" << __FUNCTION__
				      << "; WARNING, "
			              << "DomainPartitioner returned: "
				      << res << std::endl;
                            return res;
                          }
                    }
                
                if(vertexLoad != 0 && otherVertexLoad == 0)
                  {
                    res= thePartitioner->swapBoundary(vertexTag,otherVertexTag); 
                    if(res < 0)
                      {
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; WARNING, "
				  << " DomainPartitioner returned: "
				  << res << std::endl;
                        return res;
                      }
                  }
              }                
          }
      }
    return res;
  }
