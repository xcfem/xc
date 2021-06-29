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
                                                                        
// $Revision: 1.6 $
// $Date: 2009-05-11 20:56:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsSOE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 02/06

// Description: This file contains the implementation for MumpsSOE

#include "solution/system_of_eqn/linearSOE/mumps/MumpsSOE.h"
#include "solution/system_of_eqn/linearSOE/mumps/MumpsSolver.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "solution/graph/graph/VertexIter.h"
#include <cmath>

XC::MumpsSOE::MumpsSOE(SolutionStrategy *owr)
  :SparseGenSOEBase(owr, LinSOE_TAGS_MumpsSOE),
  Asize(0), matType(0)
  {}

XC::MumpsSOE::MumpsSOE(SolutionStrategy *owr, int classTag)
  : SparseGenSOEBase(owr, classTag),
  Asize(0), matType(0)
  {}

// XC::MumpsSOE::MumpsSOE(SolutionStrategy *owr, MumpsSolver &the_Solver, int _matType)
//   : SparseGenSOEBase(owr, the_Solver, LinSOE_TAGS_MumpsSOE),
//     Asize(0), matType(_matType)
//   {
//     the_Solver.setSparseGenSOEBase(*this);
//   }



// XC::MumpsSOE::MumpsSOE(SolutionStrategy *owr, LinearSOESolver &the_Solver, int classTag, int _matType)
//   :SparseGenSOEBase(owr, the_Solver, classTag),
//    Asize(0), matType(_matType)
//   {}


XC::MumpsSOE::~MumpsSOE(void)
  {}

int XC::MumpsSOE::setMumpsSolver(MumpsSolver &newSolver)
  {
    return this->setSolver(&newSolver);
  }

int XC::MumpsSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    int oldSize = size;
    size = theGraph.getNumVertex();

    // fist itearte through the vertices of the graph to get nnz
    Vertex *theVertex;
    int newNNZ = 0;
    VertexIter &theVertices = theGraph.getVertices();
    while ((theVertex = theVertices()) != 0)
      {
        const std::set<int> &theAdjacency = theVertex->getAdjacency();
        newNNZ += theAdjacency.size()+1; // the +1 is for the diag entry
      }

    if (matType !=  0) {
      newNNZ -= size;
      newNNZ /= 2;
      newNNZ += size;
    }

    nnz = newNNZ;

    if (newNNZ > Asize)
      { // we have to get more space for A and rowA
	 A.resize(newNNZ);
	 rowA.resize(newNNZ);
	 colA.resize(newNNZ);

         Asize = newNNZ;
      }

    // zero the matrix
    A.Zero();

    factored = false;

    if (size > Bsize)
      { // we have to get space for the vectors

      // resize
	B.resize(size);
        X.resize(size);
        colStartA.resize(size+1); 
	Bsize = size;
      }

    // zero the vectors
    B.Zero();
    X.Zero();
    
    // fill in colStartA and rowA
    if(size != 0)
      {
      colStartA[0] = 0;
      int startLoc = 0;
      int lastLoc = 0;
      for (int a=0; a<size; a++) {

	theVertex = theGraph.getVertexPtr(a);
	if (theVertex == 0) {
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; WARNING vertex " << a
		    << " not in graph! - size set to 0\n";
	  size = 0;
	  return -1;
	}

	int vertexTag = theVertex->getTag();
	rowA[lastLoc++] = vertexTag; // place diag in first
	const std::set<int> &theAdjacency = theVertex->getAdjacency();

	// now we have to place the entries in the ID into order in rowA

	if (matType != 0) {

      for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
	{
	    int row= *i;
	    if (row > vertexTag) {
	      bool foundPlace = false;
	      // find a place in rowA for current col
	      for (int j=startLoc; j<lastLoc; j++)
		if (rowA[j] > row) { 
		  // move the entries already there one further on
		  // and place col in current location
		  for (int k=lastLoc; k>j; k--)
		    rowA[k] = rowA[k-1];
		  rowA[j] = row;
		  foundPlace = true;
		  j = lastLoc;
		}

	      if (foundPlace == false) // put in at the end
		rowA[lastLoc] = row;
	      lastLoc++;
	    }
	  }

	} else {

      for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
	  {
	    int row = *i;
	    bool foundPlace = false;
	    // find a place in rowA for current col
	    for (int j=startLoc; j<lastLoc; j++)
	      if (rowA[j] > row) { 
		// move the entries already there one further on
		// and place col in current location
		for (int k=lastLoc; k>j; k--)
		  rowA[k] = rowA[k-1];
		rowA[j] = row;
		foundPlace = true;
		j = lastLoc;
	      }
	    if (foundPlace == false) // put in at the end
	      rowA[lastLoc] = row;

	    lastLoc++;
	  }
	}

	colStartA[a+1] = lastLoc;;	    
	startLoc = lastLoc;
      }
    }

    // fill in colA
    int count = 0;
    for (int i=0; i<size; i++)
      for (int k=colStartA[i]; k<colStartA[i+1]; k++)
	colA[count++] = i;

    // invoke setSize() on the Solver    
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if (solverOK < 0) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "WARNING solver failed setSize()\n";
      return solverOK;
    }    

    return result;
  }

int XC::MumpsSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if (fact == 0.0)  
	return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if (idSize != m.noRows() && idSize != m.noCols()) {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes\n";
	return -1;
    }

    if (matType != 0) {

      if (fact == 1.0) { // do not need to multiply 
	for (int i=0; i<idSize; i++) {
	  int col = id(i);
	  if (col < size && col >= 0) {
	    int startColLoc = colStartA[col];
	    int endColLoc = colStartA[col+1];
	    
	    for (int j=0; j<idSize; j++) {
	      int row = id(j);
	      if (row >= col && row < size && row >= 0) {
		// find place in A using rowA
		for (int k=startColLoc; k<endColLoc; k++)
		  if (rowA[k] == row) {
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
	    int startColLoc = colStartA[col];
	    int endColLoc = colStartA[col+1];

	    for (int j=0; j<idSize; j++) {
	      int row = id(j);
	      if (row >= col && row <size && row >= 0) {
		// find place in A using rowA
		for (int k=startColLoc; k<endColLoc; k++)
		  if (rowA[k] == row) {
		    A[k] += fact * m(j,i);
		    k = endColLoc;
		  }
	      }
	    }  // for j		
	  } 
	}  // for i
      }


    } else {
    
      if (fact == 1.0) { // do not need to multiply 
	for (int i=0; i<idSize; i++) {
	  int col = id(i);
	  if (col < size && col >= 0) {
	    int startColLoc = colStartA[col];
	    int endColLoc = colStartA[col+1];

	    for (int j=0; j<idSize; j++) {
	      int row = id(j);
	      if (row <size && row >= 0) {
		// find place in A using rowA
		for (int k=startColLoc; k<endColLoc; k++)
		  if (rowA[k] == row) {
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
	    int startColLoc = colStartA[col];
	    int endColLoc = colStartA[col+1];
	    for (int j=0; j<idSize; j++) {
	      int row = id(j);
	      if (row <size && row >= 0) {
		// find place in A using rowA
		for (int k=startColLoc; k<endColLoc; k++)
		  if (rowA[k] == row) {
		    A[k] += fact * m(j,i);
		    k = endColLoc;
		  }
	      }
	    }  // for j		
	  } 
	}  // for i
      }
    }

    return 0;
  }

    
int XC::MumpsSOE::sendSelf(Communicator &comm)
  { return 0; }

int XC::MumpsSOE::recvSelf(const Communicator &comm)  
  { return 0; }

