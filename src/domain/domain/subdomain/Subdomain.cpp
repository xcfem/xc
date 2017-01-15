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

// $Revision: 1.7 $
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/Subdomain.cpp,v $

// Written: fmk
// Revision: A
// Revision: B 03/98 - revised to allow parallel model generation
//
// Description: This file contains the class definition for XC::Subdomain.
// Subdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints
// and MFreedom_Constraints that have been added to the subdomain.
//
// What: "@(#) Subdomain.C, revA"

#include "Subdomain.h"
#include <cstdlib>

#include <domain/component/DomainComponent.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/element/NodePtrsWithIDs.h"
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <domain/domain/single/SingleDomNodIter.h>
#include "classTags.h"
#include "domain/domain/subdomain/modelbuilder/PartitionedModelBuilder.h"
#include <solution/analysis/model/dof_grp/DOF_Group.h>

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "utility/matrix/Matrix.h"

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <utility/Timer.h>


XC::Matrix XC::Subdomain::badResult(1,1); // for returns from getStiff, getMass and getDamp


XC::Subdomain::Subdomain(int tag,DataOutputHandler::map_output_handlers *oh,EntCmd *owr)
 :Element(tag,ELE_TAG_Subdomain),
  Domain(owr,oh),
  realCost(0.0),cpuCost(0.0),pageCost(0),
  theAnalysis(nullptr), extNodes(nullptr), theFEele(nullptr),
  thePartitionedModelBuilder(nullptr),
  mapBuilt(false),map(0),mappedVect(0),mappedMatrix(0)
  {
    // init the arrays.
    internalNodes= new ArrayOfTaggedObjects(static_cast<Domain *>(this),8024,"intNod");
    externalNodes= new ArrayOfTaggedObjects(static_cast<Domain *>(this),256,"extNod");
    //    realExternalNodes = new XC::ArrayOfTaggedObjects(256);

    internalNodeIter = new SingleDomNodIter(internalNodes);
    externalNodeIter = new SingleDomNodIter(externalNodes);
    theNodIter = new SubdomainNodIter(*this);

    // check that space was available
    if(internalNodes == 0 || externalNodes == 0 ||
        internalNodeIter == 0 || externalNodeIter == 0 ||
        theNodIter == 0) {

        std::cerr << "XC::Subdomain::Subdomain() - ran out of memory\n";
        exit(-1);
    }
}


XC::Subdomain::~Subdomain(void)
  {
    if(internalNodes) delete internalNodes;
    if(externalNodes) delete externalNodes;
    if(internalNodeIter) delete internalNodeIter;
    if(externalNodeIter) delete externalNodeIter;
    if(map) delete map;
    if(mappedVect) delete mappedVect;
    if(mappedMatrix) delete mappedMatrix;
  }

void XC::Subdomain::clearAll(void)
  {
    Domain::clearAll();
    if(internalNodes) internalNodes->clearAll();
    if(externalNodes) externalNodes->clearAll();
  }

int XC::Subdomain::buildSubdomain(int numSubdomains, PartitionedModelBuilder &theBuilder)
  {
    int result = theBuilder.buildSubdomain(this->getTag(), numSubdomains, *this);
    if(result == 0)
      {
        this->hasDomainChanged();
        this->invokeChangeOnAnalysis();
      }
    return result;
  }




//! @brief Method to add a Node to the model.
bool XC::Subdomain::addNode(Node *node)
  {
#ifdef _G3DEBUG
//  int nodTag = node->getTag();
//  // check no other node exists with same tag
//  Node *nodePtr = this->getNodePtr(nodTag);
//  if(nodePtr != 0)
//    return false;
//
//      // MISSING CODE
#endif

    bool result = internalNodes->addComponent(node);
    if(result == true)
      {
        node->setDomain(this);
        this->domainChange();
      }
    return result;
  }

bool XC::Subdomain::addExternalNode(Node *thePtr)
  {
#ifdef _G3DEBUG
  // check to see it has not alredy been added

    int nodTag = thePtr->getTag();
    TaggedObject *other = externalNodes->getComponentPtr(nodTag);
    if(other != 0)
      return false;

    other = internalNodes->getComponentPtr(nodTag);
    if(other != 0)
      return false;

#endif
      // create a dummy XC::Node & try adding it to the external nodes
      Node *newDummy = new Node(*thePtr, false);
      if(newDummy == 0)
        return false;

      bool result = externalNodes->addComponent(newDummy);
      if(result == true)
        {
          //        result = realExternalNodes->addComponent(thePtr);
          newDummy->setDomain(this);
          this->domainChange();
        }
      return result;
  }


bool XC::Subdomain::removeNode(int tag)
  {
    bool result= internalNodes->removeComponent(tag);
    if(!result)
      {
        result= externalNodes->removeComponent(tag);
        if(result)
          this->domainChange();
      }
    else
      this->domainChange();
    return result;
  }

XC::NodeIter &XC::Subdomain::getNodes(void)
  {
    theNodIter->reset();
    return *theNodIter;
  }

XC::NodePtrsWithIDs &XC::Subdomain::getNodePtrs(void)
  {
    std::cerr << "Subdomain::getNodePtrs() - should not be called\n";
    static NodePtrsWithIDs retval(this,1);
    return retval;
  }

const XC::NodePtrsWithIDs &XC::Subdomain::getNodePtrs(void) const
  {
    std::cerr << "Subdomain::getNodePtrs() - should not be called\n";
    static NodePtrsWithIDs retval(const_cast<Subdomain *>(this),1);
    return retval;
  }

const XC::Node *XC::Subdomain::getNode(int tag) const
  {
    const TaggedObject *object = internalNodes->getComponentPtr(tag);
    if(!object)
      {
        object = externalNodes->getComponentPtr(tag);
        if(object)
          {
            const Node *result= dynamic_cast<const Node *>(object);
            return result;
          }
      }
    else
      {
        const Node *result = dynamic_cast<const Node *>(object);
        return result;
      }
    return 0;
  }

XC::Node *XC::Subdomain::getNode(int tag)
  {
    TaggedObject *object = internalNodes->getComponentPtr(tag);
    if(!object)
      {
        object = externalNodes->getComponentPtr(tag);
        if(object)
          {
            Node *result= dynamic_cast<Node *>(object);
            return result;
          }
      }
    else
      {
        Node *result = dynamic_cast<Node *>(object);
        return result;
      }
    return 0;
  }

bool XC::Subdomain::hasNode(int tag)
  {
    if(this->getNode(tag) != 0)
      return true;
    else
      return false;
  }

bool XC::Subdomain::hasElement(int tag)
  {
    if(this->getElement(tag) != 0)
      return true;
    else
      return false;
  }


int XC::Subdomain::getNumNodes(void) const
  { return internalNodes->getNumComponents() + externalNodes->getNumComponents(); }

int XC::Subdomain::commit(void)
  {
    Domain::commit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != 0)
      nodePtr->commitState();
    return 0;
  }

int XC::Subdomain::revertToLastCommit(void)
  {
    Domain::revertToLastCommit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != 0)
        nodePtr->revertToLastCommit();
    return 0;
  }

int XC::Subdomain::revertToStart(void)
  {
    Domain::revertToLastCommit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != 0)
      nodePtr->revertToStart();
    return 0;
  }

int XC::Subdomain::update(void)
  { return Domain::update(); }

int XC::Subdomain::update(double newTime, double dT)
  { return Domain::update(newTime, dT); }

void XC::Subdomain::Print(std::ostream &s, int flag)
  {
    s << "Current XC::Subdomain XC::Information for XC::Subdomain: ";
    s << this->getTag() << "\n";

    s << "\nINTERNAL NODE DATA: NumNodes: ";
    s << internalNodes->getNumComponents() << "\n";
    internalNodes->Print(s);

    s << "\nEXTERNAL NODE DATA: NumNodes: ";
    s << externalNodes->getNumComponents() << "\n";
    externalNodes->Print(s);

    this->XC::Domain::Print(s);
    s << "\nEnd XC::Subdomain XC::Information\n";
  }



XC::NodeIter &XC::Subdomain::getInternalNodeIter(void)
  {
    internalNodeIter->reset();
    return *internalNodeIter;
  }


XC::NodeIter &XC::Subdomain::getExternalNodeIter(void) const
  {
    externalNodeIter->reset();
    return *externalNodeIter;
  }



void XC::Subdomain::wipeAnalysis(void)
  {
    if(theAnalysis)
      {
        theAnalysis->clearAll();
        delete theAnalysis;
      }
  }

void XC::Subdomain::setDomainDecompAnalysis(DomainDecompositionAnalysis &theNewAnalysis)
  {
    theAnalysis = &theNewAnalysis;
    //    this->XC::Domain::setAnalysis(theNewAnalysis);
  }


int XC::Subdomain::setAnalysisAlgorithm(EquiSolnAlgo &theAlgorithm)
  {
    if(theAnalysis)
      return theAnalysis->setAlgorithm(theAlgorithm);
    return 0;
  }

int XC::Subdomain::setAnalysisIntegrator(IncrementalIntegrator &theIntegrator)
  {
    if(theAnalysis)
      return theAnalysis->setIntegrator(theIntegrator);
    return 0;
  }

int XC::Subdomain::setAnalysisLinearSOE(LinearSOE &theSOE)
  {
    if(theAnalysis)
      return theAnalysis->setLinearSOE(theSOE);
    return 0;
  }

int XC::Subdomain::setAnalysisConvergenceTest(ConvergenceTest &theTest)
  {
    if(theAnalysis)
      return theAnalysis->setConvergenceTest(theTest);
    return 0;
  }

int XC::Subdomain::invokeChangeOnAnalysis(void)
  {
    int result = 0;
    if(theAnalysis)
      result = theAnalysis->domainChanged();

    mapBuilt = false;
    return result;
  }


int XC::Subdomain::getNumExternalNodes(void) const
  { return externalNodes->getNumComponents(); }

const XC::ID &XC::Subdomain::getExternalNodes(void) const
  {
    // first we check that extNodes exists and is of correct size
    int numExt = externalNodes->getNumComponents();
    if(!extNodes)
      {
        extNodes= new XC::ID(numExt);
        if(extNodes == 0 || extNodes->Size() != numExt)
          {
            std::cerr << "XC::Subdomain::getExternalNodes(): ";
            std::cerr << " - ran out of memory for size " << numExt <<std::endl;
            exit(-1);
          }
      }

    if(extNodes->Size() != numExt)
      {
        delete extNodes;
        extNodes = new XC::ID(numExt);
        if(extNodes == 0 || extNodes->Size() != numExt)
          {
            std::cerr << "XC::Subdomain::getExternalNodes(): ";
            std::cerr << " - ran out of memory for size " << numExt <<std::endl;
            exit(-1);
          }
      }

    // we now set the values of extNodes to be the node tags of the
    // external nodes

    NodeIter &theExtNodes = this->getExternalNodeIter();
    Node *nodPtr;
    int cnt = 0;

    while ((nodPtr = theExtNodes()) != 0)
      (*extNodes)(cnt++) = nodPtr->getTag();

    // done
    ID &res = *extNodes;
    return res;
  }



int XC::Subdomain::getNumDOF(void) const
  {
    if(theAnalysis)
      return theAnalysis->getNumExternalEqn();
    else
      {
        //   std::cerr << "XC::Subdomain::getNumDOF() - no XC::StaticAnalysis has been set\n";
        return 0;
      }
  }

int XC::Subdomain::commitState(void)
  { return this->commit(); }

const XC::Matrix &XC::Subdomain::getTangentStiff(void)
  {
    std::cerr << "XC::Subdomain::getTangentStiff(void)";
    std::cerr << "DOES NOT DO ANYTHING";
    return badResult;
  }

const XC::Matrix &XC::Subdomain::getInitialStiff(void)
  {
    std::cerr << "XC::Subdomain::getSecantStiff(void)";
    std::cerr << "DOES NOT DO ANYTHING";
    return badResult;
  }

const XC::Matrix &XC::Subdomain::getDamp(void)
  {
    std::cerr << "XC::Subdomain::getDamp(void)";
    std::cerr << "DOES NOT DO ANYTHING";
    return badResult;
  }

const XC::Matrix &XC::Subdomain::getMass(void)
  {
    std::cerr << "XC::Subdomain::getMass(void)";
    std::cerr << "DOES NOT DO ANYTHING";
    return badResult;
  }




void XC::Subdomain::zeroLoad(void)
  {
    std::cerr << "XC::Subdomain::zeroLoad() - should not be called\n";
  }


int XC::Subdomain::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "XC::Subdomain::addLoad() - should not be called\n";
    return 0;
  }

int XC::Subdomain::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }


const XC::Vector &XC::Subdomain::getResistingForce(void) const
  {
    if(!theAnalysis)
      {
        std::cerr << "XC::Subdomain::getResistingForce() ";
        std::cerr << " - no StaticCondensationAnalysis has been set\n";
        exit(-1);
      }

    if(!mapBuilt)
      this->buildMap();

    ID &theMap = *map;
    const XC::Vector &anaResidual = theAnalysis->getResidual();
    int numDOF = this->getNumDOF();
    for(int i=0; i<numDOF; i++)
      (*mappedVect)(i) = anaResidual(theMap(i));
    //std::cerr << "XC::Subdomain::getResidual() : " << *mappedVect;
    return *mappedVect;
  }


const XC::Vector &XC::Subdomain::getResistingForceIncInertia(void) const
  {
    std::cerr << "XC::Subdomain::getResistingForceWithInertia() ";
    std::cerr << " - should not be called\n";

    return this->getResistingForce();
  }



bool XC::Subdomain::isSubdomain(void)
  { return true; }


int XC::Subdomain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { return Domain::setRayleighDampingFactors(rF); }

int XC::Subdomain::computeTang(void)
  {
    if(theAnalysis)
      {
        theTimer.start();

        int res =0;
        res = theAnalysis->formTangent();
        return res;
      }
    else
      {
        std::cerr << "XC::Subdomain::getcomputeTang() ";
        std::cerr << " - no StaticCondensationAnalysis has been set\n";
        return 0;
      }
  }

int XC::Subdomain::computeResidual(void)
  {
    if(theAnalysis)
      {
        theTimer.start();

        int res =0;
        res = theAnalysis->formResidual();

        theTimer.pause();
        realCost += theTimer.getReal();
        cpuCost += theTimer.getCPU();
        pageCost += theTimer.getNumPageFaults();

        return res;
      }
    else
      {
        std::cerr << "XC::Subdomain::computeResidual() ";
        std::cerr << " - no StaticCondensationAnalysis has been set\n";
        return 0;
      }
  }

const XC::Matrix &XC::Subdomain::getTang(void)
  {
    if(!theAnalysis)
      {
        std::cerr << "XC::Subdomain::getTang() ";
        std::cerr << " - no StaticCondensationAnalysis has been set\n";
        exit(-1);
      }

    if(mapBuilt == false)
        this->buildMap();

    ID &theMap = *map;
    const XC::Matrix &anaTang = theAnalysis->getTangent();
    int numDOF = this->getNumDOF();
    for(int i=0; i<numDOF; i++)
      for(int j=0; j<numDOF; j++)
        (*mappedMatrix)(i,j) = anaTang(theMap(i),theMap(j));
    return *mappedMatrix;
  }


void XC::Subdomain::setFE_ElementPtr(FE_Element *theFE_Ele)
  { theFEele = theFE_Ele; }


XC::FE_Element *XC::Subdomain::getFE_ElementPtr(void)
  { return theFEele; }



const XC::Vector &XC::Subdomain::getLastExternalSysResponse(void)
  {
    if(!theFEele)
      {
        std::cerr << "FATAL ERROR: XC::Subdomain::getLastExternalSysResponse() :";
        std::cerr << " - no XC::FE_Element *exists for a subdomain\n";
        std::cerr << " This is the responsibilty of the FE_ELement constructor\n";
        exit(0);
      }

    // get the response from the FE_ele for the nodal
    // quantities - WARNING this is expressed in global dof

    if(mapBuilt == false)
      this->buildMap();

    ID &theMap = *map;
    const XC::Vector &localResponse = theFEele->getLastResponse();
    int numDOF = this->getNumDOF();
    for(int i=0; i<numDOF; i++)
      (*mappedVect)(theMap(i)) = localResponse(i);
    return *mappedVect;
  }


int XC::Subdomain::computeNodalResponse(void)
  {
    int res =0;
    if(theAnalysis)
      {
        res = theAnalysis->computeInternalResponse();
        return res;
      }
    else
      {
        std::cerr << "XC::Subdomain::computeNodalResponse() ";
        std::cerr << "- no XC::StaticAnalysis has been set\n";
        return 0;
      }
  }


int XC::Subdomain::newStep(double dT)
  {
    if(theAnalysis)
      return theAnalysis->newStep(dT);
    return 0;
  }


bool XC::Subdomain::doesIndependentAnalysis(void)
  {
    if(theAnalysis != 0)
      return theAnalysis->doesIndependentAnalysis();
    else
      return true;
  }


int XC::Subdomain::sendSelf(CommParameters &cp)
  {
    int dataTag = this->getDbTag();
    if(theAnalysis)
      {
        ID data(2);
        data(0) = theAnalysis->getClassTag();
        data(1) = 0;
        cp.sendIdData(Element::getDbTagData(),dataTag);

        return theAnalysis->sendSelf(cp);
      }
    else
      { std::cerr << "XC::Subdomain::sendSelf - no analysis set\n"; }
    return -1;
  }

int XC::Subdomain::recvSelf(const CommParameters &cp)
  {
    int dataTag = this->getDbTag();
    ID data(2);
    cp.receiveIdData(Element::getDbTagData(),dataTag);
    if(data(1) == 0)
      {
        theAnalysis = cp.getNewDomainDecompAnalysis(Element::getDbTagDataPos(0),*this);
        if(theAnalysis)
          return theAnalysis->recvSelf(cp);
      }
    return -1;
  }

double XC::Subdomain::getCost(void)
  {
    double lastRealCost = realCost;
    realCost = 0.0;
    cpuCost = 0.0;
    pageCost = 0;
    return lastRealCost;
  }


int XC::Subdomain::buildMap(void) const
  {
    if(mapBuilt == false)
      {
        // determine the mapping between local dof and subdomain ana dof
        int numDOF = this->getNumDOF();
        if(map == 0)
          map = new XC::ID(numDOF);
        if(map->Size() != numDOF)
          {
            delete map;
            map = new XC::ID(numDOF);
          }

        //int numExt = theAnalysis->getNumExternalEqn();
        int numInt = theAnalysis->getNumInternalEqn();

        const ID &theExtNodes = this->getExternalNodes();
        int numExtNodes = theExtNodes.Size();
        int locInMap =0;
        for(int i=0; i<numExtNodes; i++)
          {
            Node *nodePtr= const_cast<Node *>(this->getNode(theExtNodes(i)));
            int numNodeDOF = nodePtr->getNumberDOF();
            DOF_Group *theDOF = nodePtr->getDOF_GroupPtr();
            const ID &theLocalID = theDOF->getID();
            for(int j=0; j<numNodeDOF; j++)
              {
                int locInSubdomainExt = theLocalID(j)-numInt;
                (*map)(locInMap)=locInSubdomainExt;
                locInMap++;
              }
          }
        mapBuilt = true;

        if(mappedVect == 0)
          mappedVect = new XC::Vector(numDOF);
        if(mappedVect->Size() != numDOF)
          {
            delete mappedVect;
            mappedVect = new XC::Vector(numDOF);
          }
        if(mappedMatrix == 0)
          mappedMatrix = new XC::Matrix(numDOF,numDOF);
        if(mappedMatrix->noRows() != numDOF)
          {
            delete mappedMatrix;
            mappedMatrix = new XC::Matrix(numDOF,numDOF);
          }
      }
    return 0;
  }


XC::DomainDecompositionAnalysis *XC::Subdomain::getDDAnalysis(void)
  { return theAnalysis; }
