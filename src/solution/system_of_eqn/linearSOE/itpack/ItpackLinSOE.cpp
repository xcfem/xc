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
// $Date: 2003/02/14 23:02:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/ItpackLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// ItpackLinSolver. It solves the ItpackLinSOEobject by calling
// UMFPACK2.2.1 routines.
//
// What: "@(#) ItpackLinSolver.h, revA"

#include <fstream>

#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSOE.h>
#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>

XC::ItpackLinSOE::ItpackLinSOE(SoluMethod *owr)
  :LinearSOEData(owr,LinSOE_TAGS_ItpackLinSOE), nnz(0), Aformed(false) {}

bool XC::ItpackLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    ItpackLinSolver *tmp= dynamic_cast<ItpackLinSolver *>(newSolver);
    if(tmp)
      retval= LinearSOEData::setSolver(tmp);
    else
      std::cerr << "BandGenLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

int XC::ItpackLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;
  
  // fist itearte through the vertices of the graph to get nnz
  Vertex *theVertex;
  int newNNZ = 0;
  VertexIter &theVertices = theGraph.getVertices();
  while((theVertex = theVertices()) != 0)
    {
      const std::set<int> &theAdjacency = theVertex->getAdjacency();
      newNNZ += theAdjacency.size() +1; // the +1 is for the diag entry
    }
  nnz = newNNZ;
  
    std::cerr << "ItpackLinSOE::setSize - n " << size << " nnz " << nnz << std::endl;
  
    if(nnz > A.Size())
      { // we have to get more space for A and colA
    
        A.resize(nnz);
        colA.resize(nnz);
      }
    A.Zero();
    Aformed= false;

    if(size > B.Size()) // we have to get space for the vectors
      { 
    
        // create the new
        inic(size);
        rowStartA.resize(size+1); 
    
      }
  
    // fill in rowStartA and colA
    if(size != 0)
      {
        rowStartA[0] = 0;
        int startLoc = 0;
        int lastLoc = 0;
        for(int a=0; a<size; a++) {
      
      theVertex = theGraph.getVertexPtr(a);
      if (theVertex == 0) {
	std::cerr << "WARNING:XC::ItpackLinSOE::setSize :";
	std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	size = 0;
	return -1;
      }
      
      colA[lastLoc++] = theVertex->getTag(); // place diag in first
      const std::set<int> &theAdjacency = theVertex->getAdjacency();
      
      // now we have to place the entries in the XC::ID into order in colA
      for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
        {
	
	int row = *i;
	bool foundPlace = false;
	// find a place in colA for current col
	for (int j=startLoc; j<lastLoc; j++)
	  if (colA[j] > row) { 
	    // move the entries already there one further on
	    // and place col in current location
	    for (int k=lastLoc; k>j; k--)
	      colA[k] = colA[k-1];
	    colA[j] = row;
	    foundPlace = true;
	    j = lastLoc;
	  }
	if (foundPlace == false) // put in at the end
	  colA[lastLoc] = row;
	
	lastLoc++;
      }
      rowStartA[a+1] = lastLoc;	    
      startLoc = lastLoc;
    }
  }
  
  /*
  // fill out index
  int *indexRowPtr = &index[0];
  int *indexColPtr = &index[nnz];
  for (int ii=0; ii<size; ii++) {
    int rowBegin = rowStartA[ii];
    int rowEnd = rowStartA[ii+1] -1;
    for (int j=rowBegin; j<=rowEnd; j++) {
      int row = ii+1;  // + 1 for fortarn indexing
      int col = colA[j] +1; // +1 for fortran indexing
      *indexRowPtr++ = row;
      *indexColPtr++ = col;
    }
  }
  */

  Aformed = false;

  // invoke setSize() on the XC::Solver    
  LinearSOESolver *the_Solver = this->getSolver();
  int solverOK = the_Solver->setSize();
  if (solverOK < 0) {
    std::cerr << "WARNING:XC::ItpackLinSOE::setSize :";
    std::cerr << " solver failed setSize()\n";
    return solverOK;
  }    
  return result;
}

int XC::ItpackLinSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
{
  // check for a XC::quick return 
  if (fact == 0.0)  
    return 0;
  
  int idSize = id.Size();
  
  // check that m and id are of similar size
  if (idSize != m.noRows() && idSize != m.noCols()) {
    std::cerr << "XC::ItpackLinSOE::addA() ";
    std::cerr << " - Matrix and XC::ID not of similar sizes\n";
    return -1;
  }
  
  if (fact == 1.0) { // do not need to multiply 
    for (int i=0; i<idSize; i++) {
      int row = id(i);
      if (row < size && row >= 0) {
	int startRowLoc = rowStartA[row];
	int endRowLoc = rowStartA[row+1];
	for (int j=0; j<idSize; j++) {
	  int col = id(j);
	  if (col <size && col >= 0) {
	    // find place in A using colA
	    for (int k=startRowLoc; k<endRowLoc; k++)
	      if (colA[k] == col) {
		A[k] += m(i,j);
		k = endRowLoc;
	      }
	  }
	}  // for j		
      } 
    }  // for i
  }
  else {
    for (int i=0; i<idSize; i++) {
      int row = id(i);
      if (row < size && row >= 0) {
	int startRowLoc = rowStartA[row];
	int endRowLoc = rowStartA[row+1];
	for (int j=0; j<idSize; j++) {
	  int col = id(j);
	  if (col <size && col >= 0) {
	    // find place in A using colA
	    for (int k=startRowLoc; k<endRowLoc; k++)
	      if (colA[k] == col) {
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

void XC::ItpackLinSOE::zeroA(void)
  {
    A.Zero();
    Aformed = false;
  }

int XC::ItpackLinSOE::sendSelf(CommParameters &cp)
  { return -1; }

int XC::ItpackLinSOE::recvSelf(const CommParameters &cp)
  { return -1; }
