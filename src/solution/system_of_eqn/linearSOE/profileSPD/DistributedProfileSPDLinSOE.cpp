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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:07:52 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for XC::DistributedProfileSPDLinSOE

#include <solution/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::DistributedProfileSPDLinSOE::DistributedProfileSPDLinSOE(SolutionStrategy *owr)
  :ProfileSPDLinSOE(owr,LinSOE_TAGS_DistributedProfileSPDLinSOE), 
   DistributedBandLinSOE() {}


int XC::DistributedProfileSPDLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    const int oldSize = size;
    //int maxNumSubVertex = 0;
    int myProfileSize =0;
    static ID data(1);

    // if subprocess, collect graph, send it off, 
    // vector back containing size of system, etc.
    if(processID != 0)
      {
        result= sendGraph(theGraph,data);
        size = data(0);

        if(size > B.Size())
          iDiagLoc.resize(size);
     
        Communicator comm(0,*theChannels[0]);
        // receive my iDiagLoad
        ID iLoc(size);
        comm.receiveID(iLoc,DistributedBandLinSOE::getDbTagData(),CommMetaData(0));//XXX Assign position.

        Vertex *vertex;
        VertexIter &theSubVertices = theGraph.getVertices();
        int cnt = 0; int loc = 0;  
        while((vertex = theSubVertices()) != 0)
          {
            int vertexTag = vertex->getTag();
            int colHeight =0;
            if(vertexTag == 0)
              {
	        myProfileSize++;
	        colHeight= 1;
              }
            else
              {
                colHeight = iLoc[vertexTag] - iLoc[vertexTag-1];	    
                myProfileSize += colHeight;
              }
            loc+= colHeight;
            iDiagLoc(cnt++)= loc;	      
         }
        sizeLocal(0) = myProfileSize;

        // send local mapping & profile size to P0
        comm.sendID(sizeLocal,DistributedBandLinSOE::getDbTagData(),CommMetaData(0));
      }
    else
      {

        // if main domain, collect graphs from all subdomains,
        // merge into 1, number this one, send to subdomains the
        // id containing dof tags & start id's.

        // from each distributed soe recv it's graph
        // and merge them into primary graph

        /* int maxNumSubVertex= */ getSubGraphs(theGraph);

        size = theGraph.getNumVertex();
  
        // check we have enough space in iDiagLoc and iLastCol
        if(size != B.Size())
          iDiagLoc.resize(size);
        iDiagLoc.Zero();

    // now we go through the vertices to find the height of each col and
    // width of each row from the connectivity information.
    
    Vertex *vertexPtr;
    VertexIter &theVertices = theGraph.getVertices();

    while ((vertexPtr = theVertices()) != 0)
      {
	int vertexNum = vertexPtr->getTag();
	const std::set<int> &theAdjacency = vertexPtr->getAdjacency();
	int iiDiagLoc = iDiagLoc(vertexNum);
	int *iiDiagLocPtr = &(iDiagLoc(vertexNum));

        for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
          {
	    const int otherNum = *i;
	    int diff = vertexNum-otherNum;
	    if(diff > 0)
              {
		if(iiDiagLoc < diff)
                  {
		    iiDiagLoc = diff;
		    *iiDiagLocPtr = diff;
		  }
	      } 
	  }
      }


    // now go through iDiagLoc, adding 1 for the diagonal element
    // and then adding previous entry to give current location.
    if(!iDiagLoc.isEmpty())
      iDiagLoc(0) = 1; // NOTE FORTRAN ARRAY LOCATION - 1 of solvers uses library

    for(int j=1; j<size; j++)
      iDiagLoc(j)= iDiagLoc(j) + 1 + iDiagLoc(j-1);

    static ID data(1);
    data(0)= size;

    ID iLoc= getIDFromIntPtr(iDiagLoc.getDataPtr(), size);

    // to each distributed soe send the size data
    // and merge them into primary graph

    for(size_t j=0; j<theChannels.size(); j++)
       {
         Channel *theChannel = theChannels[j];
         theChannel->sendID(0, 0, data);
         theChannel->sendID(0, 0, iLoc);


         static ID remoteLocalSize(1);
         ID *subMap= &localCol[j];
         theChannel->recvID(0, 0, *subMap);
         theChannel->recvID(0, 0, remoteLocalSize);
         sizeLocal(j) = remoteLocalSize(0);
       }    
    myProfileSize= iDiagLoc(size-1);
  }

  profileSize = myProfileSize;
  //int numCols = theGraph.getNumVertex();
  theGraph.getNumVertex();

  if(processID != 0)
    {
      ID &globalMap= localCol[0];
      ID localMap(size);
  
      localMap.Zero();
      for(int k=0; k< globalMap.Size(); k++)
        localMap(globalMap(k)) = k; 
      localCol[0] = localMap;
    }


  // check if we need more space to hold A
  // if so then go get it
  if(profileSize != A.Size())
    { 

      // create some space for storing data sent from processes
      if(processID == 0)
        { workArea.resize(profileSize); }
  
      A.resize(profileSize);  
    }
  
  A.Zero();
  
  factored = false;
  isAcondensed = false;    
  
  if(size > B.Size())
    { // we have to get another space for A
    
      inic(size);
      myB.resize(size);
    }
  
    setupMyB(oldSize,size);
  
    // invoke setSize() on the Solver
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING DistributedProfileSPDLinSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }    
    return result;    
  }


int XC::DistributedProfileSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0)  return 0;
  
  // check that m and id are of similar size
  int idSize = id.Size();    
  if(idSize != m.noRows() && idSize != m.noCols()) {
    std::cerr << "XC::DistributedProfileSPDLinSOE::addA()	- Matrix and XC::ID not of similar sizes\n";
    return -1;
  }

  ID *theMap= nullptr;
  if(theChannels.size() > 0) 
    theMap= &localCol[0];
  
  if(fact == 1.0) { // do not need to multiply 
    for(int i=0; i<idSize; i++) {
      int col = id(i);
      if(col < size && col >= 0) {
	//	double *coliiPtr = &A[iDiagLoc(col) -1]; // -1 as fortran indexing 
	double *coliiPtr;
	if(processID == 0)
	  coliiPtr = &A[iDiagLoc(col) -1]; // -1 as fortran indexing 
	else
	  coliiPtr = &A[iDiagLoc((*theMap)(col)) -1];

	for(int j=0; j<idSize; j++) {
	  int row = id(j);
	  if(row <size && row >= 0 && 
	      row <= col) { 
	    
	    // we only add upper and inside profile
	    double *APtr= coliiPtr + (row-col);
	    *APtr += m(j,i);
	  }
	}  // for j
      } 
    }  // for i
  } else {
    for(int i=0; i<idSize; i++) {
      int col = id(i);
      if(col < size && col >= 0) {
	//  ydouble *coliiPtr = &A[iDiagLoc(col) -1]; // -1 as fortran indexing 		
	double *coliiPtr;
	if(processID == 0)
	  coliiPtr = &A[iDiagLoc(col) -1]; // -1 as fortran indexing 
	else
	  coliiPtr = &A[iDiagLoc((*theMap)(col)) -1];
			
	for(int j=0; j<idSize; j++) {
	  int row = id(j);
	  if(row <size && row >= 0 && 
	      row <= col) { 
	    
	    // we only add upper and inside profile
	    double *APtr = coliiPtr + (row-col);
	    *APtr += m(j,i) * fact;
	  }
	}  // for j
      } 
    }  // for i
    
  }
  return 0;
}

int XC::DistributedProfileSPDLinSOE::solve(void)
  {
    static XC::ID result(1);

    //
    // if subprocess send B and A and receive back result X, B & result
    //

    if(processID != 0)
      {
        sendBArecvBX(*this,factored,A.getDataPtr(),sizeLocal(0),result);
        factored = true;
      } 
    else
      {
    //
    // if main process, recv B & A from all, solve and send back X, B & result
    //
        // add P0 contribution to B
        ProfileSPDLinSOE::setB(myVectB);
    
        // receive X and A contribution from subprocess & add them in
        for(size_t j=0; j<theChannels.size(); j++)
          {
            // get X & add
            Communicator comm(0,*theChannels[j]);
            receiveX(comm);
            ProfileSPDLinSOE::addB(getX());

            //get A & add using local map
            if(factored == false)
              {
                const ID &localMap= localCol[j];
	        int localSize = sizeLocal(j);
                workArea.resize(localSize);
	        Vector vectA(workArea);    
	        comm.receiveVector(vectA,CommMetaData(1));//XXX Assign position.
		int loc = 0;
	        for(int i=0; i<localMap.Size(); i++)
                  {
                    int col= localMap(i);
                    int colSize, pos;
	            if(col == 0)
                      {
                        colSize = 1;
                        pos = 0;
	              }
	            else
                      {
                        pos = iDiagLoc(col-1);
	                colSize = iDiagLoc(col) - iDiagLoc(col-1);
	              }
	            for(int k=0; k<colSize; k++)
                      { A[pos++] += workArea[loc++]; }
	          }
              }    
          }
        //  std::cerr << *vectB;
 
        // solve
        result(0) = this->LinearSOE::solve();

        //    std::cerr << *vectX;

        // send results back
        sendResultsBack(*this,result);
      }
    return result(0);
  }

int XC::DistributedProfileSPDLinSOE::addB(const Vector &v, const ID &id,const double &fact)
  { return DistributedBandLinSOE::addB(size,v,id,fact); }


int XC::DistributedProfileSPDLinSOE::setB(const Vector &v,const double &fact)
  { return DistributedBandLinSOE::setB(size,v,fact); }


const XC::Vector &XC::DistributedProfileSPDLinSOE::getB(void) const
  {

    DistributedProfileSPDLinSOE *this_no_const= const_cast<DistributedProfileSPDLinSOE *>(this);
    if(processID)
      {
        Communicator comm(0,*theChannels[0]);
        // send B & recv merged B
        comm.sendVector(myVectB,CommMetaData(0));//XXX Assign position.
        this_no_const->receiveB(comm);
      }
    else
      {
  //
  // if main process, recv B & A from all, solve and send back X, B & result
  //
        // receive X and A contribution from subprocess & add them in
        recvBAsendBX(*this_no_const);
      } 

  return ProfileSPDLinSOE::getB();
}	

  
int XC::DistributedProfileSPDLinSOE::sendSelf(Communicator &comm)
  {
    const int res= send(comm);

    if(processID == 0)
      { sizeLocal= ID(theChannels.size()); }
    return res;
  }


int XC::DistributedProfileSPDLinSOE::recvSelf(const Communicator &comm)
  {
    const int res= receive(comm);
    sizeLocal= ID(theChannels.size());
    return res;
  }



