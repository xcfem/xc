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
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
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

//! @brief Constructor.
//!
//! Constructs an empty Subdomain with a number given by \p tag. The
//! storage of the Elements, constraints and loads are handled by the base
//! Domain class, the storage of the internal and external nodes are
//! handle by the Subdomain class. For storage of the Nodes objects of
//! type ArrayOfTaggedObjects are used. The constructors for these objects
//! take initial sizes of the arrays, the sizes used are:
//! InternalNodeArray = 8024, ExternalNodeArray = 256. The initial sizes
//! are not upperbounds on the number of components that can be added.
XC::Subdomain::Subdomain(int tag,DataOutputHandler::map_output_handlers *oh,CommandEntity *owr)
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
    //    realExternalNodes = new ArrayOfTaggedObjects(256);

    internalNodeIter = new SingleDomNodIter(internalNodes);
    externalNodeIter = new SingleDomNodIter(externalNodes);
    theNodIter = new SubdomainNodIter(*this);

    // check that space was available
    if(internalNodes == nullptr || externalNodes == nullptr ||
        internalNodeIter == nullptr || externalNodeIter == nullptr ||
        theNodIter == nullptr)
      {

        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Destructor.
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




//! @brief Method to add a node to the subdomain.
//!
//! A Method to add the node pointed to by \p node to the
//! Subdomain. This node is an internal node to the domain and is
//! returned in calls to getNodes()} and {\em getInternalNodes(). 
//! Invokes setDomain(this)} on the Node and {\em domainChanged() on
//! itself. 
bool XC::Subdomain::addNode(Node *node)
  {
#ifdef _G3DEBUG
//  int nodTag = node->getTag();
//  // check no other node exists with same tag
//  Node *nodePtr = this->getNodePtr(nodTag);
//  if(nodePtr != nullptr)
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

//! A Method to add the node pointed to by the argument.
//!
//! A Method to add the node pointed to by \p thePtr to the
//! Subdomain. This node is an external node to the subdomain and is
//! returned in calls to getExternalNodes() and {\em
//! getNodes()}. A DummyNode is created and added to the external nodes
//! storage object. 
//! Invokes setDomain(this)} on the DummyNode and {\em domainChanged() on
//! itself.
bool XC::Subdomain::addExternalNode(Node *thePtr)
  {
#ifdef _G3DEBUG
  // check to see it has not alredy been added

    int nodTag = thePtr->getTag();
    TaggedObject *other = externalNodes->getComponentPtr(nodTag);
    if(other != nullptr)
      return false;

    other = internalNodes->getComponentPtr(nodTag);
    if(other != nullptr)
      return false;

#endif
      // create a dummy XC::Node & try adding it to the external nodes
      Node *newDummy = new Node(*thePtr, false);
      if(newDummy == nullptr)
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

//! @brief Remove a node from the subdomain.
//!
//! To remove a Node whose tag is given by \p tag from the
//! Subdomain. Returns \f$0\f$ if the Node is not in the Subdoamin, otherwise
//! it removes the pointer to the node from the storage object, invokes
//! domainChange() on itself, and returns the pointer to the
//! Node. If the Node is external, the destructor is called on the
//! DummyNode. Invokes domainChanged() on itself. 
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

//! @brief Return an iter to all nodes that have been added to the subdomain.
XC::NodeIter &XC::Subdomain::getNodes(void)
  {
    theNodIter->reset();
    return *theNodIter;
  }

XC::NodePtrsWithIDs &XC::Subdomain::getNodePtrs(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; should not be called.\n";
    static NodePtrsWithIDs retval(this,1);
    return retval;
  }

const XC::NodePtrsWithIDs &XC::Subdomain::getNodePtrs(void) const
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; should not be called.\n";
    static NodePtrsWithIDs retval(const_cast<Subdomain *>(this),1);
    return retval;
  }

//! @brief Return a pointer to the node identified by the argument.
//!
//! To return a pointer to the node whose tag is given by \p tag from
//! the Subdomain. Returns \f$0\f$ if the Node is not in the Subdoamin,
//! otherwise returns a pointer to the Node, if external a pointer to the
//! DummyNode is returned. 
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
    return nullptr;
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
    return nullptr;
  }

bool XC::Subdomain::hasNode(int tag)
  {
    if(this->getNode(tag) != nullptr)
      return true;
    else
      return false;
  }

bool XC::Subdomain::hasElement(int tag)
  {
    if(this->getElement(tag) != nullptr)
      return true;
    else
      return false;
  }


//! @brief Returns the number of external and internal Nodes.
int XC::Subdomain::getNumNodes(void) const
  { return internalNodes->getNumComponents() + externalNodes->getNumComponents(); }

//! @brief invokes the base Domain classes commit() method. It then goes through
//! all the Nodes in the Subdomain, invoking commitState() on the Nodes.
int XC::Subdomain::commit(void)
  {
    Domain::commit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != nullptr)
      nodePtr->commitState();
    return 0;
  }

int XC::Subdomain::revertToLastCommit(void)
  {
    Domain::revertToLastCommit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != nullptr)
        nodePtr->revertToLastCommit();
    return 0;
  }

int XC::Subdomain::revertToStart(void)
  {
    Domain::revertToLastCommit();

    NodeIter &theNodes = this->getNodes();
    Node *nodePtr;
    while ((nodePtr = theNodes()) != nullptr)
      nodePtr->revertToStart();
    return 0;
  }

int XC::Subdomain::update(void)
  { return Domain::update(); }

int XC::Subdomain::update(double newTime, double dT)
  { return Domain::update(newTime, dT); }

//! @brief Print stuff.
void XC::Subdomain::Print(std::ostream &os, int flag)
  {
    os << "Current Subdomain Information for Subdomain: ";
    os << this->getTag() << ".\n";

    os << "\nINTERNAL NODE DATA: NumNodes: ";
    os << internalNodes->getNumComponents() << "\n";
    internalNodes->Print(os);

    os << "\nEXTERNAL NODE DATA: NumNodes: ";
    os << externalNodes->getNumComponents() << "\n";
    externalNodes->Print(os);

    this->Domain::Print(os);
    os << "\nEnd Subdomain Information\n";
  }



//! @brief Return an iterator to the internal nodes of the subdomain, nodes
//! that are added using the addNode() command.
XC::NodeIter &XC::Subdomain::getInternalNodeIter(void)
  {
    internalNodeIter->reset();
    return *internalNodeIter;
  }


//! @brief Return an itertor to the external nodes of the subdomain, nodes
//! that have been added using the {\em addExternalNode(Node *)} method.
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

//! @brief Sets the corresponding DomainDecompositionAnalysis object to be {\em
//! theAnalysis}.
void XC::Subdomain::setDomainDecompAnalysis(DomainDecompositionAnalysis &theNewAnalysis)
  {
    theAnalysis= &theNewAnalysis;
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


//! @brief Returns the number of external nodes that have been successfully
//! added to the subdomain as external nodes and have yet to be removed from the
//! subdomain. 
int XC::Subdomain::getNumExternalNodes(void) const
  { return externalNodes->getNumComponents(); }

//! @brief Returns an ID containing the tags of all nodes added to the subdomain
//! as external nodes and have yet to be removed from the subdomain.
const XC::ID &XC::Subdomain::getExternalNodes(void) const
  {
    // first we check that extNodes exists and is of correct size
    int numExt = externalNodes->getNumComponents();
    if(!extNodes)
      {
        extNodes= new ID(numExt);
        if(extNodes == 0 || extNodes->Size() != numExt)
          {
            std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		      << "; ran out of memory for size "
		      << numExt << std::endl;
            exit(-1);
          }
      }

    if(extNodes->Size() != numExt)
      {
        delete extNodes;
        extNodes = new ID(numExt);
        if(extNodes == 0 || extNodes->Size() != numExt)
          {
            std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		      << "; ran out of memory for size: "
		      << numExt << std::endl;
            exit(-1);
          }
      }

    // we now set the values of extNodes to be the node tags of the
    // external nodes

    NodeIter &theExtNodes = this->getExternalNodeIter();
    Node *nodPtr;
    int cnt = 0;

    while ((nodPtr = theExtNodes()) != nullptr)
      (*extNodes)(cnt++) = nodPtr->getTag();

    // done
    ID &res = *extNodes;
    return res;
  }



//! @brief Returns the num of external dof associated with the subdomain.
//!
//! Returns the num of external dof associated with the subdomain, the number
//! returned is the result of invoking getNumExternalEqn() on
//! the DomainDecompAnalysis object assocaited with the Subdomain. If
//! no Analysis yet associated with the Subdomain \f$0\f$ is returned.
int XC::Subdomain::getNumDOF(void) const
  {
    if(theAnalysis)
      return theAnalysis->getNumExternalEqn();
    else
      {
        //   std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	//             << "; no XC::StaticAnalysis has been set.\n";
        return 0;
      }
  }

//! @brief Invokes commit() on itself.
int XC::Subdomain::commitState(void)
  { return this->commit(); }

const XC::Matrix &XC::Subdomain::getTangentStiff(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; DOES NOT DO ANYTHING";
    return badResult;
  }

//! @brief For this class does nothing but print an error message. Subtypes may
//! provide a condensed stiffness matrix, \f$T^tKT\f$ corresponding to
//! external nodes. Returns a zero matrix of dimensions (1x1).
const XC::Matrix &XC::Subdomain::getInitialStiff(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; DOES NOT DO ANYTHING."
              << " Must be overloaded in derived classes.";
    return badResult;
  }

//! For this class does nothing but print an error message. Subtypes may
//! provide a condensed damping matrix, \f$T^tDT\f$ or a damping matrix
//! corresponding to some comination of the condensed stifffness and mass
//! matrices. Returns a zero matrix of dimensions (1x1).
const XC::Matrix &XC::Subdomain::getDamp(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; DOES NOT DO ANYTHING"
              << " Must be overloaded in derived classes.";
    return badResult;
  }

//! For this class does nothing but print an error message. Subtypes may
//! provide a condensed mass matrix, \f$T^tMT\f$ or a mass matrix with zero
//! diag elements. Returns a zero matrix of dimensions (1x1).
const XC::Matrix &XC::Subdomain::getMass(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; DOES NOT DO ANYTHING"
              << " Must be overloaded in derived classes.";
    return badResult;
  }




void XC::Subdomain::zeroLoad(void)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; should not be called.\n";
  }


int XC::Subdomain::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; should not be called.\n";
    return 0;
  }

int XC::Subdomain::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }


//! @brief Return the Vector obtained from invoking getCondensedRHS() on
//! the DomainDecompositionAnalysis object.
const XC::Vector &XC::Subdomain::getResistingForce(void) const
  {
    if(!theAnalysis)
      {
        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; no StaticCondensationAnalysis has been set.\n";
        exit(-1);
      }

    if(!mapBuilt)
      this->buildMap();

    ID &theMap = *map;
    const Vector &anaResidual = theAnalysis->getResidual();
    int numDOF = this->getNumDOF();
    for(int i=0; i<numDOF; i++)
      (*mappedVect)(i) = anaResidual(theMap(i));
    //std::cerr << Domain::getClassName() << "::" << __FUNCTION__
    //          << ": " << *mappedVect;
    return *mappedVect;
  }


const XC::Vector &XC::Subdomain::getResistingForceIncInertia(void) const
  {
    std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	      << "; should not be called.\n";

    return this->getResistingForce();
  }



//! Return \p true.
//! 
//! Return \p true. Subtypes can change this. The result of changing
//! this will be that the corresponding FE\_Elements will ask for the
//! stiffness, mass and damping matrices to form the tangent and will ask
//! for the residual to form the residual for the system of equations.
bool XC::Subdomain::isSubdomain(void)
  { return true; }


int XC::Subdomain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { return Domain::setRayleighDampingFactors(rF); }

//! The method first starts a Timer object running. formTang(), 
//! is then invoked on the DomainDecompositionAnalysis object. The
//! Timer is then stopped and the real time is added to the \p realCost. 
//! Returns the result of invoking \p formTang.
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
        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; no StaticCondensationAnalysis has been set.\n";
        return 0;
      }
  }

//! The method first starts a Timer object running. formResidual(), 
//! is then invoked on the DomainDecompositionAnalysis object. The
//! Timer is then stopped and the real time is added to the \p realCost. 
//! Returns the result of invoking \p formResidual.
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
        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; no StaticCondensationAnalysis has been set.\n";
        return 0;
      }
  }

//! @brief Return the Matrix obtained from invoking getTangent() on
//! the DomainDecompositionAnalysis object.
const XC::Matrix &XC::Subdomain::getTang(void)
  {
    if(!theAnalysis)
      {
        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; no StaticCondensationAnalysis has been set.\n";
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


//! @brief Set the corresponding {\em FE\_Element} to be that poited to by {\em
//! theFEelePtr}.
void XC::Subdomain::setFE_ElementPtr(FE_Element *theFE_Ele)
  { theFEele = theFE_Ele; }


//! @brief Return a pointer to the last FE\_Element set using {\em
//! setFE\_ElementPtr}. If no FE\_Element has been set nullptr
//! is returned.
XC::FE_Element *XC::Subdomain::getFE_ElementPtr(void)
  { return theFEele; }


//! @brief Return the Vector obtained by calling getLastSysResponse() on
//! the associated FE\_Element.
const XC::Vector &XC::Subdomain::getLastExternalSysResponse(void)
  {
    if(!theFEele)
      {
        std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		  << "; FATAL ERROR: "
		  << "; no FE_Element *exists for a subdomain.\n"
		  << " This is the responsibilty of the"
	          << " FE_ELement constructor.\n";
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


//! @brief Set the nodal responses for the nodes in the subdomain.
//! 
//! To set the nodal responses for the nodes in the subdomain. Returns the
//! result of invoking computeInternalResponse() on the DomainDecomposition
//! analysis object associated with the subdomain. 
int XC::Subdomain::computeNodalResponse(void)
  {
    int res =0;
    if(theAnalysis)
      res= theAnalysis->computeInternalResponse();
    else
      std::cerr << Domain::getClassName() << "::" << __FUNCTION__
	        << "; no analysis has been set.\n";
    return res;
  }


int XC::Subdomain::newStep(double dT)
  {
    if(theAnalysis)
      return theAnalysis->newStep(dT);
    return 0;
  }


bool XC::Subdomain::doesIndependentAnalysis(void)
  {
    if(theAnalysis != nullptr)
      return theAnalysis->doesIndependentAnalysis();
    else
      return true;
  }

//! @brief Send itself.
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
      std::cerr << Domain::getClassName() << "::" << __FUNCTION__
		<< "; no analysis set.\n";
    return -1;
  }

//! @brief Receive itself.
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

//! Return the current value of \p realCost.
//!
//! Returns the current value of \p realCost, resetting the value of \p
//! realCost to be 0.0. The value of \p realCost is added to when
//! computeTang() and \p computeREsidual are invoked on the Subdomain.
double XC::Subdomain::getCost(void)
  {
    double lastRealCost= realCost;
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
        if(map == nullptr)
          map = new ID(numDOF);
        if(map->Size() != numDOF)
          {
            delete map;
            map = new ID(numDOF);
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

        if(mappedVect == nullptr)
          mappedVect = new Vector(numDOF);
        if(mappedVect->Size() != numDOF)
          {
            delete mappedVect;
            mappedVect = new Vector(numDOF);
          }
        if(mappedMatrix == nullptr)
          mappedMatrix = new Matrix(numDOF,numDOF);
        if(mappedMatrix->noRows() != numDOF)
          {
            delete mappedMatrix;
            mappedMatrix = new Matrix(numDOF,numDOF);
          }
      }
    return 0;
  }


XC::DomainDecompositionAnalysis *XC::Subdomain::getDDAnalysis(void)
  { return theAnalysis; }
