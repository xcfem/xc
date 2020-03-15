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
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/ShadowSubdomain.cpp,v $

// Written: fmk
// Revision: A
//
// Description: This file contains the class definition for XC::ShadowSubdomain.
// ShadowSubdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints
// and MFreedom_Constraints that have been added to the XC::ShadowSubdomain.
//
// What: "@(#) ShadowSubdomain.C, revA"


#include "ShadowSubdomain.h"
#include <cstdlib>

#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/Timer.h>
#include <domain/load/pattern/LoadPattern.h>

#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/domain/single/SingleDomNodIter.h>
#include <domain/domain/single/SingleDomEleIter.h>
#include "solution/graph/graph/Graph.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "domain/domain/subdomain/modelbuilder/PartitionedModelBuilder.h"

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/recorder/Recorder.h>

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "utility/matrix/Matrix.h"

#include "ShadowActorSubdomain.h"

int XC::ShadowSubdomain::count = 0; // MHS
int XC::ShadowSubdomain::numShadowSubdomains = 0;
std::deque<XC::ShadowSubdomain *> XC::ShadowSubdomain::theShadowSubdomains;

void XC::ShadowSubdomain::free_vectors(void) const
  {
    if(theVector)
      {
        delete theVector;
        theVector= nullptr;
      }
    if(theMatrix)
      {
        delete theMatrix;
        theMatrix= nullptr;
      }
  }

void XC::ShadowSubdomain::alloc_vectors(const size_t &numDOF) const
  {
    free_vectors();
    theVector= new Vector(numDOF);
    theMatrix= new Matrix(numDOF,numDOF);
  }

void XC::ShadowSubdomain::resize_vectors(const size_t &numDOF) const
  {
    size_t sz= 0;
    if(theVector)
      sz= theVector->Size();
    if(sz!=numDOF)
      alloc_vectors(numDOF);
  }

void XC::ShadowSubdomain::free_arrays(void)
  {
    if(theShadowSPs)
      {
        delete theShadowSPs;
        theShadowSPs= nullptr;
      }
    if(theShadowMPs)
      {
        delete theShadowMPs;
        theShadowMPs= nullptr;
      }
    if(theShadowLPs)
      {
        delete theShadowLPs;
        theShadowLPs= nullptr;
      }
  }

void XC::ShadowSubdomain::alloc_arrays(const size_t &sz1,const size_t &sz2)
  {
    free_arrays();
    theShadowSPs=new ArrayOfTaggedObjects(static_cast<Domain *>(this),sz1,"SPs");
    theShadowMPs=new ArrayOfTaggedObjects(static_cast<Domain *>(this),sz1,"MPs");
    theShadowLPs=new ArrayOfTaggedObjects(static_cast<Domain *>(this),sz2,"LPs");
  }

XC::ShadowSubdomain::ShadowSubdomain(int tag, MachineBroker &theMachineBroker, FEM_ObjectBroker &theObjectBroker,DataOutputHandler::map_output_handlers *oh,CommandEntity *owr)
  :Shadow(ACTOR_TAGS_SUBDOMAIN, theObjectBroker, theMachineBroker, 0),
   Subdomain(tag,oh,owr),
   msgData(4),
   theElements(0,128),
   theNodes(0,128),
   theExternalNodes(0,128),
   theLoadCases(0,128),
   theShadowSPs(nullptr), theShadowMPs(nullptr), theShadowLPs(nullptr),
   numDOF(0),numElements(0),numNodes(0),numExternalNodes(0),
   numSPs(0),numMPs(0), buildRemote(false), gotRemoteData(false),
   theFEele(0),
   theVector(0), theMatrix(0)
  {

    numShadowSubdomains++;
    theShadowSubdomains.push_back(this);
    alloc_arrays(256,32);

    // does nothing
    numLoadPatterns = 0;

    msgData(0)= ShadowActorSubdomain_setTag;
    msgData(1) = tag;
    this->sendID(msgData);
    this->setCommitTag(tag);
  }


XC::ShadowSubdomain::ShadowSubdomain(int tag, Channel &the_Channel, FEM_ObjectBroker &theObjectBroker,DataOutputHandler::map_output_handlers *oh,CommandEntity *owr)
  :Shadow(the_Channel, theObjectBroker),
   Subdomain(tag,oh,owr),
   msgData(4),
   theElements(0,128),
   theNodes(0,128),
   theExternalNodes(0,128),
   theLoadCases(0,128),
   theShadowSPs(nullptr), theShadowMPs(nullptr), theShadowLPs(nullptr),
   numDOF(0),numElements(0),numNodes(0),numExternalNodes(0),
   numSPs(0),numMPs(0), buildRemote(false), gotRemoteData(false),
   theFEele(0),
   theVector(0), theMatrix(0)
  {

    numShadowSubdomains++;
    theShadowSubdomains.push_back(this);
    alloc_arrays(256,32);

    // does nothing
    numLoadPatterns = 0;
  }

XC::ShadowSubdomain::~ShadowSubdomain()
  {
    // send a message to the remote actor telling it to shut sown
    msgData(0) = ShadowActorSubdomain_DIE;
    this->sendID(msgData);
    this->recvID(msgData);
    free_mem();
    std::cerr << "XC::ShadowSubdomain::~ShadowSubdomain()\n";
  }


int XC::ShadowSubdomain::buildSubdomain(int numSubdomains, PartitionedModelBuilder &theBuilder)
  {
    // send a message identify setting ModelBuilder and it's class tag
    buildRemote = true;
    gotRemoteData = false;

    msgData(0) = ShadowActorSubdomain_buildSubdomain;
    msgData(1) = theBuilder.getClassTag();
    msgData(2) = numSubdomains;
    msgData(3) = this->getTag();
    this->sendID(msgData);

    // send the builder
    this->sendObject(theBuilder);

    // mark the domain as having been modified
    this->domainChange();
    return 0;
  }


int XC::ShadowSubdomain::getRemoteData(void)
  {
    if(buildRemote == true && gotRemoteData == false)
      {
        msgData(0) = ShadowActorSubdomain_getRemoteData;
        this->sendID(msgData);

        this->recvID(msgData);
        numExternalNodes = msgData(0);
        numDOF= msgData(1);

        std::cerr << "XC::ShadowSubdomain::getRemoteData numExtNodes " << numExternalNodes << std::endl;
        if(theExternalNodes.Size() != numExternalNodes)
          theExternalNodes[numExternalNodes-1] = 0; // this will resize
        if(theExternalNodes.Size() != numExternalNodes)
          {
            std::cerr << "XC::ShadowSubdomain::getRemoteData(void) - need to resize the XC::ID\n";
            return -1;
          }
        if(numExternalNodes != 0)
          this->recvID(theExternalNodes);
        std::cerr << "XC::ShadowSubdomain::getREmoteData " << theExternalNodes;
      }
    gotRemoteData = true;
    return 0;
  }

bool XC::ShadowSubdomain::addElement(Element *theEle)
  {
    int tag = theEle->getTag();

#ifdef _G3DEBUG
        // do all the checking stuff
#endif

    msgData(0) = ShadowActorSubdomain_addElement;
    msgData(1) = theEle->getClassTag();
    msgData(2) = theEle->getDbTag();
    this->sendID(msgData);
    this->sendObject(*theEle);
    theElements[numElements] = tag;
    numElements++;
    //    this->domainChange();

    /*
    msgData(0) = 5;
    msgData(1) = 6;
    msgData(2) = 7;
    msgData(3) = 8;
    this->sendID(msgData);
    this->recvID(msgData);
    std::cerr << "XC::ShadowSubdomain::addElement() : " << msgData;
    */

    delete theEle;

    return true;
  }

bool XC::ShadowSubdomain::addNode(Node *theNode)
  {
    int tag = theNode->getTag();
#ifdef _G3DEBUG
        // do all the checking stuff
#endif
    msgData(0) = ShadowActorSubdomain_addNode;
    msgData(1) = theNode->getClassTag();
    msgData(2) = theNode->getDbTag();
    this->sendID(msgData);
    this->sendObject(*theNode);
    theNodes[numNodes] = tag;
    numNodes++;
    // this->domainChange();

    delete theNode;

    return true;
  }

bool XC::ShadowSubdomain::addExternalNode(Node *theNode)
  {
    int tag = theNode->getTag();
#ifdef _G3DEBUG
        // do all the checking stuff
#endif

    msgData(0) = ShadowActorSubdomain_addExternalNode;
    msgData(1) = theNode->getClassTag();
    msgData(2) = theNode->getDbTag();
    this->sendID(msgData);
    this->sendObject(*theNode);
    theNodes[numNodes] = tag;
    theExternalNodes[numExternalNodes] = tag;
    numNodes++;
    numExternalNodes++;
    numDOF += theNode->getNumberDOF();

    //    this->domainChange();

    return true;
  }

bool XC::ShadowSubdomain::addSFreedom_Constraint(SFreedom_Constraint *theSP)
  {

#ifdef _G3DEBUG
        // do all the checking stuff
#endif
    msgData(0) = ShadowActorSubdomain_addSFreedom_Constraint;
    msgData(1) = theSP->getClassTag();
    msgData(2) = theSP->getDbTag();
    this->sendID(msgData);
    this->sendObject(*theSP);
    numSPs++;
    // this->domainChange();

    theShadowSPs->addComponent(theSP);

    return true;
  }

bool XC::ShadowSubdomain::addMFreedom_Constraint(MFreedom_Constraint *theMP)
  {
#ifdef _G3DEBUG
        // do all the checking stuff
#endif
    msgData(0) = ShadowActorSubdomain_addMFreedom_Constraint;
    msgData(1) = theMP->getClassTag();
    msgData(2) = theMP->getDbTag();
    this->sendID(msgData);
    this->sendObject(*theMP);
    numMPs++;
    // // this->domainChange();

    theShadowMPs->addComponent(theMP);
    return true;
  }


bool XC::ShadowSubdomain::addLoadPattern(LoadPattern *thePattern)
  {
#ifdef _G3DEBUG
        // do all the checking stuff
#endif
    msgData(0) = ShadowActorSubdomain_addLoadPattern;
    msgData(1) = thePattern->getClassTag();
    msgData(2) = thePattern->getDbTag();
    this->sendID(msgData);
    this->sendObject(*thePattern);
    //    this->domainChange();


    theShadowLPs->addComponent(thePattern);
    numLoadPatterns++;

    return true;
  }



bool XC::ShadowSubdomain::addSFreedom_Constraint(SFreedom_Constraint *theSP, int loadPattern)
  {
#ifdef _G3DEBUG
        // do all the checking stuff
#endif

  /*
  LoadPattern *thePattern = this->XC::Subdomain::getLoadPattern(loadPattern);
  if((thePattern == 0) || (thePattern->addSFreedom_Constraint(theSP) == false)) {
    std::cerr << "XC::ShadowSubdomain::addSFreedom_Constraint() - could not add XC::SFreedom_Constraint: " << *theSP;
    return false;
  }
  */

  msgData(0) = ShadowActorSubdomain_addSFreedom_ConstraintToPattern;
  msgData(1) = theSP->getClassTag();
  msgData(2) = theSP->getDbTag();
  msgData(3) = loadPattern;
  this->sendID(msgData);
  this->sendObject(*theSP);
  numSPs++;
  // this->domainChange();

  return true;
  }

bool XC::ShadowSubdomain::addNodalLoad(NodalLoad *theLoad, int loadPattern)
  {
#ifdef _G3DEBUG
  // do all the checking stuff
#endif

  /*
  LoadPattern *thePattern = this->XC::Subdomain::getLoadPattern(loadPattern);
  if((thePattern == 0) || (thePattern->addNodalLoad(theLoad) == false)) {
    std::cerr << "XC::ShadowSubdomain::addNodalLoad() - could not add the load: " << *theLoad;
    return false;
  }
  std::cerr << "XC::ShadowSubdomain::addNodalLoad : " << this->getTag() << " " << theLoad->getNodeTag() << std::endl;
  */

  msgData(0) = ShadowActorSubdomain_addNodalLoadToPattern;
  msgData(1) = theLoad->getClassTag();
  msgData(2) = theLoad->getDbTag();
  msgData(3) = loadPattern;
  this->sendID(msgData);
  this->sendObject(*theLoad);

  return true;
  }

bool XC::ShadowSubdomain::addElementalLoad(ElementalLoad *theLoad, int loadPattern)
  {
    LoadPattern *thePattern = getConstraints().getLoadPattern(loadPattern);
    if((thePattern == 0) || (thePattern->addElementalLoad(theLoad) == false))
      {
        std::cerr << "XC::ShadowSubdomain::addElementalLoad() - could not add the load: " << *theLoad;
        return false;
      }
    msgData(0) = ShadowActorSubdomain_addElementalLoadToPattern;
    msgData(1) = theLoad->getClassTag();
    msgData(2) = theLoad->getDbTag();
    msgData(3) = loadPattern;
    this->sendID(msgData);
    this->sendObject(*theLoad);
    return true;
  }

bool XC::ShadowSubdomain::hasNode(int tag)
  {
    msgData(0) = ShadowActorSubdomain_hasNode;
    msgData(1) = tag;
    this->sendID(msgData);
    this->recvID(msgData);
    if(msgData(0) == 0)
      return true;
    else
      return false;
  }

bool XC::ShadowSubdomain::hasElement(int tag)
  {
    msgData(0) = ShadowActorSubdomain_hasElement;
    msgData(1) = tag;
    this->sendID(msgData);
    this->recvID(msgData);
    if(msgData(0) == 0)
      return true;
    else
      return false;
  }


bool XC::ShadowSubdomain::removeElement(int tag)
  {
    bool retval= false;
    const int loc= theElements.removeValue(tag);
    if(loc>=0)
      { // the element is there go get it
        // send a message to remove the object from the actor
        msgData(0) = ShadowActorSubdomain_removeElement;
        msgData(1) = tag;
        this->sendID(msgData);

        numElements--;
        retval= true;
//         // this->domainChange();

//         // get the element from the actor
//         this->recvID(msgData);
//         int theType = msgData(0);

//         if(theType == -1) // the ele was not there!
//             return 0;

//         Element *theEle = theBroker->getNewElement(theType);
//         if(theEle != 0)
//             this->recvObject(*theEle);

//         return theEle;
      }
    return retval;
  }

bool XC::ShadowSubdomain::removeNode(int tag)
  {
    bool retval= false;
    int loc = theNodes.removeValue(tag);
    if(loc>=0)
      { // the node is there, go get it
        // send a message to actor requesting node
        msgData(0) = ShadowActorSubdomain_removeNode;
        msgData(1) = tag;
        this->sendID(msgData);


        numNodes--;
        retval= true;
        // this->domainChange();
        // remove from external as well
        loc = theExternalNodes.removeValue(tag);
        if(loc >= 0)
          numExternalNodes--;

//         // receive the node from the actor
//         this->recvID(msgData);
//         int theType = msgData(0);

//         if(theType == -1)
//             return 0;

//         Node *theNode = theBroker->getNewNode(theType);
//         if(theNode != 0) {
//             this->recvObject(*theNode);
//             if(loc >= 0)
//                 numDOF -= theNode->getNumberDOF();
      }
    return retval;
  }

bool XC::ShadowSubdomain::removeSFreedom_Constraint(int tag)
  {
    bool retval= theShadowSPs->removeComponent(tag);
    if(retval)
      {
        msgData(0) = ShadowActorSubdomain_removeSFreedom_Constraint;
        msgData(1) = tag;
        this->sendID(msgData);
        numSPs--;
      }
    return retval;
  }

bool XC::ShadowSubdomain::removeMFreedom_Constraint(int tag)
  {
    bool retval= theShadowMPs->removeComponent(tag);
    if(retval)
      {
        msgData(0) = ShadowActorSubdomain_removeMFreedom_Constraint;
        msgData(1) = tag;
        this->sendID(msgData);
        numMPs--;
      }
    return retval;
  }

bool XC::ShadowSubdomain::removeLoadPattern(int loadTag)
  {
    bool retval= theShadowLPs->removeComponent(loadTag);
    if(retval)
      {
        msgData(0) = ShadowActorSubdomain_removeLoadPattern;
        msgData(1) = loadTag;
        this->sendID(msgData);
        numLoadPatterns--;
      }
    return retval;
  }

//! @brief Removes the nodal load from container.
bool XC::ShadowSubdomain::removeNodalLoad(int loadTag, int loadPattern)
  {
    bool res= false;
    TaggedObject *mc= theShadowLPs->getComponentPtr(loadPattern);
    if(mc)
      {
        LoadPattern *theLoadPattern= dynamic_cast<LoadPattern *>(mc);
        res= theLoadPattern->removeNodalLoad(loadTag);
        if(res)
          {
            msgData(0) = ShadowActorSubdomain_removeNodalLoadFromPattern;
            msgData(1) = loadTag;
            msgData(2) = loadPattern;
            this->sendID(msgData);
          }
      }
    return res;
  }

//! @brief Removes the elemental load from container.
bool XC::ShadowSubdomain::removeElementalLoad(int loadTag, int loadPattern)
  {
    bool res= false;
    // remove the object from the container
    TaggedObject *mc = theShadowLPs->getComponentPtr(loadPattern);
    if(mc)
      {
        LoadPattern *theLoadPattern= dynamic_cast<LoadPattern *>(mc);
        res = theLoadPattern->removeElementalLoad(loadTag);
        if(res)
          {
            msgData(0)= ShadowActorSubdomain_removeElementalLoadFromPattern;
            msgData(1)= loadTag;
            msgData(2)= loadPattern;
            this->sendID(msgData);
          }
      }
    return res;
  }

bool XC::ShadowSubdomain::removeSFreedom_Constraint(int loadTag, int loadPattern)
  {
    bool retval= false;
    // remove the object from the container
    TaggedObject *mc = theShadowLPs->getComponentPtr(loadPattern);
    if(mc)
      {
        LoadPattern *theLoadPattern = dynamic_cast<LoadPattern *>(mc);
        retval= theLoadPattern->removeSFreedom_Constraint(loadTag);
        if(retval)
          {
            msgData(0) = ShadowActorSubdomain_removeSFreedom_ConstraintFromPattern;
            msgData(1) = loadTag;
            msgData(2) = loadPattern;
            this->sendID(msgData);
          }
      }
    return retval;
  }


static XC::ArrayOfTaggedObjects fakeArray(nullptr,1,"Error");
static XC::SingleDomEleIter fakeEleIter(&fakeArray);
static XC::SingleDomNodIter fakeNodIter(&fakeArray);

XC::ElementIter &XC::ShadowSubdomain::getElements()
  {
    std::cerr << "XC::ShadowSubdomain::getElements() ";
    std::cerr << " - SHOULD NEVER BE CALLED - EXITING\n";
    exit(-1);

    // this will never be called - just for a strict compiler
    return fakeEleIter;
  }

XC::NodeIter &XC::ShadowSubdomain::getNodes()
  {
    std::cerr << "XC::ShadowSubdomain::getNodes() ";
    std::cerr << " - SHOULD NEVER BE CALLED - EXITING\n";
    exit(-1);

    // this will never be called - just for a strict compiler
    return fakeNodIter;
  }

XC::NodeIter &XC::ShadowSubdomain::getInternalNodeIter(void)
  {
    std::cerr << "XC::ShadowSubdomain::getInternalNodeIter() ";
    std::cerr << " - SHOULD NEVER BE CALLED - EXITING\n";
    exit(-1);

    // this will never be called - just for a strict compiler
    return fakeNodIter;
  }

XC::NodeIter &XC::ShadowSubdomain::getExternalNodeIter(void)
  {
    std::cerr << "XC::ShadowSubdomain::getExternalNodeIter() ";
    std::cerr << " - SHOULD NEVER BE CALLED - EXITING\n";
    exit(-1);

    // this will never be called - just for a strict compiler
    return fakeNodIter;
  }


XC::Element *XC::ShadowSubdomain::getElementPtr(int tag)
  {
    if(theElements.getLocation(tag) < 0)
        return 0;

    msgData(0) = ShadowActorSubdomain_getElementPtr;
    msgData(1) = tag;
    this->sendID(msgData);
    this->recvID(msgData);
    int theType = msgData(0);

    Element *theEle = theBroker->getNewElement(theType);
    if(theEle)
      { this->recvObject(*theEle); }
    std::cerr << "XC::ShadowSubdomain::getElementPtr() ";
    std::cerr << " - SHOULD BE AVOIDED IF POSSIBLE \n";

    return theEle;
  }

XC::Node *XC::ShadowSubdomain::getNodePtr(int tag)
  {
    if(theNodes.getLocation(tag) < 0)
        return 0;

    msgData(0) = ShadowActorSubdomain_getNodePtr;
    msgData(1) = tag;
    this->sendID(msgData);
    this->recvID(msgData);
    int theType = msgData(0);

    Node *theNod = theBroker->getNewNode(theType);
    if(theNod != 0) {
        this->recvObject(*theNod);
    }
    std::cerr << "XC::ShadowSubdomain::getNodPtr() ";
    std::cerr << " - SHOULD BE AVOIDED IF POSSIBLE \n";

    return theNod;
  }

int XC::ShadowSubdomain::getNumElements(void) const
  { return numElements; }

int XC::ShadowSubdomain::getNumLoadPatterns(void) const
  { return numLoadPatterns; }

int XC::ShadowSubdomain::getNumNodes(void) const
  { return numNodes; }

int XC::ShadowSubdomain::getNumSPs(void) const
  { return numSPs; }

int XC::ShadowSubdomain::getNumMPs(void) const
  { return numMPs; }

static XC::Graph fakeGraph;

XC::Graph &XC::ShadowSubdomain::getElementGraph(void)
  {
    std::cerr << "XC::ShadowSubdomain::getElementGraph() ";
    std::cerr << " - NOT YET IMPLEMENTED - EXITING\n";
    exit(-1);

    // this will never be called, needed for a strict compiler
    return fakeGraph;
  }

XC::Graph &XC::ShadowSubdomain::getNodeGraph(void)
  {
    std::cerr << "XC::ShadowSubdomain::getNodeGraph() ";
    std::cerr << " - NOT YET IMPLEMENTED - EXITING\n";
    exit(-1);

    // this will never be called, needed for a strict compiler
    return fakeGraph;
  }


void XC::ShadowSubdomain::applyLoad(double time)
  {
    DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
    if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true)
      {
        msgData(0) = ShadowActorSubdomain_applyLoad;
        Vector data(4);
        data(0) = time;

        this->sendID(msgData);
        this->sendVector(data);
      }
  }


void XC::ShadowSubdomain::setCommitTag(int newTag)
  {
    msgData(0) = ShadowActorSubdomain_setCommitTag;
    msgData(1) = newTag;

    //    this->sendID(msgData);
  }

void XC::ShadowSubdomain::setCurrentTime(double time)
  {
  DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
  if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true) {
    msgData(0) = ShadowActorSubdomain_setCurrentTime;
    Vector data(4);
    data(0) = time;

    this->sendID(msgData);
    this->sendVector(data);
  }
  }

void XC::ShadowSubdomain::setCommittedTime(double time)
  {
    msgData(0) = ShadowActorSubdomain_setCommittedTime;
    Vector data(4);
    data(0) = time;

    this->sendID(msgData);
    this->sendVector(data);
  }

void XC::ShadowSubdomain::setLoadConstant(void)
  {
    msgData(0) = ShadowActorSubdomain_setLoadConstant;

    this->sendID(msgData);
  }


int XC::ShadowSubdomain::update(void)
  {
  DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
  if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true) {
    msgData(0) =  ShadowActorSubdomain_update;
    this->sendID(msgData);
  }
  return 0;
  }

int XC::ShadowSubdomain::update(double newTime, double dT)
  {
    static Vector data(2);
    DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
    if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true)
      {
        msgData(0) =  ShadowActorSubdomain_updateTimeDt;
        this->sendID(msgData);
        data(0) = newTime;
        data(1) = dT;
        this->sendVector(data);
      }
    return 0;
  }


int XC::ShadowSubdomain::barrierCheckIN(void)
  {
    static ID data(1);
    this->recvID(data);
    return data(0);
  }

int XC::ShadowSubdomain::barrierCheckOUT(int result)
  {
    static ID data(1);
    data(0) = result;
    this->sendID(data);
    return 0;
  }


int XC::ShadowSubdomain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  {
    msgData(0) = ShadowActorSubdomain_setRayleighDampingFactors;
    this->sendID(msgData);
    static Vector data(4);
//     data(0) = alphaM;
//     data(1) = betaK;
//     data(2) = betaK0;
//     data(3) = betaKc;
    this->sendVector(data);
    return 0;
  }




void XC::ShadowSubdomain::clearAll(void)
  {
    msgData(0) = ShadowActorSubdomain_clearAll;
    this->sendID(msgData);
    this->recvID(msgData);
  }


int XC::ShadowSubdomain::addRecorder(Recorder &theRecorder)
  {
  msgData(0) = ShadowActorSubdomain_addRecorder;
  msgData(1) = theRecorder.getClassTag();
  this->sendID(msgData);
  this->sendObject(theRecorder);
  return 0;
  }

int XC::ShadowSubdomain::removeRecorders(void)
  {
  msgData(0) = ShadowActorSubdomain_removeRecorders;
  this->sendID(msgData);
  return 0;
  }

int XC::ShadowSubdomain::commit(void)
  {
  DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
  if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true) {
    msgData(0) = ShadowActorSubdomain_commit;
    this->sendID(msgData);
    return 0;
  }
  return 0;
  }

int XC::ShadowSubdomain::revertToLastCommit(void)
  {
  DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
  if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true) {
    msgData(0) = ShadowActorSubdomain_revertToLastCommit;
    this->sendID(msgData);
    return 0;
  }
  return 0;
  }

int XC::ShadowSubdomain::revertToStart(void)
  {
  msgData(0) = ShadowActorSubdomain_revertToStart;
  this->sendID(msgData);
  return 0;
  }


void XC::ShadowSubdomain::wipeAnalysis(void)
  {
  msgData(0) = ShadowActorSubdomain_wipeAnalysis;

  this->sendID(msgData);
  }

void XC::ShadowSubdomain::setDomainDecompAnalysis(DomainDecompositionAnalysis &theDDAnalysis)
  {
    msgData(0) = ShadowActorSubdomain_setDomainDecompAnalysis;
    msgData(1) = theDDAnalysis.getClassTag();

    this->sendID(msgData);
    this->sendObject(theDDAnalysis);

    this->XC::Subdomain::setDomainDecompAnalysis(theDDAnalysis);
  }

int XC::ShadowSubdomain::setAnalysisAlgorithm(EquiSolnAlgo &theAlgorithm)
  {
    msgData(0) = ShadowActorSubdomain_setAnalysisAlgorithm;
    msgData(1) = theAlgorithm.getClassTag();

    this->sendID(msgData);
    this->sendObject(theAlgorithm);
    return 0;
  }

int XC::ShadowSubdomain::setAnalysisIntegrator(IncrementalIntegrator &theIntegrator)
  {
    msgData(0) = ShadowActorSubdomain_setAnalysisIntegrator;
    msgData(1) = theIntegrator.getClassTag();

    this->sendID(msgData);
    this->sendObject(theIntegrator);
    return 0;
  }

int XC::ShadowSubdomain::setAnalysisLinearSOE(LinearSOE &theSOE)
  {
    msgData(0) = ShadowActorSubdomain_setAnalysisLinearSOE;
    msgData(1) = theSOE.getClassTag();

    LinearSOESolver *theSolver = theSOE.getSolver();
    msgData(2) = theSolver->getClassTag();

    this->sendID(msgData);
    this->sendObject(theSOE);
    this->sendObject(*theSolver);

    return 0;
  }

int XC::ShadowSubdomain::setAnalysisConvergenceTest(ConvergenceTest &theTest)
  {
    msgData(0) = ShadowActorSubdomain_setAnalysisConvergenceTest;
    msgData(1) = theTest.getClassTag();

    this->sendID(msgData);
    this->sendObject(theTest);

    std::cerr << "XC::ShadowSubdomain::setAnalysisConvergenceTest(ConvergenceTest &theTest)\n";
    return 0;
  }



void XC::ShadowSubdomain::domainChange(void)
  {
    msgData(0) = ShadowActorSubdomain_domainChange;
    this->sendID(msgData);

    resize_vectors(numDOF);
  }

void XC::ShadowSubdomain::clearAnalysis(void)
  {
    msgData(0) = ShadowActorSubdomain_clearAnalysis;
    this->sendID(msgData);
  }

int XC::ShadowSubdomain::getNumExternalNodes(void) const
  {
    if(gotRemoteData == false && buildRemote == true)
      {
        std::cerr << "WARNING: XC::ShadowSubdomain::getNumExternalNodes()";
        std::cerr << " - not yet received the data\n";
      }
    return numExternalNodes;
  }

const XC::ID &XC::ShadowSubdomain::getExternalNodes(void) const
  {
    ShadowSubdomain *this_no_const= const_cast<ShadowSubdomain *>(this);
    // if the subdoamin was built remotly need to get it's data
    if(gotRemoteData == false && buildRemote == true)
      this_no_const->getRemoteData();
    return theExternalNodes;
  }

int XC::ShadowSubdomain::getNumDOF(void) const
  {
    // if the subdoamin was built remotly need to get it's data
    ShadowSubdomain *this_no_const= const_cast<ShadowSubdomain *>(this);
    if(gotRemoteData == false && buildRemote == true)
      this_no_const->getRemoteData();
    return numDOF;
  }


const XC::Matrix &XC::ShadowSubdomain::getTang(void)
  {
  // if the subdoamin was built remotly need to get it's data
  if(gotRemoteData == false && buildRemote == true)
    this->getRemoteData();

    msgData(0) =  ShadowActorSubdomain_getTang;
    this->sendID(msgData);

    resize_vectors(numDOF);

    this->recvMatrix(*theMatrix);
    return *theMatrix;
  }



const XC::Vector &XC::ShadowSubdomain::getResistingForce(void) const
  {
    ShadowSubdomain *this_no_const= const_cast<ShadowSubdomain *>(this);
    // if the subdoamin was built remotly need to get it's data
    if(gotRemoteData == false && buildRemote == true)
      this_no_const->getRemoteData();

    msgData(0) = ShadowActorSubdomain_getResistingForce;
    this_no_const->sendID(msgData);

    resize_vectors(numDOF);

    this_no_const->recvVector(*theVector);
    return *theVector;
  }


int XC::ShadowSubdomain::computeTang(void)
  {
    count++;

    if(count == 1) {
      msgData(0) = ShadowActorSubdomain_computeTang;
      msgData(1) = this->getTag();
      this->sendID(msgData);

      for(int i = 0; i < numShadowSubdomains; i++) {
        ShadowSubdomain *theShadow = theShadowSubdomains[i];
        if(theShadow != this)
          theShadow->computeTang();
      }
    }
    else if(count <= numShadowSubdomains) {
      msgData(0) = ShadowActorSubdomain_computeTang;
      msgData(1) = this->getTag();
      this->sendID(msgData);
    }
    else if(count == 2*numShadowSubdomains - 1)
      count = 0;

    return 0;
  }

int XC::ShadowSubdomain::computeResidual(void)
  {
    count++;

    if(count == 1) {
      msgData(0) = ShadowActorSubdomain_computeResidual;
      this->sendID(msgData);

      for(int i = 0; i < numShadowSubdomains; i++) {
        ShadowSubdomain *theShadow = theShadowSubdomains[i];
        if(theShadow != this)
          theShadow->computeResidual();
      }
    }
    else if(count <= numShadowSubdomains) {
      msgData(0) = ShadowActorSubdomain_computeResidual;
      this->sendID(msgData);
    }
    else if(count == 2*numShadowSubdomains - 1)
      count = 0;

    return 0;
  }


static XC::Vector fakeVector(0);

const XC::Vector &XC::ShadowSubdomain::getLastExternalSysResponse(void)
  {
    std::cerr << "XC::ShadowSubdomain::getLastExternalSysResponse() ";
    std::cerr << " SHOULD NEVER BE CALLED\n";
    exit(0);

    // this should never be called, provided for a strict compiler
    return fakeVector;
  }

int XC::ShadowSubdomain::computeNodalResponse(void)
  {

  DomainDecompositionAnalysis *theDDA = this->getDDAnalysis();
  if(theDDA != 0 && theDDA->doesIndependentAnalysis() != true) {
    FE_Element *theFePtr = this->getFE_ElementPtr();

    if(theFePtr != 0) {

      const Vector &lastChange = theFePtr->getLastResponse();
      msgData(0) =  ShadowActorSubdomain_computeNodalResponse;
      msgData(1) = lastChange.Size();
      if(numDOF != msgData(1)) {
        std::cerr << "XC::ShadowSubdomain::update(void)";
        std::cerr << " - numDOF " << numDOF << " and size of XC::Vector ";
        std::cerr << msgData(1) << "do not agree?\n";
        numDOF = msgData(1);
      }
      this->sendID(msgData);
      Vector theChange(lastChange);
      this->sendVector(theChange);
    }
  }

  return 0;
  }


int XC::ShadowSubdomain::newStep(double dT)
  {
    msgData(0) =  ShadowActorSubdomain_newStep;
    this->sendID(msgData);
    static Vector timeStep(4);
    timeStep(0) = dT;
    this->sendVector(timeStep);
    return 0;
  }


double XC::ShadowSubdomain::getCost(void)
  {
  /*
    msgData(0) = ShadowActorSubdomain_getCost;

    this->sendID(msgData);
    Vector cost(2);
    this->recvVector(cost);
    return cost(0);
  */
  return 0.0;
  }


int XC::ShadowSubdomain::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::ShadowSubdomain::sendSelf() ";
    std::cerr << " - NOT YET IMPLEMENTED\n";
    return -1;
  }

int XC::ShadowSubdomain::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::ShadowSubdomain::recvSelf() ";
    std::cerr << " - NOT YET IMPLEMENTED\n";
    return -1;
  }


void XC::ShadowSubdomain::Print(std::ostream &s, int flag) const
  {
    msgData(0) = ShadowActorSubdomain_Print;

    ShadowSubdomain *this_no_const= const_cast<ShadowSubdomain *>(this);
    // I'ts weird this.
    this_no_const->sendID(msgData);
    this_no_const->recvID(msgData);
  }


int XC::ShadowSubdomain::buildEleGraph(Graph &theEleGraph)
  {
    std::cerr << "XC::ShadowSubdomain::buildEleGraph() ";
    std::cerr << " - NOT YET IMPLEMENTED\n";
    return -1;
  }

int XC::ShadowSubdomain::buildNodeGraph(Graph &theNodeGraph)
  {
    std::cerr << "XC::ShadowSubdomain::buildNodeGraph() ";
    std::cerr << " - NOT YET IMPLEMENTED\n";
    return -1;
  }

int XC::ShadowSubdomain::buildMap(void) const
  {
    mapBuilt = true;
    return 0;
  }



double XC::ShadowSubdomain::getNodeDisp(int nodeTag, int dof, int &errorFlag)
  {
    static Vector data(1);
    data(0) = 0.0;

    msgData(0) = ShadowActorSubdomain_getNodeDisp;
    msgData(1) = nodeTag;
    msgData(2) = dof;
    this->sendID(msgData);
    this->recvID(msgData);
    errorFlag = msgData(0);
    if(errorFlag == 0)
      { this->recvVector(data); }
    return data(0);
  }

int XC::ShadowSubdomain::setMass(const XC::Matrix &mass, int nodeTag)
  {
    msgData(0) = ShadowActorSubdomain_setMass;
    msgData(1) = nodeTag;
    msgData(2) = mass.noRows();
    msgData(3) = mass.noCols();
    this->sendID(msgData);
    this->sendMatrix(mass);
    this->recvID(msgData);
    return msgData(0);
  }






