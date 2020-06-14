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
// $Date: 2005/12/06 22:04:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for XC::BandSPDLinSOE


#include <solution/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/actor/channel/ChannelQueue.h"

XC::DistributedBandSPDLinSOE::DistributedBandSPDLinSOE(AnalysisAggregation *owr)
  :BandSPDLinSOE(owr,LinSOE_TAGS_DistributedBandSPDLinSOE), DistributedBandLinSOE() {}

//! @brief Compute the values of numSubD y numSuperD.
void XC::DistributedBandSPDLinSOE::calcBand(Graph &theGraph)
  {
    size = theGraph.getNumVertex();
  
    //
    // determine the number of superdiagonals and subdiagonals
    //
    half_band= theGraph.getVertexDiffMaxima();
    half_band += 1; // include the diagonal
  }

int XC::DistributedBandSPDLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    int oldSize = size;
    half_band = 0;
    static ID data(2);

    if(processID != 0)
      {
        // if subprocess, collect graph, send it off, 
        // vector back containing size of system, etc.
        result= sendGraph(theGraph,data);
        size = data(0);
        half_band = data(1);
      } 
  
    else
      {
        // if main domain, collect graphs from all subdomains,
        // merge into 1, number this one, send to subdomains the
        // id containing dof tags & start id's.

        // from each distributed soe recv it's graph
        // and merge them into primary graph
        getSubGraphs(theGraph);
        calcBand(theGraph);
        data(0) = size;
        data(1) = half_band;
        sendSizeData(data);
      }

    const int numCols = theGraph.getNumVertex();
    calcLocalMap(size);
    
    int newSize;
    if(processID != 0)
      newSize = numCols * half_band;
    else
      newSize = size * half_band;

    if(newSize != A.Size())
      { // we have to get another space for A
        if(processID == 0)
          {
            workArea.resize(newSize);
          }
        A.resize(newSize);    
      }
    A.Zero();
    factored = false;
  
    if(size>B.Size())
      {    
        inic(size);
        myB.resize(size);
      }
  
    setupMyB(oldSize,size);
  
    // invoke setSize() on the Solver
    if(processID == 0)
      result= setSolverSize();
    return result;    
  }


int XC::DistributedBandSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if(fact == 0.0)  return 0;
  
    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "BandSPDLinSOE::addA() - Matrix and ID not of similar sizes\n";
        return -1;
      }
  
    ID *theMap= nullptr;
    const int numChannels= theChannels.size();
    if(numChannels > 0)
      theMap= &localCol[0];

    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                // double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
	        double *coliiPtr;
                if(processID == 0)
	          coliiPtr = A.getDataPtr() + (col+1) * half_band -1;
	        else
	          coliiPtr = A.getDataPtr() + ((*theMap)(col)+1)*half_band-1;

                const int minColRow = col - half_band + 1;
                for(int j=0; j<idSize; j++)
                  {
                    const int row= id(j);
                    if(row <size && row >= 0 &&  row <= col && row >= minColRow)
                      { // only add upper
	                double *APtr = coliiPtr + (row-col);
	                *APtr += m(j,i);
	              }
	          }  // for j
              } 
          }  // for i
      }
    else
      {
    for(int i=0; i<idSize; i++) {
      int col = id(i);
      if(col < size && col >= 0) {
	// double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
	double *coliiPtr;
	if(processID == 0)
	  coliiPtr = A.getDataPtr() + (col+1) * half_band -1;
	else
	  coliiPtr = A.getDataPtr() + ((*theMap)(col)+1)*half_band-1;

	int minColRow = col - half_band +1;
	for(int j=0; j<idSize; j++) {
	  int row = id(j);
	  if(row <size && row >= 0 && 
	      row <= col && row >= minColRow) { // only add upper
	    double *APtr = coliiPtr + (row-col);
	    *APtr += m(j,i)*fact;
	  }
	}  // for j
      } 
    }  // for i
  }
    return 0;

  return 0;
}

int XC::DistributedBandSPDLinSOE::solve(void)
  {

    static ID result(1);

    //
    // if subprocess send B and A and receive back result X, B & result
    //

    if(processID != 0)
      {
        sendBArecvBX(*this,factored,A.getDataPtr(),A.Size(),result);
        factored = true;
      } 
    //
    // if main process, recv B & A from all, solve and send back X, B & result
    //
    else
      {
        const int ldA = half_band;   
        recvXA(*this,factored,A.getDataPtr(),ldA,result);
      } 
    return result(0);
  }	


int XC::DistributedBandSPDLinSOE::addB(const Vector &v, const ID &id,const double &fact)
  { return DistributedBandLinSOE::addB(size,v,id,fact); }


int XC::DistributedBandSPDLinSOE::setB(const Vector &v,const double &fact)
  { return DistributedBandLinSOE::setB(size,v,fact); }


const XC::Vector &XC::DistributedBandSPDLinSOE::getB(void) const
  {
    DistributedBandSPDLinSOE *this_no_const= const_cast<DistributedBandSPDLinSOE *>(this);
    if(processID)
      {
        Communicator comm(0,*theChannels[0]);
        // send B & recv merged B
        comm.sendVector(myVectB,CommMetaData(0));//XXX assign position.
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
    return BandSPDLinSOE::getB();
  }	

  
int XC::DistributedBandSPDLinSOE::sendSelf(Communicator &comm)
  {
    const int retval= send(comm);
    return retval;
  }


int XC::DistributedBandSPDLinSOE::recvSelf(const Communicator &comm)
  {
    const int retval= receive(comm);
    return retval;
  }



