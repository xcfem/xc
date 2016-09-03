//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
