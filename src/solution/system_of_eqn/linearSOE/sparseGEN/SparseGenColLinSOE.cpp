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

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
XC::SparseGenColLinSOE::SparseGenColLinSOE(SolutionStrategy *owr)
  :SparseGenSOEBase(owr,LinSOE_TAGS_SparseGenColLinSOE) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: class identifier.
XC::SparseGenColLinSOE::SparseGenColLinSOE(SolutionStrategy *owr,int classTag)
  : SparseGenSOEBase(owr,classTag) {}

XC::SystemOfEqn *XC::SparseGenColLinSOE::getCopy(void) const
  { return new SparseGenColLinSOE(*this); }

//! @brief Set the solver to use.
//!
//! Invokes {\em setLinearSOE(*this)} on \p newSolver.
//! If the system size is not equal to \f$0\f$, it also invokes setSize()
//! on \p newSolver, printing a warning and returning \f$-1\f$ if this
//! method returns a number less than \f$0\f$. Finally it returns the result
//! of invoking the LinearSOE classes setSolver() method.
bool XC::SparseGenColLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    SparseGenColLinSolver *tmp= dynamic_cast<SparseGenColLinSolver *>(newSolver);
    if(tmp)
      retval= SparseGenSOEBase::setSolver(tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; solver type incompatible with this system of equations."
		<< std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
//!
//! The size of the system is determined from the Graph object {\em
//! theGraph}, which must contain \p size vertices labelled \f$0\f$ through
//! \f$size-1\f$, the adjacency list for each vertex containing the associated
//! vertices in a column of the matrix \f$A\f$. The size is determined by
//! invoking getNumVertex() on \p theGraph and the number of
//! non-zeros is determined by looking at the size of the adjacenecy list
//! of each vertex in the graph, allowing space for the diagonal term. If
//! the old space allocated for the 1d arrays is not big enough, it the
//! old space is returned to the heap and new space is allocated from the
//! heap. Prints a warning message, sets size to \f$0\f$ and returns a \f$-1\f$,
//! if not enough memory is available on the heap for the 1d arrays. If
//! memory is available, the components of the arrays are 
//! zeroed and \f$A\f$ is marked as being unfactored. If the system size has
//! increased, new Vector objects for \f$x\f$ and \f$b\f$ using the {\em (double
//! *,int)} Vector constructor are created. The \f$colStartA\f$ and \f$rowA\f$
//! are then determined by looping through the vertices, setting \f$colStartA(i)
//! = colStartA(i-1) + 1 + \f$the size of Vertices \f$i\f$ adjacency list and 
//! placing the contents of \f$i\f$ and the adjacency list into \f$rowA\f$ in
//! ascending order. Finally, the result of invoking setSize() on
//! the associated Solver object is returned.
int XC::SparseGenColLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);

    // fist iterate through the vertices of the graph to get nnz
    Vertex *theVertex= nullptr;
    int newNNZ= 0;
    VertexIter &theVertices = theGraph.getVertices();
    while((theVertex = theVertices()) != nullptr)
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
	
    factored= false;
    
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
	    if(theVertex == nullptr)
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING :"
			  << " vertex " << a
			  << " not in graph! - size set to 0.\n";
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
    int solverOK= the_Solver->setSize();
    if(solverOK < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING :"
		  << " solver failed setSize()\n";
	return solverOK;
      }
    return result;
  }

//! @brief Assemblies the product fact*m into the system matrix.
//!
//! First tests that \p loc and \p M are of compatible sizes; if not
//! a warning message is printed and a \f$-1\f$ is returned. The LinearSOE
//! object then assembles \p fact times the Matrix {\em 
//! M} into the matrix \f$A\f$. The Matrix is assembled into \f$A\f$ at the
//! locations given by the ID object \p loc, i.e. \f$a_{loc(i),loc(j)} +=
//! fact * M(i,j)\f$. If the location specified is outside the range,
//! i.e. \f$(-1,-1)\f$ the corrseponding entry in \p M is not added to
//! \f$A\f$. If \p fact is equal to \f$0.0\f$ or \f$1.0\f$, more efficient steps
//! are performed. Returns \f$0\f$.
int XC::SparseGenColLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0)  
	return 0;

    const int idSize = id.Size();
    
    // check that m and id are of similar size
    if(idSize != m.noRows() && idSize != m.noCols())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Matrix and ID not of similar sizes\n";
	return -1;
      }
    
    if(fact == 1.0)
      { // do not need to multiply 
	for(int i=0; i<idSize; i++)
	  {
	    const int col= id(i);
	    if(col < size && col >= 0)
	      {
		const int startColLoc= colStartA(col);
		const int endColLoc= colStartA(col+1);
		for(int j=0; j<idSize; j++)
		  {
		    const int row= id(j);
		    if(row <size && row >= 0)
		      {
			// find place in A using rowA
			for(int k=startColLoc; k<endColLoc; k++)
			  {
			    if(rowA(k) == row)
			      {
			        A[k]+= m(j,i);
			        break; //k = endColLoc;
			      }
			  }
		      }
		  }  // for j		
	      } 
	  } // for i
      }
    else
      {
	for(int i=0; i<idSize; i++)
	  {
	    const int col = id(i);
	    if(col < size && col >= 0)
	      {
		const int startColLoc = colStartA(col);
		const int endColLoc = colStartA(col+1);
		for(int j=0; j<idSize; j++)
		  {
		    const int row = id(j);
		    if(row <size && row >= 0)
		      {
			// find place in A using rowA
			for(int k= startColLoc; k<endColLoc; k++)
			  if(rowA(k) == row)
			    {
			      A[k]+= fact * m(j,i);
			      break; //k= endColLoc;
			    }
		      }
		  }  // for j
	      } 
	  }  // for i
      }
    return 0;
  }

    
int XC::SparseGenColLinSOE::sendSelf(Communicator &comm)
  { return 0; }

int XC::SparseGenColLinSOE::recvSelf(const Communicator &comm)  
  { return 0; }

