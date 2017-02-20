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
//DistributedLinSOE.cc

#include "DistributedLinSOE.h"
#include "solution/system_of_eqn/linearSOE/LinearSOEData.h"
#include "solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::DistributedLinSOE::DistributedLinSOE()
  : DistributedObj() {}


//! @brief Collect graph, send it off, vector back containing size of system, etc.
int XC::DistributedLinSOE::sendGraph(Graph &theGraph,ID &data)
  {
    int res= 0;
    CommParameters cp(0,*theChannels[0]);
    inicComm(3);
    res+= cp.sendMovable(theGraph,getDbTagData(),CommMetaData(0));
    
    res+= cp.receiveID(data,getDbTagData(),CommMetaData(1));

    ID subMap(theGraph.getNumVertex());
    localCol[0]= subMap;
    Vertex *vertex;
    VertexIter &theSubVertices = theGraph.getVertices();
    int cnt = 0;
    while((vertex = theSubVertices()) != 0) 
      (subMap)(cnt++) = vertex->getTag();
    res+= cp.sendID(subMap,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief From each distributed SOE recv it's graph and merge them into master graph.
int XC::DistributedLinSOE::getSubGraphs(Graph &theGraph)
  {
    int maxNumSubVertex = 0;
    FEM_ObjectBroker theBroker;
    const int numChannels= theChannels.size();
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j],theBroker);
        inicComm(1);

        Graph theSubGraph;
        cp.receiveMovable(theSubGraph,getDbTagData(),CommMetaData(0));//XXX asignar posición.
        theGraph.merge(theSubGraph);
        const int numSubVertex= theSubGraph.getNumVertex();
        ID subMap(numSubVertex);
        localCol[j]= subMap;
        maxNumSubVertex= std::max(maxNumSubVertex,numSubVertex);
      }
    return maxNumSubVertex;
  }

//! @brief To each distributed SOE send the size data and merge them into master graph
int XC::DistributedLinSOE::sendSizeData(const ID &data)
  {
    int res= 0;
    FEM_ObjectBroker theBroker;
    const int numChannels= theChannels.size();
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j],theBroker);
        inicComm(3);
        res+= cp.sendID(data,getDbTagData(),CommMetaData(1));
        res+= cp.receiveID(localCol[j],getDbTagData(),CommMetaData(2));
      }
    return res;
  }

void XC::DistributedLinSOE::calcLocalMap(const int &size)
  {
    if(processID != 0)
      {
        const ID &globalMap= localCol[0];
        ID localMap(size);
  
        localMap.Zero();
        for(int k=0; k< globalMap.Size(); k++)
          (localMap)(globalMap(k))= k; 
        localCol[0] = localMap;
      }
  }

int XC::DistributedLinSOE::getSendID(CommParameters &cp)
  {
    int retval =0;

    // if P0 check if already sent. If already sent use old processID; if not allocate a new_ process 
    // id for remote part of object, enlarge channel * to hold a channel * for this remote object.
    // if not P0, return current processID

    if(processID == 0)
      {
        // check if already using this object
        bool found= buscaCanal(cp,retval);

        // if new_ object, enlarge Channel pointers to hold new_ channel * & allocate new_ ID
        if(found == false)
          {
            assert(cp.getChannel());
            theChannels.push_back(cp.getChannel());
            const int numChannels= theChannels.size();
            localCol.resize(numChannels);
            // allocate new_ processID for remote object
            retval = numChannels;
          }
      }
    else 
      retval= processID;
    return retval;
  }

int XC::DistributedLinSOE::send(CommParameters &cp)
  {
    const int sendID= getSendID(cp);

    // send remotes processID
    ID idData(1);
    idData(0)= sendID;
    int res= cp.sendID(idData,getDbTagData(),CommMetaData(0));//XXX asignar posición.
    if(res < 0)
      {
        std::cerr <<"WARNING DistributedLinSOE::sendSelf() - failed to send data\n";
        return -1;
      }
    return res;
  }

int XC::DistributedLinSOE::receive(const CommParameters &cp)
  {
    ID idData(1);
    int res= cp.receiveID(idData,getDbTagData(),CommMetaData(0));//XXX asignar posición.
    if(res < 0)
      {
        std::cerr <<"WARNING DistributedLinSOE::recvSelf() - failed to send data\n";
        return -1;
      }	      
    processID = idData(0);

    theChannels.push_back(const_cast<CommParameters &>(cp).getChannel());
    const int numChannels= theChannels.size();
    localCol.resize(numChannels);
    return res;
  }

int XC::DistributedLinSOE::setChannels(const ChannelQueue &theC)
  {
    DistributedObj::setChannels(theC);
    const int numChannels= theChannels.size();
    localCol.resize(numChannels);
    return 0;
  }
