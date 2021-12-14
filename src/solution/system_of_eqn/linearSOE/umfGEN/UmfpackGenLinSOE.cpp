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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:02:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// UmfpackGenLinSolver. It solves the UmfpackGenLinSOEobject by calling
// UMFPACK2.2.1 routines.
//
// What: "@(#) UmfpackGenLinSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>


XC::UmfpackGenLinSOE::UmfpackGenLinSOE(SolutionStrategy *owr)
  :LinearSOEData(owr,LinSOE_TAGS_UmfpackGenLinSOE), Ax(), Ap(), Ai()
  {}

XC::SystemOfEqn *XC::UmfpackGenLinSOE::UmfpackGenLinSOE::getCopy(void) const
  { return new UmfpackGenLinSOE(*this); }

bool XC::UmfpackGenLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    UmfpackGenLinSolver *tmp= dynamic_cast<UmfpackGenLinSolver *>(newSolver);
    if(tmp)
      {
	retval= LinearSOEData::setSolver(tmp);
	if(X.Size()!=0)
	  {
	    int solverOK= newSolver->setSize();
	    if(solverOK < 0)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING: the new solver could not setSize().\n";
		return -1;
	    }
	}	
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; solver not compatible with this system of equations."
		<< std::endl;
    return retval;
  }

//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::UmfpackGenLinSOE::setSize(Graph &theGraph)
  {
    size= checkSize(theGraph);
    if(size < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          <<"; size of soe < 0\n";
	return -1;
      }

    // fist iterate through the vertices of the graph to get nnz
    Vertex *theVertex;
    int nnz= 0;
    VertexIter &theVertices = theGraph.getVertices();
    while ((theVertex = theVertices()) != 0)
      {
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	nnz += theAdjacency.size() +1; // the +1 is for the diag entry
      }

    // resize A, B, X
    Ap.reserve(size+1);
    Ai.reserve(nnz);
    Ax.resize(nnz,0.0);
    B.resize(size);
    B.Zero();
    X.resize(size);
    X.Zero();
    
    // fill in Ai and Ap
    Ap.push_back(0);
    for(int a=0; a<size; a++)
      {

	theVertex = theGraph.getVertexPtr(a);
	if (theVertex == 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING: vertex " << a
		      << " not in graph! - size set to 0.\n";
	    size = 0;
	    return -1;
	  }

	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	std::set<int> col;

	// diagonal
	col.insert(theVertex->getTag());

	// now we have to place the entries in the ID into order in Ai
        for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
	  {
	    const int row= *i;
	    col.insert(row);
	  }

	// copy to Ai
	for(std::set<int>::const_iterator i=col.begin(); i!=col.end(); i++)
	  { Ai.push_back(*i); }

	// set Ap
	Ap.push_back(Ap[a]+col.size());
      }

    // invoke setSize() on the Solver
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if (solverOK < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: solver failed setSize().\n";
	return solverOK;
      }
    return 0;
  }


int XC::UmfpackGenLinSOE::addA(const Matrix &m, const ID &id, double fact)
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
    
    if(fact == 1.0) // do not need to multiply 
      { 
	for (int j=0; j<idSize; j++)
	  {
	    const int col = id(j);
	    if (col<0 || col>=size)
	      { continue; }
	    for (int i=0; i<idSize; i++)
	      {
		const int row = id(i);
		if (row<0 || row>=size)
		  { continue; }

		// find place in A
		for (int k=Ap[col]; k<Ap[col+1]; k++)
		  {
		    if(Ai[k] == row)
		      {
			Ax[k] += m(i,j);
			break;
		      }
		  }
	      }
	  }
      }
    else
      {
	for(int j=0; j<idSize; j++)
	  {
	    const int col = id(j);
	    if (col<0 || col>=X.Size())
	      { continue; }
	    for(int i=0; i<idSize; i++)
	      {
		const int row = id(i);
		if (row<0 || row>=X.Size())
		  { continue; }
		// find place in A
		for (int k=Ap[col]; k<Ap[col+1]; k++)
		  {
		    if(Ai[k] == row)
		      {
			Ax[k] += fact*m(i,j);
			break;
		      }
		  }
	      }
	  }
      }
    return 0;
  }

    
void XC::UmfpackGenLinSOE::zeroA(void)
  {
    Ax.assign(Ax.size(),0.0);
  }

int XC::UmfpackGenLinSOE::sendSelf(Communicator &comm)
  {
    return 0;
  }

int XC::UmfpackGenLinSOE::recvSelf(const Communicator &comm)
  {
    return 0;
  }

