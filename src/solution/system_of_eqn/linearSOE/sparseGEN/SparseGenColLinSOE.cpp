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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/12/06 22:11:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 04/98
// Revision: A
//
// Description: This file contains the implementation for XC::SparseGenColLinSOE

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>

XC::SparseGenColLinSOE::SparseGenColLinSOE(AnalysisAggregation *owr)
  :SparseGenSOEBase(owr,LinSOE_TAGS_SparseGenColLinSOE) {}

XC::SparseGenColLinSOE::SparseGenColLinSOE(AnalysisAggregation *owr,int classTag)
  :SparseGenSOEBase(owr,classTag) {}

bool XC::SparseGenColLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    SparseGenColLinSolver *tmp= dynamic_cast<SparseGenColLinSolver *>(newSolver);
    if(tmp)
      retval= SparseGenSOEBase::setSolver(tmp);
    else
      std::cerr << "SparseGenColLinSOE::setSolver; solver incompatible con system of equations." << std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::SparseGenColLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    // fist iterate through the vertices of the graph to get nnz
    Vertex *theVertex;
    int newNNZ = 0;
    VertexIter &theVertices = theGraph.getVertices();
    while((theVertex = theVertices()) != 0)
      {
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	newNNZ += theAdjacency.size() +1; // the +1 is for the diag entry
      }
    nnz = newNNZ;

    if(newNNZ > A.Size())
      { // we have to get more space for A and rowA
	A.resize(newNNZ);
        rowA.resize(newNNZ);
      }
    A.Zero();
	
    factored = false;
    
    if(size > B.Size())
      { // we have to get space for the vectors
	
        inic(size);
	colStartA.resize(size+1);	
      }

    // fill in colStartA and rowA
    if(size != 0)
      {
        colStartA(0)= 0;
        int startLoc = 0;
        int lastLoc = 0;
        for(int a=0;a<size;a++)
          {
            theVertex = theGraph.getVertexPtr(a);
	    if(theVertex == 0)
              {
	        std::cerr << "WARNING:XC::SparseGenColLinSOE::setSize :";
	        std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	        size = 0;
	        return -1;
	      }
            rowA(lastLoc++) = theVertex->getTag(); // place diag in first
	    const std::set<int> &theAdjacency = theVertex->getAdjacency();
	
	    // now we have to place the entries in the ID into order in rowA
            for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
              {
                int row = *i;
	        bool foundPlace = false;
	        // find a place in rowA for current col
	        for(int j=startLoc; j<lastLoc; j++)
	          if(rowA(j) > row)
                    { 
	              // move the entries already there one further on
	              // and place col in current location
	              for(int k=lastLoc; k>j; k--)
			rowA(k) = rowA(k-1);
	              rowA(j) = row;
	              foundPlace = true;
	              j = lastLoc;
	            }
	        if(!foundPlace) // put in at the end
	          rowA(lastLoc)= row;
                lastLoc++;
	      }
	    colStartA(a+1)= lastLoc;;	    
	    startLoc = lastLoc;
          }
      }
    // invoke setSize() on the Solver    
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
	std::cerr << "WARNING:XC::SparseGenColLinSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
      }   
    return result;
  }

int XC::SparseGenColLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if (fact == 0.0)  
	return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if (idSize != m.noRows() && idSize != m.noCols()) {
	std::cerr << "SparseGenColLinSOE::addA() ";
	std::cerr << " - Matrix and XC::ID not of similar sizes\n";
	return -1;
    }
    
    if (fact == 1.0) { // do not need to multiply 
      for (int i=0; i<idSize; i++) {
	int col = id(i);
	if (col < size && col >= 0) {
	  int startColLoc = colStartA(col);
	  int endColLoc = colStartA(col+1);
	  for (int j=0; j<idSize; j++) {
	    int row = id(j);
	    if (row <size && row >= 0) {
	      // find place in A using rowA
	      for (int k=startColLoc; k<endColLoc; k++)
		if(rowA(k) == row) {
		  A[k] += m(j,i);
		  k = endColLoc;
		}
	    }
	  }  // for j		
	} 
      }  // for i
    } else {
      for (int i=0; i<idSize; i++) {
	int col = id(i);
	if (col < size && col >= 0) {
	  int startColLoc = colStartA(col);
	  int endColLoc = colStartA(col+1);
	  for (int j=0; j<idSize; j++) {
	    int row = id(j);
	    if (row <size && row >= 0) {
	      // find place in A using rowA
	      for (int k=startColLoc; k<endColLoc; k++)
		if(rowA(k) == row) {
		  A[k] += fact * m(j,i);
		  k = endColLoc;
		}
	    }
	  }  // for j		
	} 
      }  // for i
    }
    return 0;
  }

    
int XC::SparseGenColLinSOE::sendSelf(CommParameters &cp)
  { return 0; }

int XC::SparseGenColLinSOE::recvSelf(const CommParameters &cp)  
  { return 0; }

