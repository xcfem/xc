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
int XC::DistributedLinSOE::sendGraph(Graph &theGraph,ID &datos)
  {
    int res= 0;
    CommParameters cp(0,*theChannels[0]);
    inicComm(3);
    res+= cp.sendMovable(theGraph,getDbTagData(),CommMetaData(0));
    
    res+= cp.receiveID(datos,getDbTagData(),CommMetaData(1));

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
int XC::DistributedLinSOE::sendSizeData(const ID &datos)
  {
    int res= 0;
    FEM_ObjectBroker theBroker;
    const int numChannels= theChannels.size();
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j],theBroker);
        inicComm(3);
        res+= cp.sendID(datos,getDbTagData(),CommMetaData(1));
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
