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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/06/20 21:35:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.cpp,v $

// Written: fmk 
// Created: 05/05
//
// Description: This file contains the implementation for XC::DistributedDiagonalSOE


#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>


XC::DistributedDiagonalSOE::DistributedDiagonalSOE(SolutionStrategy *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_DistributedDiagonalSOE),
   DistributedLinSOE(), numProcesses(0),
   myDOFs(0,32), myDOFsShared(0,16), numShared(0) {}


//! @brief Sets the size of the system from the number of vertices in the graph.
int XC::DistributedDiagonalSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size= checkSize(theGraph);
    //
    // first we build an XC::ID containing all local DOFs
    //

    myDOFs.resize(size);

    int count = 0;
    Vertex *theVertex;
    VertexIter &theVertices = theGraph.getVertices();
    while ((theVertex = theVertices()) != 0)
      {
        int vertexTag = theVertex->getTag();
        myDOFs(count) = vertexTag;
        count++;
      }

    static ID otherSize(1);
    ID otherDOFS(0, size/10);

    if(processID != 0)
      {

    //
    // each process send it's local and receives all other processes IDs (remote IDs).
    // from the remote IDs the local process figures out which DOF's it shares with others.
    // it then sends the shared dofs' out.
    //

      Channel *theChannel= theChannels[0];
      theChannel->recvID(0, 0, otherSize);
      numProcesses = otherSize(0);

    // send local
    otherSize(0) = size;
    theChannel->sendID(0, 0, otherSize);
    if (size != 0)
      theChannel->sendID(0, 0, myDOFs);

    for (int i=0; i<numProcesses-1; i++) {
      // receive remote & check for shared DOFs
      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);

      if (numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
	
	for (int j=0; j<numOther; j++) {
	  int otherTag = otherDOFS(j);
	  if (myDOFs.getLocation(otherTag) != -1 && myDOFsShared.getLocation(otherTag) == -1) 
	    myDOFsShared[numShared++] = otherTag;
	}
      }
    }

    // send my shared DOFs
    otherSize(0) = myDOFsShared.Size();
    theChannel->sendID(0, 0, otherSize);
    if (size != 0)
      theChannel->sendID(0, 0, myDOFsShared);

    // recv all shared DOFs
    theChannel->recvID(0, 0, otherSize);	
    numShared = otherSize(0);

    if (numShared != 0) {
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

    const int numChannels= theChannels.size();
    for (int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      otherSize(0) = numChannels+1;
      theChannel->sendID(0, 0, otherSize);

      otherSize(0) = size;
      theChannel->sendID(0, 0, otherSize);
      if (size != 0)
	theChannel->sendID(0, 0, myDOFs);
    }

    for (int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];

      // receive remote & check for shared DOFs

      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);
      if (numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
      }

      // need to send to all others
      for (int k=0; k<numChannels; k++) {
	Channel *theChannel = theChannels[k];
	if (k != j) {
	  theChannel->sendID(0, 0, otherSize);	
	  if (numOther != 0) {
	    theChannel->sendID(0, 0, otherDOFS);	    
	  }
	}
      }
      
      // need to merge with mine 
      for (int l=0; l<numOther; l++) {
	int otherTag = otherDOFS(l);
	if (myDOFs.getLocation(otherTag) != -1 && myDOFsShared.getLocation(otherTag) == -1) { 
	  myDOFsShared[numShared++] = otherTag;
	}
      }
    }

    // now recv each of the shared DOFs & merge with mine to form the primary list
    for (int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      theChannel->recvID(0, 0, otherSize);	
      int numOther = otherSize(0);
      if (numOther != 0) {
	otherDOFS.resize(numOther);
	theChannel->recvID(0, 0, otherDOFS);
      } 
      // need to merge with mine 
      for (int k=0; k<numOther; k++) {
	int otherTag = otherDOFS(k);
	if (myDOFsShared.getLocation(otherTag) == -1) 
	  myDOFsShared(numShared++) = otherTag;
      }
    }

    // now send the shared XC::ID
    for (int j=0; j<numChannels; j++) {
      Channel *theChannel = theChannels[j];
      otherSize(0) = numShared;
      theChannel->sendID(0, 0, otherSize);	
      theChannel->sendID(0, 0, myDOFsShared);
    }
  }


    A.resize(size); 
    inic(size);
  
    dataShared.resize(2*numShared); // 2 times for A & B
    vectShared= Vector(dataShared.getDataPtr(), 2*numShared);


    A.Zero();
    factored= false;

    //
    // now let's redo the mapping of the dof's .. locally numbered 0 through size
    //

  
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      std::cerr << "WARNING DistributedDiagonalSOE::setSize - no AnalysisModel\n";
    else
      {
        DOF_GrpIter &theDOFGroups= mdl->getDOFGroups();
        DOF_Group *dofGroupPtr= nullptr;
    
        while((dofGroupPtr= theDOFGroups()) != 0)
          {
            const ID &theID = dofGroupPtr->getID();
            for(int i=0; i<theID.Size(); i++)
              {
                int dof = theID(i);
                if(dof >= 0)
                  {
                    int newDOF = myDOFs.getLocation(dof);
                    dofGroupPtr->setID(i, newDOF);
	          }
	      }   
          }
    
    // iterate through the FE_Element getting them to set their IDs
    FE_EleIter &theEle= mdl->getFEs();
    FE_Element *elePtr= nullptr;
    while ((elePtr = theEle()) != 0)
	elePtr->setID(); 
  }  

  // invoke setSize() on the Solver
    result= setSolverSize();
  
    DistributedDiagonalSolver *the_Solver= dynamic_cast<DistributedDiagonalSolver *>(this->getSolver());
  the_Solver->setLinearSOE(*this);
  return result;
}

int XC::DistributedDiagonalSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0)  return 0;
  
#ifdef _G3DEBUG
  // check that m and id are of similar size
  int idSize = id.Size();    
  if (idSize != m.noRows() && idSize != m.noCols()) {
    std::cerr << "FullGenLinSOE::addA()	- Matrix and ID not of similar sizes\n";
    return -1;
  }
#endif

  if (fact == 1.0) { // do not need to multiply if fact == 1.0
    for (int i=0; i<id.Size(); i++) {
      int pos = id(i);
      if (pos <size && pos >= 0) {
	A[pos] += m(i,i);
      }
    }
  } else if (fact == -1.0) { // do not need to multiply if fact == -1.0
    for (int i=0; i<id.Size(); i++) {
      int pos = id(i);
      if (pos <size && pos >= 0) {
	A[pos] -= m(i,i);
      }
    }
  } else {
    for (int i=0; i<id.Size(); i++) {
      int pos = id(i);
      if (pos <size && pos >= 0) {
	A[pos] += m(i,i) * fact;
      }
    }
  }	

  return 0;
}
 
    
void XC::DistributedDiagonalSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

int XC::DistributedDiagonalSOE::sendSelf(Communicator &comm)
  { return send(comm); }

int XC::DistributedDiagonalSOE::recvSelf(const Communicator &comm)
  { return receive(comm); }

