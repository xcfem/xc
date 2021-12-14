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

// $Revision: 1.3 $
// $Date: 2006/01/12 23:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/SimpleNumberer.cpp,v $



// Written: fmk
// Revision: A
//
// Description: This file contains the class definition for XC::SimpleNumberer.
// SimpleNumberer is an object to perform a simple numbering of the vertices.
// It does this by using the graphs XC::VertexIter and assigning the numbers as
// it comes across the vertices.
//
// What: "@(#) SimpleNumberer.C, revA"

#include "SimpleNumberer.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "solution/graph/graph/VertexIter.h"
#include "utility/matrix/ID.h"

//! @brief Constructor
XC::SimpleNumberer::SimpleNumberer(void)
 :BaseNumberer(GraphNUMBERER_TAG_SimpleNumberer) {}

//! @brief Virtual constructor.
XC::GraphNumberer *XC::SimpleNumberer::getCopy(void) const
  { return new SimpleNumberer(*this); }

const XC::ID &XC::SimpleNumberer::number(Graph &theGraph, int lastVertex)
  {
    // see if we can do quick return
    if(!checkSize(theGraph))
      return theRefResult;
        

    // Now we go through the iter and assign the numbers

    if(lastVertex != -1)
      {
        std::cerr << "WARNING:  SimpleNumberer::number -";
        std::cerr << " - does not deal with lastVertex";
      }

    Vertex *vertexPtr= nullptr;
    VertexIter &vertexIter= theGraph.getVertices();
    int count = 0;

    while((vertexPtr = vertexIter()) != 0)
      {
        theRefResult(count++) = vertexPtr->getTag();
        vertexPtr->setTmp(count);
      }
    return theRefResult;
  }

//! @brief Send the object thru the communicator argument.
int XC::SimpleNumberer::sendSelf(Communicator &comm)
  { return 0; }

//! @brief Receive the object thru the communicator argument.
int XC::SimpleNumberer::recvSelf(const Communicator &comm)
  { return 0; }

//! @brief Do the numbering.
const XC::ID &XC::SimpleNumberer::number(Graph &theGraph, const XC::ID &startVertices)
  { return this->number(theGraph); }
