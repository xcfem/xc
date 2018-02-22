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
// $Date: 2006/01/10 00:42:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.cpp,v $
                                                                        
// Written: fmk 
// Created: 04/05
//
// Description: This file contains the implementation for XC::SparseGenRowLinSOE

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::SparseGenRowLinSOE::SparseGenRowLinSOE(AnalysisAggregation *owr)
  : SparseGenSOEBase(owr,LinSOE_TAGS_SparseGenRowLinSOE) {}

bool XC::SparseGenRowLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    SparseGenRowLinSolver *tmp= dynamic_cast<SparseGenRowLinSolver *>(newSolver);
    if(tmp)
      retval= SparseGenSOEBase::setSolver(tmp);
    else
      std::cerr << "SparseGenRowLinSOE::setSolver; solver incompatible con system of equations." << std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::SparseGenRowLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    // fist iterate through the vertices of the graph to get nnz
    Vertex *theVertex;
    int newNNZ = 0;
    VertexIter &theVertices = theGraph.getVertices();
    while ((theVertex = theVertices()) != 0)
      {
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	newNNZ += theAdjacency.size() +1; // the +1 is for the diag entry
      }
    nnz = newNNZ;

    if(newNNZ > A.Size())
      { // we have to get more space for A and colA
	A.resize(newNNZ);
	colA.resize(newNNZ);
      }
    A.Zero();
	
    factored = false;
    
    if(size > B.Size())
      {	
        inic(size);
	rowStartA.resize(size+1); 
      }

    // fill in rowStartA and colA
    if(size != 0)
      {
        rowStartA(0) = 0;
        int startLoc = 0;
        int lastLoc = 0;
        for(int a=0; a<size; a++)
          {
            theVertex = theGraph.getVertexPtr(a);
	if (theVertex == 0) {
	  std::cerr << "WARNING:XC::SparseGenRowLinSOE::setSize :";
	  std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	  size = 0;
	  return -1;
	}

	colA(lastLoc++) = theVertex->getTag(); // place diag in first
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	
	// now we have to place the entries in the XC::ID into order in colA
        for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
	  {

	  int row = *i;
	  bool foundPlace = false;
	  // find a place in colA for current col
	  for (int j=startLoc; j<lastLoc; j++)
	    if (colA(j) > row) { 
	      // move the entries already there one further on
	      // and place col in current location
	      for (int k=lastLoc; k>j; k--)
		
		colA(k) = colA(k-1);
	      colA(j) = row;
	      foundPlace = true;
	      j = lastLoc;
	    }
	  if(foundPlace == false) // put in at the end
	    colA(lastLoc) = row;

	  lastLoc++;
	}
	rowStartA(a+1)= lastLoc;;	    
	startLoc = lastLoc;
      }
    }
    
    // invoke setSize() on the XC::Solver   
     LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if (solverOK < 0) {
	std::cerr << "WARNING:XC::SparseGenRowLinSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
    }    
    return result;
}

int 
XC::SparseGenRowLinSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
{
    // check for a quick return 
    if (fact == 0.0)  
	return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if (idSize != m.noRows() && idSize != m.noCols()) {
	std::cerr << "XC::SparseGenRowLinSOE::addA() ";
	std::cerr << " - Matrix and XC::ID not of similar sizes\n";
	return -1;
    }
    
    if (fact == 1.0) { // do not need to multiply 
	for (int i=0; i<idSize; i++) {
	    int row = id(i);
	    if (row < size && row >= 0) {
	      int startRowLoc = rowStartA(row);
	      int endRowLoc = rowStartA(row+1);
		for (int j=0; j<idSize; j++) {
		    int col = id(j);
		    if (col <size && col >= 0) {
			// find place in A using colA
			for (int k=startRowLoc; k<endRowLoc; k++)
			  if (colA(k) == col) {
				A[k] += m(i,j);
				k = endRowLoc;
			    }
		     }
		}  // for j		
	    } 
	}  // for i
    } else {
	for (int i=0; i<idSize; i++) {
	    int row = id(i);
	    if (row < size && row >= 0) {
	      int startRowLoc = rowStartA(row);
	      int endRowLoc = rowStartA(row+1);
		for (int j=0; j<idSize; j++) {
		    int col = id(j);
		    if (col <size && col >= 0) {
			// find place in A using colA
			for (int k=startRowLoc; k<endRowLoc; k++)
			  if (colA(k) == col) {
				A[k] += fact * m(i,j);
				k = endRowLoc;
			    }
		     }
		}  // for j		
	    } 
	}  // for i
    }
    return 0;
}

    
int XC::SparseGenRowLinSOE::sendSelf(CommParameters &cp)
  { return 0; }

int XC::SparseGenRowLinSOE::recvSelf(const CommParameters &cp)  
  { return 0; }

