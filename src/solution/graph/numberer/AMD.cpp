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
                                                                        
// $Revision: 1.1 $
// $Date: 2009-12-10 00:40:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/AMD.cpp,v $
                                                                        

// Description: This file contains the class definition for AMD.

// What: "@(#) AMD.C, revA"


#include "AMD.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "solution/graph/graph/VertexIter.h"
#include "utility/matrix/ID.h"
#include "suitesparse/amd.h"

// Constructor
XC::AMD::AMD(void)
 : BaseNumberer(GraphNUMBERER_TAG_AMD) {}

//! @brief Virtual constructor.
XC::GraphNumberer *XC::AMD::getCopy(void) const
  { return new AMD(*this); }

//! @brief Do the numbering.
const XC::ID &XC::AMD::number(Graph &theGraph, int startVertex)
  {
    int numVertex = theGraph.getNumVertex();

    if (numVertex == 0) 
      return theRefResult;

    theRefResult.resize(numVertex);

    int nnz = 0;
    Vertex *vertexPtr;
    VertexIter &vertexIter = theGraph.getVertices();

    while ((vertexPtr = vertexIter()) != 0)
      {
        const std::set<int> &adjacency = vertexPtr->getAdjacency();
        nnz+= adjacency.size();
      }

    std::vector<int> P(numVertex);
    std::vector<int> Ap(numVertex+1);
    std::vector<int> Ai(nnz);
    //double Control[AMD_CONTROL];
    //double Info[AMD_INFO];

    VertexIter &vertexIter2 = theGraph.getVertices();

    nnz = 0;
    int count = 1;
    Ap[0] = 0;

    while((vertexPtr = vertexIter2()) != 0)
      {
        const std::set<int> &adjacency = vertexPtr->getAdjacency();
        for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
	  { Ai[nnz++]= *i; }
        Ap[count++] = nnz;
      }  

    amd_order(numVertex, Ap.data(), Ai.data(), P.data(), (double *)nullptr, (double *)nullptr);

    for(int i=0; i<numVertex; i++)
      theRefResult[i] = P[i];

    return theRefResult;
  }



//! @brief Send the object thru the communicator argument.
int XC::AMD::sendSelf(Communicator &comm)
  { return 0; }

//! @brief Receive the object thru the communicator argument.
int  XC::AMD::recvSelf(const Communicator &comm)
  { return 0; }


//! @brief Do the numbering.
const XC::ID &XC::AMD::number(Graph &theGraph, const ID &startVertices)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; WARNING: not implemented with startVertices";
    return theRefResult;
  }






