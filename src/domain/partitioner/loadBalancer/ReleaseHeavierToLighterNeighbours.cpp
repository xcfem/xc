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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/ReleaseHeavierToLighterNeighbours.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/ReleaseHeavierToLighterNeighbours.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for XC::ReleaseHeavierToLighterNeighbours.
// A XC::ReleaseHeavierToLighterNeighbours is an object used to partition a XC::PartitionedDomain.
//
// What: "@(#) ReleaseHeavierToLighterNeighbours.C, revA"

#include "ReleaseHeavierToLighterNeighbours.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
 
XC::ReleaseHeavierToLighterNeighbours::ReleaseHeavierToLighterNeighbours(void)
  : LoadBalancer() {}

XC::ReleaseHeavierToLighterNeighbours::ReleaseHeavierToLighterNeighbours(double fG, int rels, bool disallowDisconnected)
  : LoadBalancer(fG,rels,disallowDisconnected){}


int XC::ReleaseHeavierToLighterNeighbours::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = getDomainPartitioner();    
    if(!thePartitioner)
      {
	std::cerr << "ReleaseHeavierToLighterNeighbours::balance ";
	std::cerr << "- No domainPartitioner has been set\n";
	return -1;
      }
    else
      {
        int res = 0;
        int numPartitions = theWeightedGraph.getNumVertex();
        for(int i=1; i<=numPartitions; i++)
          {
	    res = thePartitioner->releaseBoundary(i,theWeightedGraph,true,factorGreater);  
            if(res<0)
              {
                std::cerr << "WARNING XC::ReleaseHeavierToLighterNeighbours";
	        std::cerr << "::balance - DomainPartitioner returned ";
	        std::cerr << res << std::endl;
	        return res;    
	      }
          }
        return res;
      }
  }
