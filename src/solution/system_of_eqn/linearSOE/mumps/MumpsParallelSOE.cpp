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
                                                                        
// $Revision: 1.5 $
// $Date: 2009-05-11 20:56:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/mumps/MumpsParallelSOE.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for MumpsParallelSOE


#include "solution/system_of_eqn/linearSOE/mumps/MumpsParallelSOE.h"
#include "solution/system_of_eqn/linearSOE/mumps/MumpsParallelSolver.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/actor/channel/Channel.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "solution/graph/graph/VertexIter.h"
#include <cmath>
#include "utility/actor/actor/Communicator.h"

XC::MumpsParallelSOE::MumpsParallelSOE(SolutionStrategy *owr)
  : MumpsSOE(owr, LinSOE_TAGS_MumpsParallelSOE), 
   processID(0)
  {}

// XC::MumpsParallelSOE::MumpsParallelSOE(SolutionStrategy *owr, MumpsParallelSolver &the_Solver, int _matType)
//   : MumpsSOE(owr, the_Solver, LinSOE_TAGS_MumpsParallelSOE),
//   processID(0)
//   {
//     the_Solver.setMumpsSOE(*this);
//   }

//! @brief Virtual constructor.
XC::SystemOfEqn *XC::MumpsParallelSOE::getCopy(void) const
  { return new MumpsParallelSOE(*this); }

XC::MumpsParallelSOE::~MumpsParallelSOE()
  {
    theChannels.clear();
    localCol.clear();
  }

int XC::MumpsParallelSOE::setSize(Graph &theGraph)
  {
    int result= 0;
    //int oldSize= size;
    //int maxNumSubVertex= 0;

    // first iterate through the vertices of the graph to get nnz and n.
    int maxVertexTag = -1;
    Vertex *theVertex= nullptr;
    int newNNZ= 0;
    size = theGraph.getNumVertex();
    //opserr << "MumpsParallelSOE: size : " << size << std::endl;
    
    VertexIter &theVertices = theGraph.getVertices();
    while ((theVertex = theVertices()) != 0)
      {
        const std::set<int> &theAdjacency = theVertex->getAdjacency();
        newNNZ+= theAdjacency.size()+1; // the +1 is for the diag entry
      }

    if(matType !=  0)
      {
       // symmetric - allows us to reduce nnz by almost half
       newNNZ -= size;
        newNNZ /= 2;
        newNNZ += size;
      }

    nnz = newNNZ;
    if(processID != 0)
      {
	//
	// if subprocess, send local max vertexTag (n)
	// recv ax n from P0
	//
	static ID data(1);
	data(0)= maxVertexTag;
	Channel *theChannel = theChannels[0];
	theChannel->sendID(0, 0, data);
	theChannel->recvID(0, 0, data);
	size = data(0);
      }
    else
      {
	//
	// from each distributed soe recv it's max n and compare; return max
	// n to all.
	//

	static ID data(1);
	//FEM_ObjectBroker theBroker;
	const size_t numChannels= theChannels.size();
	for (size_t j=0; j<numChannels; j++)
	  {
	    theChannels[j]->recvID(0, 0, data);
	    if(data(0) > maxVertexTag)
	      maxVertexTag = data(0);
	  }

	data(0) = maxVertexTag;

	for(size_t j=0; j<numChannels; j++)
	  { theChannels[j]->sendID(0, 0, data); }
	size = maxVertexTag;
      }
    
    size+=1; // vertices numbered 0 through n-1

    if(nnz > Asize)
      { // we have to get more space for A and rowA
	 A.resize(newNNZ);
	 rowA.resize(newNNZ);
	 colA.resize(newNNZ);

         Asize= nnz;
      }

    // zero the matrix
    A.Zero();
    rowA.Zero();
    colA.Zero();

    factored = false;

    if(size > Bsize)
      { // we have to get space for the vectors

      // resize
	B.resize(size);
        X.resize(size);
	myB.resize(size);
	workArea.resize(size);
        colStartA.resize(size+1);
	
	Bsize = size;
      }

    // zero the vectors
    B.Zero();
    X.Zero();
    myB.Zero();

    
    // fill in colStartA and rowA
    if(size!=0)
      {
      colStartA[0] = 0;
      int startLoc = 0;
      int lastLoc = 0;
      for (int a=0; a<size; a++)
	{
	  theVertex = theGraph.getVertexPtr(a);
	  if(theVertex == 0)
	    {
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; WARNING vertex " << a
			<< " not in graph! - size set to 0\n";
	      size= 0;
	      return -1;
	    }

  	  const int vertexTag = theVertex->getTag();
	  rowA[lastLoc++]= vertexTag; // place diag in first
	  const std::set<int> &theAdjacency = theVertex->getAdjacency();

	  // now we have to place the entries in the ID into order in rowA
  	  if(matType != 0) // symmetric
	    {

	      for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
		{
		    int row= *i;
		    if(row > vertexTag)
		      {
			bool foundPlace = false;
			// find a place in rowA for current col
			for (int j=startLoc; j<lastLoc; j++)
			  if (rowA[j] > row)
			    { 
			      // move the entries already there one further on
			      // and place col in current location
			      for(int k=lastLoc; k>j; k--)
				rowA[k] = rowA[k-1];
			      rowA[j] = row;
			      foundPlace = true;
			      j = lastLoc;
			    }

			if(foundPlace == false) // put in at the end
			  rowA[lastLoc] = row;
			lastLoc++;
		      }
		}
	    }
	  else // unsymmetric
	    {
	      for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
		  {
		    int row = *i;
		    bool foundPlace = false;
		    // find a place in rowA for current col
		    for(int j=startLoc; j<lastLoc; j++)
		      if(rowA[j] > row)
			{ 
			  // move the entries already there one further on
			  // and place col in current location
			  for(int k=lastLoc; k>j; k--)
			    rowA[k] = rowA[k-1];
			  rowA[j] = row;
			  foundPlace = true;
			  j = lastLoc;
		        }
		    if(foundPlace == false) // put in at the end
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
    const int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING solver failed setSize()\n";
        return solverOK;
      }
    return result;
  }

int XC::MumpsParallelSOE::solve(void)
  {
    int resSolver = 0;

    //
    // if subprocess send B, solve and recv back X and B
    //

    if(processID != 0)
      {
	// send B
	Channel *theChannel = theChannels[0];
	theChannel->sendVector(0, 0, myB);

	resSolver =  this->LinearSOE::solve();

	if(resSolver == 0)
	  {
	    // receive X,B and result
	    theChannel->recvVector(0, 0, X);
	    theChannel->recvVector(0, 0, B);
	    factored = true;
	  }
      }

    //
    // if main process, recv B & A from all, solve and send back X, B & result
    //

    else
      {
	// add P0 contribution to B
	B= myB;

	// receive B
	const size_t numChannels= theChannels.size();
	for(size_t j=0; j<numChannels; j++)
	  {
	    // get X & add
	    theChannels[j]->recvVector(0, 0, X);
	    B+= X;
	  }

	// solve
	resSolver = this->LinearSOE::solve();

	// send results back
	if(resSolver == 0)
	  {
	    for (size_t j=0; j<numChannels; j++)
	      {
		Channel *theChannel = theChannels[j];
		theChannel->sendVector(0, 0, X);
		theChannel->sendVector(0, 0, B);
	      }
	  }
      }
    return resSolver;
  }	

int XC::MumpsParallelSOE::addB(const Vector &v, const ID &id, double fact)
  {
    // check for a quick return 
    if (fact == 0.0)  return 0;

    int idSize = id.Size();    
    // check that m and id are of similar size
    if (idSize != v.Size() )
      {
        std::cerr << "SparseGenColLinSOE::addB() ";
        std::cerr << " - Vector and ID not of similar sizes\n";
        return -1;
      }    

    if (fact == 1.0) { // do not need to multiply if fact == 1.0
      for (int i=0; i<idSize; i++) {
	int pos = id(i);
	if (pos <size && pos >= 0)
	  myB[pos] += v(i);
      }
    } else if (fact == -1.0) { // do not need to multiply if fact == -1.0
      for (int i=0; i<idSize; i++) {
	int pos = id(i);
	if (pos <size && pos >= 0)
	  myB[pos] -= v(i);
      }
    } else {
      for (int i=0; i<idSize; i++) {
	int pos = id(i);
	if (pos <size && pos >= 0)
	  myB[pos] += v(i) * fact;
      }
    }

    return 0;
  }

int XC::MumpsParallelSOE::setB(const Vector &v, double fact)
  {
    // check for a quick return 
    if (fact == 0.0)  return 0;

    //std::cerr << "MumpsParallelSOE::setB() - start()\n";
    //std::cerr << v;

    if (v.Size() != size) {
      std::cerr << "WARNING DistributedBandGenLinSOE::setB() -";
      std::cerr << " incompatible sizes " << size << " and " << v.Size()
		<< std::endl;
      return -1;
    }

    if (fact == 1.0) { // do not need to multiply if fact == 1.0
      for (int i=0; i<size; i++) {
	myB[i] = v(i);
      }
    } else if (fact == -1.0) {
      for (int i=0; i<size; i++) {
	myB[i] = -v(i);
      }
    } else {
      for (int i=0; i<size; i++) {
	myB[i] = v(i) * fact;
      }
    }

     //std::cerr << "MumpsParallelSOE::setB() - end()\n";
    return 0;
  }

void XC::MumpsParallelSOE::zeroB(void)
  { myB.Zero(); }

const XC::Vector &XC::MumpsParallelSOE::getB(void) const
  {
    XC::Vector &mutableB= const_cast<XC::Vector &>(B);
    if(processID != 0)
      {
        Channel *theChannel = theChannels[0];
        // send B & recv merged B
        theChannel->sendVector(0, 0, myB);
        theChannel->recvVector(0, 0, mutableB);
      } 

    //
    // if main process, recv B & A from all, solve and send back X, B & result
    //

    else
      {
	mutableB = myB;

	Vector remoteB(workArea.data(), size);    
	// receive X and A contribution from subprocess & add them in
	const size_t numChannels= theChannels.size();
	for(size_t j=0; j<numChannels; j++)
	  {
	    Channel *theChannel = theChannels[j];
	    theChannel->recvVector(0, 0, remoteB);
	    mutableB+= remoteB;
	  }

	// send results back
	for(size_t j=0; j<numChannels; j++)
	  {
	    Channel *theChannel = theChannels[j];
	    theChannel->sendVector(0, 0, mutableB);
	  }
      } 
    return B;
  }

//! @brief Send object members through the communicator argument.
int XC::MumpsParallelSOE::sendData(Communicator &comm)
  {
    int res= 0;
    LinearSOESolver *theSoeSolver = this->getSolver();
    if(theSoeSolver)
      {
        res+= comm.sendMovable(*theSoeSolver,getDbTagData(),CommMetaData(0));
      }
    else
      {
        std::cerr << "WARNING MumpsParallelSOE::sendSelf() - no solver to send!\n";
	return -1;
      }
    
    int sendID =0;

    // if P0 check if already sent. If already sent use old processID; if not allocate a new process 
    // id for remote part of object, enlarge channel * to hold a channel * for this remote object.

    // if not P0, send current processID

    size_t numChannels= theChannels.size();
    Channel *theChannel= comm.getChannel();
    if(processID == 0)
      {
      // check if already using this object
      bool found= false;
      for(size_t i=0; i<numChannels; i++)
	if(theChannels[i] == theChannel)
	  {
	    sendID = i+1;
	    found = true;
	  }

      // if new object, enlarge Channel pointers to hold new channel * & allocate new ID
      if(found == false)
	{
	  const size_t nextNumChannels= numChannels + 1;
	  theChannels.resize(nextNumChannels);
	  theChannels[numChannels]= theChannel;
  	  numChannels = nextNumChannels;

	  localCol.resize(numChannels);
	  for(size_t i=0; i<numChannels; i++)
	    localCol[i].clear();
	  // allocate new processID for remote object
	  sendID = numChannels;
	}
      }
    else 
      sendID = processID;
    res+= comm.sendInts(sendID, matType,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::MumpsParallelSOE::recvData(const Communicator &comm)
  {
    int res= comm.receiveInts(processID, matType,getDbTagData(),CommMetaData(1));

    const size_t numChannels = 1;
    theChannels.resize(numChannels);
    Channel *theChannel= const_cast<Channel *>(comm.getChannel());
    theChannels[0]= theChannel;

    localCol.resize(numChannels);
    for(size_t i=0; i<numChannels; i++)
      localCol[i].clear();

    MumpsParallelSolver *theSolvr= new MumpsParallelSolver();
    res+= comm.receiveMovable(*theSolvr,getDbTagData(),CommMetaData(0));
    if(res < 0)
      {
        std::cerr <<"WARNING MumpsParallelSOE::sendSelf() - failed to recv solver\n";
        return -1;
      }
    theSolvr->setLinearSOE(this);
    this->setSolver(theSolvr);
    return 0;
  }

int XC::MumpsParallelSOE::sendSelf(Communicator &comm)
  {
    inicComm(3); 
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; communicator failed to send data.\n";
    return res;
  }

int XC::MumpsParallelSOE::recvSelf(const Communicator &comm)  
  {
    // first we get the data about the state of the mesh for this cTag
    inicComm(3);
    int res= comm.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
              << "; communicator failed to recv the initial ID.\n";
    else
      res+= recvData(comm);
    return res;
  }


int XC::MumpsParallelSOE::setChannels(const std::vector<Channel *> &channels)
  {
    theChannels= channels;

    const size_t numChannels= theChannels.size();
    localCol.resize(numChannels);
    for(size_t i=0; i<numChannels; i++)
      localCol[i].clear();

    return 0;
  }

int XC::MumpsParallelSOE::setProcessID(int dTag) 
  {
    processID = dTag;
    return 0;
  }

