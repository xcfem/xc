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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/region/MeshRegion.cpp,v $
                                                                        
                                                                        
// Written: fmk 
//
// Description: This file contains the class definition for XC::MeshRegion.
// A XC::MeshRegion is a part of the domain which is defined by a set of
// Elements and Nodes (all the end nodes of the elements are in the region, 
// as are all elements whose end nodes are in the region)
//
// What: "@(#) MeshRegion.h, revA"


#include "MeshRegion.h"
#include <cstdlib>
#include <cstring>

#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include "utility/matrix/IDVarSize.h"
#include <domain/domain/Domain.h>
#include <utility/recorder/NodeRecorder.h>
#include <utility/recorder/ElementRecorder.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "utility/actor/actor/ArrayCommMetaData.h"



void XC::MeshRegion::libera(void) 
  {
    if(theNodes)
      delete theNodes;
    theNodes= nullptr;
    if(theElements)
      delete theElements;
    theElements= nullptr;
  }

void XC::MeshRegion::copia(const MeshRegion &otra) 
  {
    libera();
    if(otra.theNodes)
      {
        size_t numNodes = otra.theNodes->Size();
        theNodes = new IDVarSize(0, numNodes);
        for(size_t i=0;i<numNodes;i++)
          theNodes[i]= otra.theNodes[i];
      }
  }

XC::MeshRegion::MeshRegion(int tag) 
  :ContinuaReprComponent(tag, REGION_TAG_MeshRegion), rayFactors(), theNodes(nullptr), theElements(nullptr),
   currentGeoTag(0), lastGeoSendTag(-1) {}

XC::MeshRegion::MeshRegion(int tag, int cTag) 
  :ContinuaReprComponent(tag, cTag), rayFactors(), theNodes(nullptr), theElements(nullptr),
   currentGeoTag(0), lastGeoSendTag(-1) {}

//! @brief Constructor de copia
XC::MeshRegion::MeshRegion(const MeshRegion &otra) 
  :ContinuaReprComponent(otra), rayFactors(otra.rayFactors), theNodes(nullptr), theElements(nullptr),
   currentGeoTag(otra.currentGeoTag), lastGeoSendTag(otra.lastGeoSendTag)
  { copia(otra); }

//! @brief Operador asignación.
XC::MeshRegion &XC::MeshRegion::operator=(const MeshRegion &otra)
  {
    ContinuaReprComponent::operator=(otra);
    rayFactors= otra.rayFactors;
    currentGeoTag= otra.currentGeoTag;
    lastGeoSendTag= otra.lastGeoSendTag;
    copia(otra);
    return *this;
  }

//! @brief Virtual constructor.
XC::MeshRegion *XC::MeshRegion::getCopy(void) const
  { return new MeshRegion(*this); }

XC::MeshRegion::~MeshRegion(void) 
  { libera(); }

int XC::MeshRegion::setNodes(const XC::ID &theNods)
  {
    libera();

    //
    // create new element & node lists
    //

    // create empty lists
    Domain *theDomain = this->getDomain();
    if(!theDomain == 0)
      {
	std::cerr << "XC::MeshRegion::setNodes() - no domain yet set\n";
        return -1;
      }

    int numNodes = theNods.Size();
    theNodes = new XC::ID(0,numNodes);
    theElements = new XC::ID(0, numNodes);
    if(theNodes == 0 || theElements == 0)
      {
        std::cerr << "XC::MeshRegion::setElements() - ran out of memory\n";
        return -1;
      }

    // add nodes to the node list if in the domain
    int loc = 0;
    for(int i=0; i<numNodes; i++)
      {
        int nodeTag = theNods(i);
        Node *theNode = theDomain->getNode(nodeTag);
        if(theNode)
          {
            if (theNodes->getLocation(nodeTag) < 0)
	    (*theNodes)[loc++] = nodeTag;      
          }
      }

    // now loop over the ele to create the ele list
    // NOTE - ele added to list if all it's nodes are in the region
    loc = 0;

    ElementIter &theEles = theDomain->getElements();
    Element *theEle;

    // loop over all ele
    while((theEle = theEles()) != 0)
      {
        int eleTag = theEle->getTag();
        // check to see if all external nodes in node list
        bool in = true;
        const XC::ID &theEleNodes = theEle->getNodePtrs().getExternalNodes();
        int numNodes = theEleNodes.Size();

        for(int i=0; i<numNodes; i++)
          {
            int nodeTag = theEleNodes(i);
            if(theNodes->getLocation(nodeTag) < 0)
              {
	        in = false;
	        i = numNodes;
              }
          }
        // if they are all in the node list add the ele to ele list
        if(in == true) 
          (*theElements)[loc++] = eleTag;
      }
    return 0;
  }


int XC::MeshRegion::setElements(const XC::ID &theEles)
  {
    libera();

    // create new element & node lists

    int numEle = theEles.Size();
    theElements= new XC::ID(0, numEle); // don't copy yet .. make sure ele in domain
    theNodes= new XC::ID(0, numEle); // initial guess at size of XC::ID
    if(theElements == 0 || theNodes == 0)
      {
	std::cerr << "XC::MeshRegion::setElements() - ran out of memory\n";
        return -1;
      }

    // now loop over the elements in ele XC::ID passed in to create the node & ele list
    // NOTE - only add those elements to the list that are in the domain
    // NOTE - node added to region if any element has it as an external node
    int locEle = 0;
    int locNode = 0;


    Domain *theDomain = this->getDomain();
    if(!theDomain)
      {
	std::cerr << "XC::MeshRegion::setElements() - no domain yet set\n";
        return -1;
      }

    Element *theEle;
    for(int i=0; i<numEle; i++)
      {
        int eleTag = theEles(i);
        theEle = theDomain->getElement(eleTag);
        if(theEle)
          {
            if(theElements->getLocation(eleTag) < 0)
	      (*theElements)[locEle++] = eleTag;

            const XC::ID &theEleNodes = theEle->getNodePtrs().getExternalNodes();
            for(int i=0; i<theEleNodes.Size(); i++)
              {
                int nodeTag = theEleNodes(i);
                // add the node tag if not already there
                if(theNodes->getLocation(nodeTag) < 0)
                  (*theNodes)[locNode++] = nodeTag;
              }
          }
      }
    return 0;
  }


const XC::ID &XC::MeshRegion::getNodes(void)
  {
    if(!theNodes)
      std::cerr << "XC::FATAL::XC::MeshRegion::getNodes(void) - no nodes yet set\n";
    return *theNodes;
  }

const XC::ID &XC::MeshRegion::getElements(void)
  {
    if(!theElements)
      std::cerr << "XC::FATAL::XC::MeshRegion::getElements(void) - no elements yet set\n";
    return *theElements;
  }


int XC::MeshRegion::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  {
    rayFactors= rF;

    // now set the damping factors at the nodes & elements
    Domain *theDomain = this->getDomain();
    if(!theDomain)
      {
        std::cerr << "XC::MeshRegion::setRayleighDampingFactors() - no domain yet set\n";
        return -1;
      }
    if(theElements)
      {
        for(int i=0; i<theElements->Size(); i++)
          {
            int eleTag = (*theElements)(i);
            Element *theEle = theDomain->getElement(eleTag);
            if(theEle)
              theEle->setRayleighDampingFactors(rayFactors);
          }
      }

    if(theNodes)
      {
        for(int i=0; i<theNodes->Size(); i++)
          {
           int nodTag = (*theNodes)(i);
           Node *theNode = theDomain->getNode(nodTag);
           if(theNode)
	     theNode->setRayleighDampingFactor(rayFactors.getAlphaM());
          }
      }
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::MeshRegion::sendData(CommParameters &cp)
  {
    int res= ContinuaReprComponent::sendData(cp);
    res+= cp.sendMovable(rayFactors,getDbTagData(),CommMetaData(2));
    res+= cp.sendIDPtr(theNodes,getDbTagData(),ArrayCommMetaData(3,4,5));
    res+= cp.sendIDPtr(theElements,getDbTagData(),ArrayCommMetaData(6,7,8));
    res+= cp.sendInts(currentGeoTag,lastGeoSendTag,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::MeshRegion::recvData(const CommParameters &cp)
  {
    int res= ContinuaReprComponent::recvData(cp);
    res+= cp.receiveMovable(rayFactors,getDbTagData(),CommMetaData(2));
    theNodes= cp.receiveIDPtr(theNodes,getDbTagData(),ArrayCommMetaData(3,4,5));
    theElements= cp.receiveIDPtr(theElements,getDbTagData(),ArrayCommMetaData(6,7,8));
    res+= cp.receiveInts(currentGeoTag,lastGeoSendTag,getDbTagData(),CommMetaData(9));
    return res;
  }

int XC::MeshRegion::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::MeshRegion::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::MeshRegion::Print(std::ostream &s, int flag)
  {
    s << "Region: " << this->getTag() << std::endl;
    if(theElements)
      s << "Elements: "<< *theElements;
    if(theNodes)
      s << "Nodes: "<< *theNodes;
    rayFactors.Print(s,flag);
  }
