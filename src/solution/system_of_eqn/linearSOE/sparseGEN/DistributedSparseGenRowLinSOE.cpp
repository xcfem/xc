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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:20:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.cpp,v $
                                                                        
// Written: fmk 
// Created: 04/05
//
// Description: This file contains the implementation for XC::DistributedSparseGenRowLinSOE

#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>

#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::DistributedSparseGenRowLinSOE::DistributedSparseGenRowLinSOE(SoluMethod *owr)
  :SparseGenSOEBase(owr,LinSOE_TAGS_DistributedSparseGenRowLinSOE),
   colA(0), rowStartA(0), 
   numP(0), rank(-1), startRow(-1), endRow(-1), numRows(0),
   rows(0), otherProcessesRows(0), otherProcessesRowStart(0){}


bool XC::DistributedSparseGenRowLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    DistributedSparseGenRowLinSolver *tmp= dynamic_cast<DistributedSparseGenRowLinSolver *>(newSolver);
    if(tmp)
      retval= SparseGenSOEBase::setSolver(tmp);
    else
      std::cerr << "DistributedSparseGenRowLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

int XC::DistributedSparseGenRowLinSOE::setSize(Graph &theGraph)
{
  int result = 0;
  /*
  size = theGraph.getNumVertex();

  //
  // first we build an XC::ID containing all local DOFs
  //

  myDOFs.resize(size);

  int count = 0;
  Vertex *theVertex;
  VertexIter &theVertices = theGraph.getVertices();
  while ((theVertex = theVertices()) != 0) {
    int vertexTag = theVertex->getTag();
    myDOFs(count) = vertexTag;
    count++;
  }

  static XC::ID otherSize(1);
  ID otherDOFS(0, size/10);

  if(processID != 0) {

    //
    // each process send it's local and receives all other processes IDs (remote IDs).
    // from the remote IDs the local process figures out which DOF's it shares with others.
    // it then sends the shared dofs' out.
    //

    Channel *theChannel = theChannels[0];
    theChannel->recvID(0, 0, otherSize);
    numProcesses = otherSize(0);

    // send local
    otherSize(0) = size;
    theChannel->sendID(0, 0, otherSize);
    if(size != 0)
      theChannel->sendID(0, 0, myDOFs);

    for(int i=0; i<numProcesses-1; i++) {
      // receive remote & check for shared DOFs
      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);
      
      if(numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
	
	for(int j=0; j<numOther; j++) {
	  int otherTag = otherDOFS(j);
	  if(myDOFs.getLocation(otherTag) != -1 && myDOFsShared.getLocation(otherTag) == -1) 
	    myDOFsShared[numShared++] = otherTag;
	}
      }
    }

    // send my shared DOFs
    otherSize(0) = myDOFsShared.Size();
    theChannel->sendID(0, 0, otherSize);
    if(size != 0)
      theChannel->sendID(0, 0, myDOFsShared);

    // recv all shared DOFs
    theChannel->recvID(0, 0, otherSize);	
    int numShared = otherSize(0);
    if(numShared != 0) {
      myDOFsShared.resize(numShared);
      theChannel->recvID(0, 0, myDOFsShared);
    } 

  } else { // in absence of a broadcast we need the following

    //
    // each process send it's local and receives all other processes IDs (remote IDs).
    // from the remote IDs the local process figures out which DOF's it shares with others.
    // it then sends the shared dofs' out. lastly receives from P0 list of all shared.
    // NOTE:  if all sent to P0 their XC::ID's; P0 could figure out shared; but scalability issues
    //

    for(int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      otherSize(0) = numChannels+1;
      theChannel->sendID(0, 0, otherSize);

      otherSize(0) = size;
      theChannel->sendID(0, 0, otherSize);
      if(size != 0)
	theChannel->sendID(0, 0, myDOFs);
    }

    for(int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];

      // receive remote & check for shared DOFs

      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);
      if(numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
      }

      // need to send to all others
      for(int k=0; k<numChannels; k++) {
	Channel *theChannel = theChannels[k];
	if(k != j) {
	  theChannel->sendID(0, 0, otherSize);	
	  if(numOther != 0) {
	    theChannel->sendID(0, 0, otherDOFS);	    
	  }
	}
      }
      
      // need to merge with mine 
      for(int l=0; l<numOther; l++) {
	int otherTag = otherDOFS(l);
	if(myDOFs.getLocation(otherTag) != -1 && myDOFsShared.getLocation(otherTag) == -1) { 
	  myDOFsShared[numShared++] = otherTag;
	}
      }
    }

    // now recv each of the shared DOFs & merge with mine to form the master list
    for(int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);
      if(numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
      } 
      // need to merge with mine 
      for(int k=0; k<numOther; k++) {
	int otherTag = otherDOFS(k);
	if(myDOFsShared.getLocation(otherTag) == -1) 
	  myDOFsShared(numShared++) = otherTag;
      }
    }

    // now send the shared XC::ID
    for(int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      otherSize(0) = numShared;
      theChannel->sendID(0, 0, otherSize);	
      theChannel->sendID(0, 0, myDOFsShared);
    } 
  }

  //
  // now master list has been created we recv from each process the edges corresponding
  // to shared vertices.
  // 



  // 
  // determine system size
  //
  
  // first determine the largest in graph on local
  int myN = 0;
  VertexIter &theVertices = theGraph.getVertices();
  Vertex *theVertex;
  while ((theVertex = theVertices()) != 0) {
    if(theVertex->getTag() > myN)
      myN = theVertex->getTag();
  }
    
  // each process sends local, size is max of them all
  size = myN;
  for(int i=0; i<numP; i++) {
    static XC::ID rankSendRecv(1);
    if(rank == i) {
      rankSendRecv = myN;
      theChannel->sendID(0,0,rankSendRecv);
    } else {
      theChannel->recvID(0,0,rankSendRecv);
      if(rankSendRecv(0) > n)
	n = rankSendRecv(0);
    }
  }
  
  //
  // now determine rows assigned to each process
  //
  
  numRows = n/numP;
  startRow = numRows*rank;
    endRow = numRows*(rank+1);
  } else {
    startRow = numRows*(numP-1);
    endRow = n;
  }
  
  numRows = endRow-startRow;

    //
    // now create a graph of all rows assigned to processor & any rows
    // that are included from local graph (theGraph passed in method call)
    //

    //
    // graph must obviously contain the original plus any new_ Vertices from startRow
    // through endRow not already in XC::Graph; rows XC::ID must also contain any vertex in XC::Graph
    // but not in current startRow through endRow.
    //

    Graph myGraph(theGraph);
    rows.resize(numRows);

    for(int i=0; i<numRows; i++) {
      int row = startRow + i;
      rows(i) = row;
      if((theVertex = myGraph.getVertexPtr(row)) == 0) {
	theVertex=new_ Vertex(row, row);
	if(myGraph.addVertex(theVertex, false) == false) {
	  std::cerr << "WARNING XC::DistributedDistributedSparseGenRowLinSOE::addVertex - error adding vertex\n";
	}
      }
    }
      
    int numAllRows = numRows;
    VertexIter &theVertices2 = myGraph.getVertices();
    while ((theVertex = theVertices2()) != 0) {      
      int row = theVertex->getTag();
      int rowLoc = rows.getLocation(row);
      if(rowLoc == -1) {
	rows[numAllRows] = row;
	numAllRows++;
      }
    }

    rowStartA= ID(numAllRows+1); 

    // graph must also contain all edges that exist in other graphs for vertices
    // in numRows; we will only add edges & not vertices; graph will end up undirected
    // which is why we can't use addEdge XC::Graph method but must add edges to Vertices

    for(int i=0; i<numP; i++) {
      if(rank == i) {

	//
	// of course we only need to send the vertices of the graph that we are not responsible for!
	//

	Graph graphToSend;
	Vertex *theVertex;
	VertexIter &theVertices = theGraph.getVertices();	
	while ((theVertex = theVertices()) != 0) {
	  int vertexTag = theVertex->getTag();
	  if(vertexTag < startRow && vertexTag >= endRow) {
	    Vertex *copyVertex=new_ Vertex(*theVertex);
	    graphToSend.addVertex(copyVertex, false);
	  }
	}

	// now send this new_ graph
	graphToSend.sendSelf(0, *theChannel);
      } else {

	// receive the graph, for each vertex in recvd graph, if vertex is in myGraph
	// add any edges not already present in newly recvd graph to vertex in myGraph.

	FEM_ObjectBroker theBroker;
	Graph otherGraph;
	otherGraph.recvSelf(0, *theChannel, theBroker);
	VertexIter &theVertices = otherGraph.getVertices();
	Vertex *otherVertex;
	while ((otherVertex = theVertices()) != 0) {
	  int otherTag = otherVertex->getTag();
	  int otherTagLoc = rows.getLocation(otherTag);
	  if(otherTagLoc >= 0) {
	    theVertex = myGraph.getVertexPtr(otherTag);
	    const ID &otherAdjacency = otherVertex->getAdjacency();
	    for(int j=0; j<otherAdjacency.Size(); j++) {
	      theVertex->addEdge(otherAdjacency(j));
	    }
	  }
	}
      }
    }

    int newNNZ = 0;
    VertexIter &theVertices3 = myGraph.getVertices();
    while ((theVertex = theVertices3()) != 0) {
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	newNNZ += theAdjacency.size() +1; // the +1 is for the diag entry
    }
    nnz = newNNZ;

    if(newNNZ > Asize) { // we have to get more space for A and colA
	if(A != 0) 
	    delete [] A;
	if(colA != 0)
	    delete [] colA;
	
	A.resize(newNNZ);
	colA.resize(newNNZ);
	
        if(A == 0 || colA == 0) {
            std::cerr << "WARNING XC::DistributedSparseGenRowLinSOE::DistributedSparseGenRowLinSOE :";
	    std::cerr << " ran out of memory for A and colA with nnz = ";
	    std::cerr << newNNZ << " \n";
	    size = 0; Asize = 0; nnz = 0;
	    result =  -1;
        } 
	
	Asize = newNNZ;
    }

    // zero the matrix
    for(int i=0; i<Asize; i++)
	A[i] = 0;
	
    factored = false;
    
    if(n > Bsize) { // we have to get space for the vectors
	
	// delete the old	
	if(B != 0) delete [] B;
	if(X != 0) delete [] X;
	if(rowStartA != 0) delete [] rowStartA;

	// create the new
	B.resize(n);
	X.resize(n);
	
        if(B == 0 || X == 0) {
            std::cerr << "WARNING XC::DistributedSparseGenRowLinSOE::DistributedSparseGenRowLinSOE :";
	    std::cerr << " ran out of memory for vectors (size) (";
	    std::cerr << size << ") \n";
	    size = 0; Bsize = 0;
	    result =  -1;
        }
	else
	    Bsize = n;
    }

    // zero the vectors
    for(int j=0; j<n; j++) {
	B[j] = 0;
	X[j] = 0;
    }
    
    // create new_ Vectors objects
    if(n != oldSize) {
	if(vectX != 0)
	    delete vectX;

	if(vectB != 0)
	    delete vectB;
	
	vectX= Vector(X, n);
	vectB= Vector(B, n);	
    }

    // fill in rowStartA and colA
    if(n != 0) {
      rowStartA[0] = 0;
      int startLoc = 0;
      int lastLoc = 0;
      for(int a=0; a<numAllRows; a++) {

	theVertex = theGraph.getVertexPtr(rows(a));
	if(theVertex == 0) {
	  std::cerr << "WARNING:XC::DistributedSparseGenRowLinSOE::setSize :";
	  std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	  size = 0;
	  return -1;
	}

	colA[lastLoc++] = theVertex->getTag(); // place diag in first
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	int idSize = theAdjacency.size();
	
	// now we have to place the entries in the XC::ID into order in colA
	for(int i=0; i<idSize; i++) {

	  int row = theAdjacency(i);
	  bool foundPlace = false;
	  // find a place in colA for current col
	  for(int j=startLoc; j<lastLoc; j++)
	    if(colA[j] > row) { 
	      // move the entries already there one further on
	      // and place col in current location
	      for(int k=lastLoc; k>j; k--)
		
		colA[k] = colA[k-1];
	      colA[j] = row;
	      foundPlace = true;
	      j = lastLoc;
	    }
	  if(foundPlace == false) // put in at the end
	    colA[lastLoc] = row;

	  lastLoc++;
	}
	rowStartA[a+1] = lastLoc;;	    
	startLoc = lastLoc;
      }
    }
    
    // invoke setSize() on the XC::Solver    
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0) {
	std::cerr << "WARNING:XC::DistributedSparseGenRowLinSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
    }    
  */
    return result;
}

int XC::DistributedSparseGenRowLinSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0)  
      return 0;

    const int idSize= id.Size();
    
    // check that m and id are of similar n
    if(idSize != m.noRows() && idSize != m.noCols())
      {
	std::cerr << "DistributedSparseGenRowLinSOE::addA() ";
	std::cerr << " - Matrix and XC::ID not of similar ns\n";
	return -1;
      }
    
    for(int i=0; i<idSize; i++)
      {
        const int row = id(i);
        if(row < size && row >= 0)
          {
	    int localRow = rows.getLocation(row);
	    if(localRow == -1) 
	      std::cerr << "DistributedSparseGenRowLinSOE::addA() - you goofed\n";
	int startRowLoc = rowStartA[localRow];
	int endRowLoc = rowStartA[localRow+1];
	for(int j=0; j<idSize; j++) {
	  int col = id(j);
	  if((col < size) && (col >= 0)) {
	    // find place in A using colA
	    for(int k=startRowLoc; k<endRowLoc; k++)
	      if(colA[k] == col) {
		A[k] += m(i,j) * fact;
		k = endRowLoc;
	      }
	  }
	}  // for j		
      } 
    }  // for i

    return 0;
}

int XC::DistributedSparseGenRowLinSOE::sendSelf(CommParameters &cp)
  {
    const int retval= send(cp);
    return retval;
  }


int XC::DistributedSparseGenRowLinSOE::recvSelf(const CommParameters &cp)
  {
    const int retval= receive(cp);
    return retval;
  }





