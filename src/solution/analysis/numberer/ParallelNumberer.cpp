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
** file 'COPYRIGHT'  in :main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2005/11/29 21:55:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/ParallelNumberer.cpp,v $                                                                        

// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for XC::ParallelNumberer.
//
// What: "@(#) ParallelNumberer.C, revA"

#include <solution/analysis/numberer/ParallelNumberer.h>
#include <solution/analysis/model/AnalysisModel.h>

#include <domain/domain/Domain.h>
#include <utility/matrix/ID.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "solution/graph/numberer/GraphNumberer.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>

XC::ParallelNumberer::ParallelNumberer(ModelWrapper *owr,int dTag, int numSub,const ChannelQueue &theC) 
  :DOF_Numberer(owr,NUMBERER_TAG_ParallelNumberer), DistributedObj(theC), theNumberer(nullptr) {}

XC::ParallelNumberer::ParallelNumberer(ModelWrapper *owr) 
  :DOF_Numberer(owr,NUMBERER_TAG_ParallelNumberer), theNumberer(nullptr) {}


XC::ParallelNumberer::~ParallelNumberer(void) 
  {
    if(theNumberer)
      delete theNumberer;
  }

//! @brief Constructor virtual.
XC::DOF_Numberer *XC::ParallelNumberer::getCopy(void) const
  { return new ParallelNumberer(*this);  }

// int numberDOF(void)
// The ParalellNumberer sitting on P0, collects each partition graph from P1 through Pn-1, 
// merges them into 1 large graph, & then numbers this graph. The ParallelNumberers sitting 
// on P1 through Pn-1 then receive the mapping info for the dof tag and dof numbering from P0.

int XC::ParallelNumberer::numberDOF(int lastDOF)
  {
    int result = 0;

    // get a pointer to the model & check its not null
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    Domain *theDomain = 0;
    if(theModel) theDomain = theModel->getDomainPtr();
  
    if(theModel == 0 || theDomain == 0)
      {
        std::cerr << "WARNING XC::ParallelNumberer::numberDOF(int) -";
        std::cerr << " - no AnalysisModel.\n";
        return -1;
      }
  
    if(lastDOF != -1)
      {
        std::cerr << "WARNING XC::ParallelNumberer::numberDOF(int lastDOF):";
        std::cerr << " does not use the lastDOF as requested\n";
      }

    Graph &theGraph= theModel->getDOFGroupGraph();

    // if subdomain, collect graph, send it off, get 
    // ID back containing dof tags & start id numbers.
    if(processID != 0)
      {
        CommParameters cp(0,*theChannels[0]);
        const int numVertex = theGraph.getNumVertex();

        /*
        static XC::ID test(2); test(0) = processID; test(1) = 25;
        theChannel->recvID(0, 0, test);
        */

        cp.sendMovable(theGraph,DistributedObj::getDbTagData(),CommMetaData(1));

        // recv iD
        ID theID(2*numVertex);
        cp.receiveID(theID,DistributedObj::getDbTagData(),CommMetaData(2));

        // set vertex numbering based on ID received
        for(int i=0; i<numVertex; i ++)
          {
            const int vertexTag= theID(i);
            int startID= theID(i+numVertex);
            //Vertex *vertexPtr = theGraph.getVertexPtr(vertexTag);
            const int dofTag= vertexTag;
            DOF_Group *dofPtr= theModel->getDOF_GroupPtr(dofTag);
            if(!dofPtr)
              {
                std::cerr << "WARNING ParallelNumberer::numberDOF - ";
                std::cerr << "DOF_Group " << dofTag << "not in XC::AnalysisModel!\n";
                result= -4;
              }
            else
              {
                const ID &theDOFID= dofPtr->getID();
                //std::cerr << "P: " << processID << " dofTag: " << dofTag << " " << "start: " << startID << " " << theDOFID;
                const int idSize= theDOFID.Size();
                for(int j=0; j<idSize; j++)
                  if(theDOFID(j) == -2 || theDOFID(j) == -3) dofPtr->setID(j, startID++);
              }
            //const ID &theDOFID= dofPtr->getID();
          }
        cp.sendID(theID,DistributedObj::getDbTagData(),CommMetaData(2));
      } 
    else
      {
        // if XC::main domain, collect graphs from all subdomains,
        // merge into 1, number this one, send to subdomains the
        // id containing dof tags & start id's.

        // for P0 domain determine original vertex and ref tags
        const int numVertex= theGraph.getNumVertex(); 
        const int numVertexP0= numVertex;

        ID vertexTags(numVertex);
        ID vertexRefs(numVertex);
        Vertex *vertexPtr;
        int loc= 0;
        VertexIter &theVertices= theGraph.getVertices();
        while((vertexPtr= theVertices()) != 0)
          {
            vertexTags[loc]= vertexPtr->getTag();
            vertexRefs[loc]= vertexPtr->getRef();
            loc++;
          }
    
        const int numChannels= theChannels.size();
	std::vector<ID> theSubdomainIDs(numChannels);
        FEM_ObjectBroker theBroker;

        // for each subdomain we receive graph, create an XC::ID (to store
        // subdomain graph to merged graph vertex mapping and the final
        // subdoain graph vertex to startDOF mapping) and finally merge the
        // subdomain graph

        for(int j=0; j<numChannels; j++)
          {
            CommParameters cp(0,*theChannels[j]);
            Graph theSubGraph;

            /*
            static XC::ID test(2); test(0)= processID; test(1)= 25;
            theChannel->sendID(0, 0, test);
            */
            cp.receiveMovable(theSubGraph,DistributedObj::getDbTagData(),CommMetaData(3));
            theSubdomainIDs[j]= ID(theSubGraph.getNumVertex()*2);
            this->mergeSubGraph(theGraph, theSubGraph, vertexTags, vertexRefs, theSubdomainIDs[j]);
          }
    
        // we use graph numberer if one was provided in constructor,
        // otherwise we number based on subdomains (all in subdomain 1 numbered first, 
        // then  those in 2 not in 1 and so on till done.
        //    GraphNumberer *theNumberer= this->getGraphNumbererPtr();

        ID theOrderedRefs(theGraph.getNumVertex());

        if(theNumberer)
          {
            // use the supplied graph numberer to number the merged graph
            theOrderedRefs= theNumberer->number(theGraph, lastDOF);     
          }
        else
          {
            // assign numbers based on the subdomains

            int loc= 0;
            for(int l=0; l<numChannels; l++)
              {
                const ID &theSubdomain= theSubdomainIDs[l];
                int numVertexSubdomain= theSubdomain.Size()/2;

                for(int i=0; i<numVertexSubdomain; i++)
                  {
                    const int vertexTagMerged= theSubdomain(i+numVertexSubdomain);
                    //  int refTag= vertexRefs[vertexTags.getLocation(vertexTagMerged)];
                    if(theOrderedRefs.getLocation(vertexTagMerged) == -1)
                      theOrderedRefs[loc++]= vertexTagMerged;
                  }
              }

            // now order those not yet ordered in p0
            for(int j=0; j<numVertexP0; j++)
              {
                int refTagP0= vertexTags[j];
                if(theOrderedRefs.getLocation(refTagP0) == -1)
                  theOrderedRefs[loc++]= refTagP0;
              }        
          }
    int count= 0;
    for(int i=0; i<theOrderedRefs.Size(); i++)
      {
        int vertexTag= theOrderedRefs(i);
        //      int vertexTag= vertexTags[vertexRefs.getLocation(tag)];
        Vertex *vertexPtr= theGraph.getVertexPtr(vertexTag);
        int numDOF= vertexPtr->getColor();
        vertexPtr->setTmp(count);
        count += numDOF;
      }

    // number own dof's
    for(int i=0; i<numVertexP0; i++  ) {
      int vertexTag= vertexTags(i);
      Vertex *vertexPtr= theGraph.getVertexPtr(vertexTag);

      int startID= vertexPtr->getTmp();
      int dofTag= vertexTag;
      DOF_Group *dofPtr;        
      dofPtr= theModel->getDOF_GroupPtr(dofTag);
      if(dofPtr == 0) {
        std::cerr << "WARNING XC::ParallelNumberer::numberDOF - ";
        std::cerr << "DOF_Group (P0) " << dofTag << "not in XC::AnalysisModel!\n";
        result= -4;
      } else {
        const ID &theDOFID= dofPtr->getID();
        int idSize= theDOFID.Size();
        for(int j=0; j<idSize; j++)
          if(theDOFID(j) == -2 || theDOFID(j) == -3) dofPtr->setID(j, startID++);
      }
    }

    // now given the ordered refs we determine the mapping for each subdomain
    // and send the id with the information back to the subdomain, which it uses to order
    // it's own graph
    for(int k=0; k<numChannels; k++)
     {
        CommParameters cp(0,*theChannels[k]);
        ID &theSubdomain= theSubdomainIDs[k];
        int numVertexSubdomain= theSubdomain.Size()/2;

        for(int i=0; i<numVertexSubdomain; i++)
          {
            int vertexTagMerged= theSubdomain[numVertexSubdomain+i];
            Vertex *vertexPtr= theGraph.getVertexPtr(vertexTagMerged);
            int startDOF= vertexPtr->getTmp();
            theSubdomain[i+numVertexSubdomain]= startDOF;
          }
        cp.sendID(theSubdomain,DistributedObj::getDbTagData(),CommMetaData(4));
        cp.receiveID(theSubdomain,DistributedObj::getDbTagData(),CommMetaData(4));
      }      
  }

  // iterate through the XC::FE_Element getting them to set their IDs
  FE_EleIter &theEle= theModel->getFEs();
  FE_Element *elePtr;
  while ((elePtr= theEle()) != 0)
    elePtr->setID();
  
  return result;
}


int XC::ParallelNumberer::mergeSubGraph(Graph &theGraph, Graph &theSubGraph, ID &vertexTags, ID &vertexRefs, ID &theSubdomainMap)
{  
  // for each vertex in the SubGraph we see if a vertex exists in the XC::Graph which has the same
  // reference tag (Reference tag in the XC::AnalysisModel graph is the node tag) .. if so this will be 
  // the new_ vertex tag for SubGraph vertex in new_ graph, otherwise we assign it some new_ vertex tag,
  // create a vertex for this new_ vertex tag & add it to the graph

  Vertex *subVertexPtr;
  VertexIter &theSubGraphIter1= theSubGraph.getVertices();
  int count =0;
  int numVertex= theGraph.getNumVertex();
  int numVertexSub= theSubGraph.getNumVertex();

  while ((subVertexPtr= theSubGraphIter1()) != 0) {
    int vertexTagSub= subVertexPtr->getTag();
    int vertexTagRef= subVertexPtr->getRef();
    int loc= vertexRefs.getLocation(vertexTagRef);

    int vertexTagMerged;
    if(loc < 0)
      {
        // if not already in, we will be creating a new_ vertex
        vertexTagMerged= theGraph.getFreeTag();
        vertexTags[numVertex]= vertexTagMerged;
        vertexRefs[numVertex]= vertexTagRef;
        Vertex newVertex(vertexTagMerged, vertexTagRef, subVertexPtr->getWeight(), subVertexPtr->getColor());

        theGraph.addVertex(newVertex);
        numVertex++;
      }
    else
      vertexTagMerged= vertexTags[loc];

    // use the subgraphs XC::ID to hold the mapping of vertex numbers between merged and original
    theSubdomainMap[count]= vertexTagSub;
    theSubdomainMap[count+numVertexSub]= vertexTagMerged;
    count++;
  }

    // for each vertex in subgraph, we add it's adjacenecy into the merged graph
    VertexIter &theSubGraphIter2= theSubGraph.getVertices();
    while ((subVertexPtr= theSubGraphIter2()) != 0)
      {
        const int vertexTagSub= subVertexPtr->getTag();
        const int loc= theSubdomainMap.getLocation(vertexTagSub);
        const int vertexTagMerged= theSubdomainMap[loc+numVertexSub];

        const std::set<int> &adjacency= subVertexPtr->getAdjacency();

        for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
            const int vertexTagSubAdjacent= *i;
            const int loc= theSubdomainMap.getLocation(vertexTagSubAdjacent);
            const int vertexTagMergedAdjacent= theSubdomainMap[loc+numVertexSub];      
            theGraph.addEdge(vertexTagMerged, vertexTagMergedAdjacent);
          }
      }


  return 0;
}


int XC::ParallelNumberer::sendSelf(CommParameters &cp)
  {
    int sendID =0;

    // if P0 check if already sent. If already sent use old processID; if not allocate a new_ process 
    // id for remote part of object, enlarge channel * to hold a channel * for this remote object.

    // if not P0, send current processID

    if(processID == 0)
      {
        // check if already using this object
        bool found= buscaCanal(cp,sendID);

        // if new_ object, enlarge channel pointers to hold new_ channel * & allocate new_ ID
        if(found == false)
          {
            theChannels.push_back(cp.getChannel());
            // allocate new_ processID for remote object
            sendID= theChannels.size();
          }
      }
    else 
      sendID= processID;


    // send remotes processID
    ID idData(1);
    idData(0)= sendID;
  
    int res= cp.sendID(idData,DistributedObj::getDbTagData(),CommMetaData(0));
    if(res < 0)
      {
        std::cerr <<"WARNING XC::DistributedSparseGenColLinSOE::sendSelf() - failed to send data\n";
        return -1;
      }
    return 0;
  }

int XC::ParallelNumberer::recvSelf(const CommParameters &cp)
  {
    ID idData(1);
    int res= cp.receiveID(idData,DistributedObj::getDbTagData(),CommMetaData(0));
    if(res < 0)
      {
        std::cerr <<"WARNING XC::Parallel::recvSelf() - failed to send data\n";
        return -1;
      }              
    processID= idData(0);

    theChannels.resize(1);
    theChannels[0]= const_cast<Channel *>(cp.getChannel());
    return 0;
  }


int XC::ParallelNumberer::numberDOF(ID &lastDOFs)
{

  int result= 0;
  
  // get a pointer to the model & check its not null
  AnalysisModel *theModel= this->getAnalysisModelPtr();
  Domain *theDomain= 0;
  if(theModel != 0) theDomain= theModel->getDomainPtr();
  
  if(theModel == 0 || theDomain == 0) {
    std::cerr << "WARNING ParallelNumberer::numberDOF(int) -";
    std::cerr << " - no AnalysisModel.\n";
    return -1;
  }
  
  Graph &theGraph= theModel->getDOFGroupGraph();
  
  // if subdomain, collect graph, send it off, get 
  // ID back containing dof tags & start id numbers.
    if(processID != 0)
      {
        CommParameters cp(0,*theChannels[0]);
        int numVertex= theGraph.getNumVertex();
        cp.sendMovable(theGraph,DistributedObj::getDbTagData(),CommMetaData(5));
        ID theID(2*numVertex);
        cp.receiveID(theID,DistributedObj::getDbTagData(),CommMetaData(6));
    for(int i=0; i<numVertex; i += 2) {
      int dofTag= theID(i);
      int startID= theID(i+1);
      DOF_Group *dofPtr;        
      dofPtr= theModel->getDOF_GroupPtr(dofTag);
      if(dofPtr == 0) {
        std::cerr << "WARNING XC::ParallelNumberer::numberDOF - ";
        std::cerr << "DOF_Group " << dofTag << "not in XC::AnalysisModel!\n";
        result= -4;
      } else {
        const ID &theID= dofPtr->getID();
        int idSize= theID.Size();
        for(int j=0; j<idSize; j++)
          if(theID(j) == -2) dofPtr->setID(j, startID++);
      }
    }
  } 
  
  // if XC::main domain, collect graphs from all subdomains,
  // merge into 1, number this one, send to subdomains the
  // id containing dof tags & start id's.
  else {
    
    // determine original vertex and ref tags
    int numVertex= theGraph.getNumVertex();
    ID vertexTags(numVertex);
    ID vertexRefs(numVertex);
    Vertex *vertexPtr;
    int loc= 0;
    VertexIter &theVertices= theGraph.getVertices();
    while ((vertexPtr= theVertices()) != 0) {
      vertexTags[loc]= vertexPtr->getTag();
      vertexRefs[loc]= vertexPtr->getRef();
      loc++;
    }
    
    const int numChannels= theChannels.size();
    std::vector<ID> theSubdomainIDs(numChannels);
    FEM_ObjectBroker theBroker;

    // merge all subdomain graphs
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j]);
        Graph theSubGraph;
        cp. receiveMovable(theSubGraph,DistributedObj::getDbTagData(),CommMetaData(6));
        theSubdomainIDs[j]= ID(theSubGraph.getNumVertex()*2);
        this->mergeSubGraph(theGraph, theSubGraph, vertexTags, vertexRefs, theSubdomainIDs[j]);
      }

    // number the merged graph
    //    result=  this->XC::DOF_Numberer::number(theGraph);

    // send results of numbered back to subdomains
    for(int k=0; k<numChannels; k++)
      {
        Channel *theChannel= theChannels[k];
        // this->determineSubIDs
        theChannel->sendID(0, 0, theSubdomainIDs[k]);
      }      
    }

  return result;
}
