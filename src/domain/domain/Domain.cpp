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

// $Revision: 1.33 $
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/Domain.cpp,v $

// Written: fmk
//
// Purpose: This file contains the class definition for Domain
// Domain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints
// and MFreedom_Constraints. These objects are all added to the Domain by a
// ModelBuilder.
//
// What: "@(#) Domain.C, revA"


#include "Domain.h"
#include "../../FEProblem.h"
#include <cstdlib>
#include "utility/recorder/Recorder.h"

#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/node/NodeIter.h"
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/LoadCombination.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

#include <domain/domain/single/SingleDomEleIter.h>
#include <domain/domain/single/SingleDomNodIter.h>
#include <domain/domain/single/SingleDomSFreedom_Iter.h>
#include <domain/domain/single/SingleDomMFreedom_Iter.h>
#include <domain/domain/single/SingleDomMRMFreedom_Iter.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/domain/single/SingleDomAllSFreedom_Iter.h>

#include <solution/graph/graph/Vertex.h>
#include "solution/graph/graph/Graph.h"
#include "domain/mesh/region/MeshRegion.h"
#include "domain/mesh/region/DqMeshRegion.h"
#include <solution/analysis/analysis/Analysis.h>
#include "utility/database/FE_Datastore.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"

#include "preprocessor/Preprocessor.h"


#include "utility/actor/actor/ArrayCommMetaData.h"


void XC::Domain::free_mem(void)
  {
    //delete the objects in the domain
    clearAll();
  }

//! @brief Constructor.
//!
//! Constructs an empty domain. The storage for the DomainComponents uses
//! ArrayOfTaggedObjects objects for each type of object to be stored.
//!
//! @param owr: object that contains this one.
XC::Domain::Domain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(),CallbackCommit(""), dbTag(0),
   currentGeoTag(0), hasDomainChangedFlag(false), commitTag(0),
   mesh(this), constraints(this), theRegions(nullptr),
   nmbCombActual(""), lastChannel(0), lastGeoSendTag(-1) {}

//! @brief Constructor.
//!
//! Constructs an empty Domain. The initial sizes specified
//! for these objects are no more needed (so ignored)
//!
//! @param owr: object that contains this one.
//! @param numNodes: number of nodes.
//! @param numElements: number of elements.
//! @param numSPs: number of single freedom constraints.
//! @param numMPs: number of multi-freedom constraintes.
//! @param numLoadPatterns: number of load patterns.
//! @param numNodeLockers: number of node lockers.
XC::Domain::Domain(EntCmd *owr,int numNodes, int numElements, int numSPs, int numMPs, int numLoadPatterns,int numNodeLockers,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(), CallbackCommit(""), dbTag(0),
   currentGeoTag(0), hasDomainChangedFlag(false), commitTag(0), mesh(this),
   constraints(this), theRegions(nullptr), nmbCombActual(""), lastChannel(0),
   lastGeoSendTag(-1) {}

//! @brief Removes all components from domain (nodes, elements, loads &
//! constraints).
//! 
//! To remove all the components from the model and invoke the destructor
//! on these objects. First clearAll() is invoked on all the
//! LoadPattern objects. Then the domain object invokes {\em
//! clearAll()} on its container objects. In addition the destructor for
//! each Recorder object that has been added to the domain is invoked. In
//! addition the current time and load factor are set to \f$0\f$, and the box
//! bounding the domain is set to the box enclosing the origin. 
//!
//! GENERAL NOTE ON REMOVAL OF COMPONENTS:
//! downward casts (while bad) are o.k. as only the type
//! of components can be added to the storage objects, e.g.
//! only elements can be added to theElements therefore
//! casting a DomainComponent from theElements to an Element is o.k.
void XC::Domain::clearAll(void)
  {
    constraints.clearAll();
    
    // clean out the containers
    mesh.clearAll();

    if(theRegions)
      {
        theRegions->clearAll();
        delete theRegions;
        theRegions= nullptr;
      }
    nmbCombActual= "";

    // set the time back to 0.0
    timeTracker.Zero();

    // rest the flag to be as initial
    hasDomainChangedFlag = false;

    currentGeoTag = 0;
    lastGeoSendTag = -1;
    lastChannel = 0;
  }

//! @brief Destructor.
//!
//! destructor, this calls delete on all components of the model,
//! i.e. calls delete on all that is added to the model.
//! WARNING: if 3rd constructor, TaggedObjectStorage objects passed
//! must have been created with new and nowhere else must the
//! destructor be called.
XC::Domain::~Domain(void)
  { free_mem(); }

//! @brief Prepares the domain to solve for a new load pattern.
void XC::Domain::resetLoadCase(void)
  {
    nmbCombActual= "";
    revertToStart();
    setCommitTag(0);
    timeTracker.Zero();
  }

//! @brief Assigns Stress Reduction Factor for element deactivation.
void XC::Domain::setDeadSRF(const double &d)
  { Element::setDeadSRF(d); }


//! @brief Adds to the domain the element being passed as parameter.
bool XC::Domain::addElement(Element *element)
  { return mesh.addElement(element); }

//! @brief Adds to the domain the node being passed as parameter.
bool XC::Domain::addNode(Node * node)
  { return mesh.addNode(node); }

//! @brief Adds a single freedom constraint to the domain.
//!
//! To add the single point constraint pointed to by spConstraint to the
//! domain. 
//! In addition the container always checks to ensure that no other
//! constraint with a similar tag exists in the domain. The domain then invokes
//! setDomain(this) on the constraint and domainChange() on itself. 
bool XC::Domain::addSFreedom_Constraint(SFreedom_Constraint *spConstraint)
  {
    bool result= constraints.addSFreedom_Constraint(spConstraint);
    if(result)
      {
        spConstraint->setDomain(this);
        this->domainChange();
      }
    return true;
  }

//! @brief Adds to the domain a multi-freedom constraint.
//! 
//! To add the multiple point constraint pointed to by theMPptr, to the
//! domain.  
//! In addition the domain always checks to ensure that no other
//! MP\_Constraint with a similar tag exists in the domain. If the checks are
//! successful, the constraint is added to domain by the domain invoking {\em
//! addComponent(theMPptr)} on the container for the MP\_Constraints. The
//! domain then invokes {\em setDomain(this)} on the 
//! constraint and domainChange() on itself. The call returns {\em
//! true} if the constraint was added, otherwise a warning is
//! raised and \p false is returned.
bool XC::Domain::addMFreedom_Constraint(MFreedom_Constraint *mpConstraint)
  {
    bool result= constraints.addMFreedom_Constraint(mpConstraint);
    if(result)
      {
        mpConstraint->setDomain(this);
        this->domainChange();
      }
    return result;
  }

//! @brief Adds to the domain a multi-freedom multi-retained node constraint.
bool XC::Domain::addMRMFreedom_Constraint(MRMFreedom_Constraint *mrmpConstraint)
  {
    bool result= constraints.addMRMFreedom_Constraint(mrmpConstraint);
    if(result)
      {
        mrmpConstraint->setDomain(this);
        this->domainChange();
      }
    return result;
  }

//! @brief Adds to the domain una constraint monopunto.
bool XC::Domain::addSFreedom_Constraint(SFreedom_Constraint *spConstraint, int pattern)
  {
    bool result= constraints.addSFreedom_Constraint(spConstraint,pattern);
    if(!result)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; " << pattern
                  << " pattern could not add the SFreedom_Constraint\n";
        return false;
      }

    spConstraint->setDomain(this);
    this->domainChange();
    return true;
  }

//! @brief Appends a nodal load to the pattern being passed as parameter.
//! 
//! To add the nodal load \p theld to the LoadPattern in the domain
//! whose tag is given by \p loadPatternTag.
//! If {\em \_DEBUG} is defines, checks to see that corresponding node
//! exists in the domain. A pointer to the LoadPattern is obtained from
//! the LoadPattern container and the load is added to LoadPattern by
//! invoking {\em addNodalLoad(theLd)} on the LoadPattern object. The
//! domain is responsible for invoking {\em setDomain(this)} on the
//! load. The call returns \p true if the load was added, a warning
//! is raised and and {\em false} is returned. 
//!
//! @param load: load over node.
//! @param pattern: load pattern identifier.
bool XC::Domain::addNodalLoad(NodalLoad *load, int pattern)
  {
    bool result= constraints.addNodalLoad(load,pattern);
    if(result)
      {
        load->setDomain(this); // done in LoadPattern::addNodalLoad()
        this->domainChange();
      }
    return result;
  }

//! @brief Adds a load over element to the pattern.
//! 
//! To add the elemental load \p theld to the LoadPattern in the domain
//! whose tag is given by \p loadPatternTag.
//! If {\em \_DEBUG} is defines, checks to see that corresponding element
//! exists in the domain. A pointer to the LoadPattern is obtained from
//! the LoadPattern container and the load is added to LoadPattern by
//! invoking {\em addElementalLoad(theLd)} on the LoadPattern object. The
//! domain is responsible for invoking {\em setDomain(this)} on the
//! load. The call returns \p true if the load was added, otherwise a warning
//! is raised and and {\em false} is returned.
//!
//! @param load: load over element.
//! @param pattern: load pattern identifier.
bool XC::Domain::addElementalLoad(ElementalLoad *load, int pattern)
  {
    bool result= constraints.addElementalLoad(load,pattern);
    if(result == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no pattern with tag "
		  << pattern
		  << " in the model, not adding the ele load"
		  << *load << std::endl;
        return false;
      }

    // load->setDomain(this); // done in LoadPattern::addElementalLoad()
    this->domainChange();
    return result;
  }

//! @brief Remove the element identified by the argument.
bool XC::Domain::removeElement(int tag)
  { return mesh.removeElement(tag); }

//! @brief Remove the node identified by the argument.
bool XC::Domain::removeNode(int tag)
  { return mesh.removeNode(tag); }

//! @brief Remove the single freedom constraint from the load pattern
//! identified by the argument.
//!
//! To remove the SFreedom\_Constraint whose tag is given by \p tag from the
//! domain. The domain achieves this by invoking {\em
//! removeComponent(tag)} on the container for the single point
//! constraints. Returns \f$0\f$ if the constraint was not in the domain,
//! otherwise the domain invokes {\em setDomain(nullptr)} on the constraint and
//! domainChange() on itself before a pointer to the constraint is
//! returned. Note this will only remove SFreedom\_Constraints which have been
//! added to the domain and not directly to LoadPatterns.
//!
//! @param theNode: node identifier.
//! @param theDOF: degree of freedom identifier.
//! @param loadPatternTag: load pattern identifier (if -1 then remove from
//! domain).
bool XC::Domain::removeSFreedom_Constraint(int theNode, int theDOF, int loadPatternTag)
  {
    bool retval= constraints.removeSFreedom_Constraint(theNode,theDOF,loadPatternTag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Removes from domain the single freedom constraint identified by the
//! argument.
//!
//! @param tag: identifier of the single freedom constraint.
bool XC::Domain::removeSFreedom_Constraint(int tag)
  {
    bool retval= constraints.removeSFreedom_Constraint(tag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Removes from domain the multi-freedom constraint identified by
//! the argument.
//!
//! To remove the MP\_Constraint whose tag is given by \p tag from the
//! domain. The domain achieves this by invoking {\em
//! removeComponent(tag)} on the container for the multi point
//! constraints. Returns \f$0\f$ if the constraint was not in the domain,
//! otherwise the domain invokes {\em setDomain(nullptr)} on the constraint and
//! domainChange() on itself before a pointer to the constraint is
//! returned.   
//!
//! @param tag: identifier of the constraint.
bool XC::Domain::removeMFreedom_Constraint(int tag)
  {
    bool result = constraints.removeMFreedom_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }


//! @brief Removes from domain the multi-freedom multi-retained node constraint
//! identified by the argument.
//!
//! @param tag: identifier of the constraint.
bool XC::Domain::removeMRMFreedom_Constraint(int tag)
  {
    bool result = constraints.removeMRMFreedom_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }

//! @brief Appends the load pattern to the domain.
//!
//! To add the LoadPattern \p thePattern to the domain.
//! The load is added to domain by the domain invoking {\em
//! addComponent(theLd)} on the container for the LoadPatterns. The domain
//! is responsible for invoking {\em setDomain(this)} on the load. The
//! call returns \p true if the load was added, otherwise a warning is
//! raised and \p false is returned.
bool XC::Domain::addLoadPattern(LoadPattern *load)
  {
    bool result= constraints.addLoadPattern(load);
    if(result)
      {
        load->setDomain(this);
        domainChange();
      }
    else
      {
        if(verbosity>3)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; can't add load pattern identified by: "
                    << load->getTag() << "'\n";
      }
    return result;
  }

//! @brief Appends the node locker object to the domain. 
bool XC::Domain::addNodeLocker(NodeLocker *nl)
  {
    bool result= constraints.addNodeLocker(nl);
    if(result)
      {
        nl->setDomain(this);
        domainChange();
      }
    return result;
  }

//! @brief Adds to the domain the load combination being passed as parameter.
bool XC::Domain::addLoadCombination(LoadCombination *comb)
  {      
    bool retval= false;
    if(comb)
      {
        if(nmbCombActual!= "")
	  std::clog << getClassName() << "::" << __FUNCTION__
	            << "; warning! "
                    << "adding combination: " << comb->getNombre()
                    << " without removing: " << nmbCombActual
                    << ".\n";
        nmbCombActual= comb->getNombre();
        if(comb->getDomain()!=this)
          comb->setDomain(this);
        retval= comb->addToDomain();
      }
    return retval;
  }

//! @brief Remove from domain el load pattern identified by the argument.
//!
//! To remove the LoadPattern whose tag is given by \p tag from the
//! domain. The domain achieves this by invoking {\em
//! removeComponent(tag, numSPs)} on the container for the constraints. 
//! If the LoadPattern exists, the domain then iterates through the loads
//! and constraints of the LoadPattern invoking {\em setDomain(nullptr)} on
//! these objects. Returns
//! \f$0\f$ if the load was not in the domain, otherwise returns a pointer to
//! the load that was removed. Invokes {\em setDomain(0)} on the load case
//! before it is returned. 
//!
//! @param tag: identifier of the load pattern.
bool XC::Domain::removeLoadPattern(int tag)
  {
    int numSPs= 0;
    bool result= constraints.removeLoadPattern(tag,numSPs);
    if(result)
      {
        // mark the domain has having changed if numSPs > 0
        // as the constraint handlers have to be redone
        if(numSPs>0)
          domainChange();
      }
    // finally return the load pattern
    return result;
  }

//! @brief Remove from domain el 
bool XC::Domain::removeNodeLocker(int tag)
  {
    int numSPs= 0;
    bool result= constraints.removeNodeLocker(tag,numSPs);
    if(result)
      {
        // mark the domain has having changed if numSPs > 0
        // as the constraint handlers have to be redone
        if(numSPs>0)
          domainChange();
      }
    // finally return the node locker
    return result;
  }

//! @brief Remove from domain el load pattern being passed as parameter.
bool XC::Domain::removeLoadPattern(LoadPattern *lp)
  {
    bool retval= false;
    if(lp)
      retval= removeLoadPattern(lp->getTag());
    return retval;
  }

//! @brief Remove from domain el load pattern being passed as parameter.
bool XC::Domain::removeNodeLocker(NodeLocker *nl)
  {
    bool retval= false;
    if(nl)
      retval= removeNodeLocker(nl->getTag());
    return retval;
  }

//! @brief Returns the name of the current load combination.
const std::string &XC::Domain::getCurrentCombinationName(void) const
  { return nmbCombActual; }

//! @brief Removes from the domain the load combination
//! being passed as parameter.
void XC::Domain::removeLoadCombination(LoadCombination *comb)
  {
    if(comb)
      {
        if((nmbCombActual!= comb->getNombre())&& (!nmbCombActual.empty()))
	  std::clog << getClassName() << "::" << __FUNCTION__
		    << "; WARNING - "
                    << "removing load combination: " << comb->getNombre()
                    << " without removing: " << nmbCombActual
                    << ".\n";
        nmbCombActual= "";
        if(comb->getDomain()!=this)
          comb->setDomain(this);
        comb->removeFromDomain();
      }
  }

//! @brief Remove from domain todos los load patterns.
void XC::Domain::removeLPs(void)
  {
    int numSPs= constraints.removeLPs();
    // mark the domain has having changed if numSPs > 0
    // as the constraint handlers have to be redone
    if(numSPs>0)
      domainChange();
  }

//! @brief Remove all node lockers from domain.
void XC::Domain::removeNLs(void)
  {
    int numSPs= constraints.removeNLs();
    // mark the domain has having changed if numSPs > 0
    // as the constraint handlers have to be redone
    if(numSPs>0)
      domainChange();
  }

//! @brief Removes from domain the nodal load being passed as parameter.
//! @param nodalLoadTag: Nodal load identifier.
//! @param loadPattern: Load pattern identifier.
bool XC::Domain::removeNodalLoad(int nodalLoadTag, int loadPattern)
  { return constraints.removeNodalLoad(nodalLoadTag,loadPattern); }


//! @brief Removes from domain the elemental load being passed as parameter.
//! @param elemLoadTag: Identifier of the load over elements to remove.
//! @param loadPattern: Load pattern identifier.
bool XC::Domain::removeElementalLoad(int elemLoadTag, int loadPattern)
  { return constraints.removeElementalLoad(elemLoadTag,loadPattern); }

//! @brief Removes from domain the single freedom constraint being passed as parameter.
///! @param singleFreedomTag: Single freedom identifier.
//! @param loadPattern: Load pattern identifier.
bool XC::Domain::removeSFreedom_Constraint(int singleFreedomTag, int loadPattern)
  {
    bool removed= constraints.removeSFreedom_Constraint(singleFreedomTag,loadPattern);
    if(removed)
      this->domainChange();
    return removed;
  }

//! @brief Clears the pointers to DOF groups.
void XC::Domain::clearDOF_GroupPtr(void)
  { mesh.clearDOF_GroupPtr(); }

//! @brief Returns an iterator to the element container.
XC::ElementIter &XC::Domain::getElements()
  { return mesh.getElements(); }

//! @brief Returns an iterator to the node container.
XC::NodeIter &XC::Domain::getNodes()
  { return mesh.getNodes(); }

//! @brief Returns a reference to the domain mesh.
const XC::Mesh &XC::Domain::getMesh(void) const
  { return mesh; }

//! @brief Returns a reference to the domain mesh.
XC::Mesh &XC::Domain::getMesh(void)
  { return mesh; }

//! @brief Returns domain constraints.
const XC::ConstrContainer &XC::Domain::getConstraints(void) const
  { return constraints; }

//! @brief Returns domain constraints.
XC::ConstrContainer &XC::Domain::getConstraints(void)
  { return constraints; }


/* GENERAL NOTE ON RETRIEVAL OF COMPONENT PTRs:
**   downward casts (while bad) are o.k. as only the type
**   of components can be added to the storage objects, e.g.
**   only elements can be added to theElements
*/

//! @brief Returns true if the element identified by the
//! tag being passed as parameter already exists en el domain.
bool XC::Domain::existElement(int tag)
 { return mesh.existElement(tag); }

//! @brief Return a pointer to the element identified
//! by the argument.
//!
//! @param tag: element identifier.
XC::Element *XC::Domain::getElement(int tag)
  { return mesh.getElement(tag); }

//! @brief Returns a const pointer to the element identified
//! by the argument.
//!
//! @param tag: element identifier.
const XC::Element *XC::Domain::getElement(int tag) const
  { return mesh.getElement(tag); }


//! @brief Return true if the mesh has a node with this tag.
//!
//! @param tag: node identifier.
bool XC::Domain::existNode(int tag)
 { return mesh.existNode(tag); }


//! @brief Return a pointer to the node identified by the argument.
//!
//! @param tag: node identifier.
XC::Node *XC::Domain::getNode(int tag)
  { return mesh.getNode(tag); }

//! @brief Return a pointer to the node identified by the argument.
//!
//! @param tag: node identifier.
const XC::Node *XC::Domain::getNode(int tag) const
  { return mesh.getNode(tag); }

int XC::Domain::getCommitTag(void) const
  { return commitTag; }

//! @brief Returns the number of elements.
int XC::Domain::getNumElements(void) const
  { return mesh.getNumElements(); }

//! @brief Returns the number of nodes.
int XC::Domain::getNumNodes(void) const
  { return mesh.getNumNodes(); }

//! @brief Returns the boundary of the finite element model.
//!
//! To return the bounding rectangle for the mesh. The information is
//! contained in a Vector of size 6 containing in the following order
//! \{xmin, ymin, zmin, xmax, ymax, zmax\}. This information is built up
//! as nodes are added to the domain, initially all are set to \f$0\f$ in the
//! constructor. 
const XC::Vector &XC::Domain::getPhysicalBounds(void)
  { return mesh.getPhysicalBounds(); }

//! @brief Builds (if necessary) the domain elements graph and
//! returns a reference to it.
//! 
//! Returns the current element graph (the connectivity of the elements
//! in the mesh). If the \p eleChangeFlag has been set
//! to \p true the method will invoke {\em buildEleGraph(theEleGraph)}
//! on itself before returning the graph. The vertices in the element
//! graph are to be labeled \f$0\f$ through \f$numEle-1\f$. The vertices
//! references contain the elemental tags.  
XC::Graph &XC::Domain::getElementGraph(void)
  { return mesh.getElementGraph(); }

//! @brief Builds (if necessary) the domain node graph and
//! returns a reference to it.
//!
//! Returns the current node graph (the connectivity of the nodes in
//! the mesh). If the \p nodeChangeFlag has been set to \p true the
//! will invoke {\em buildNodeGraph(theNodeGraph)} on itself before
//! returning the graph. The vertices in the node graph are to be labeled
//! \f$0\f$ through \f$numNode-1\f$. The Vertices references contain the nodal
//! tags.
XC::Graph &XC::Domain::getNodeGraph(void)
  { return mesh.getNodeGraph(); }

//! @brief Set the committed tag to \p newTag. 
void XC::Domain::setCommitTag(int newTag)
  { commitTag = newTag; }

//! @brief Set the current time to \p newTime. 
void XC::Domain::setCurrentTime(double newTime)
  { timeTracker.setCurrentTime(newTime); }

//! @brief Set the committed time to \p newTime. 
void XC::Domain::setCommittedTime(double newTime)
  { timeTracker.setCommittedTime(newTime); }

void XC::Domain::setTime(double newTime)
  {
    setCurrentTime(newTime);
    setCommittedTime(newTime);
  }

//! @brief Apply the loads for the given time \p pseudoTime.
//!
//! To apply the loads for the given time \p pseudoTime. The domain
//! first clears the current load at all nodes and elements, done by
//! invoking zeroUnbalancedLoad()} on each node and {\em zeroLoad()
//! on each element. The domain then invokes {\em applyLoad(pseudoTime)}
//! on all LoadPatterns which have been added to the domain. The domain
//! will then invoke {\em applyConstraint(pseudoTime)} on all the
//! constraints in the single and multi point constraint
//! containers. Finally the domain sets its current time to be {\em
//! pseudoTime}.  
void XC::Domain::applyLoad(double timeStep)
  {
    // set the current pseudo time in the domain to be newTime
    setCurrentTime(timeStep);

    // first loop over nodes and elements getting them to first zero their loads
    mesh.zeroLoads();

    constraints.applyLoad(timeStep);
  }

//! @brief Set all the loads as constant.
//!
//! To set the loads in the LoadPatterns to be constant. The domain
//! achieves this by invoking setLoadConstant() on all the
//! LoadPatterns which have been added to the domain. Note that
//! LoadPatterns added after this method has been invoked will not be
//! constant until this method is invoked again. 
void XC::Domain::setLoadConstant(void)
  { constraints.setLoadConstant(); }

//! @brief Inicializa.
int XC::Domain::initialize(void)
  { return mesh.initialize(); }

//! @brief Set Rayleigh damping factors.
int XC::Domain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { return mesh.setRayleighDampingFactors(rF); }

//! @brief Commits domain state and triggers "record" method
//! for all defined recorders.
//!
//! To commit the state of the domain , that is to accept the current
//! state as being ion the solution path. The domain invokes {\em
//! commit()} on all nodes in the domain and then {\em 
//! commit()} on all elements of the domain. These are calls for the nodes
//! and elements to set there committed state as given by their current
//! state. The domain will then set its committed time variable to be
//! equal to the current time and lastly increments its commit tag by \f$1\f$.  
int XC::Domain::commit(void)
  {
    //
    // first invoke commit on all nodes and elements in the domain
    //
    mesh.commit();

    // set the new committed time in the domain
    setCommittedTime(timeTracker.getCurrentTime());

    // if(!CallbackCommit.empty())
    //   EjecutaBloque(CallbackCommit,"domain:callback_commit");

    ObjWithRecorders::record(commitTag,timeTracker.getCurrentTime()); //Llama al método record de todos los recorders.

    // update the commitTag
    commitTag++;
    return 0;
  }

//! @brief Returns the domain to its last commited state.
//!
//! To return the domain to the state it was in at the last commit. The
//! domain invokes revertToLastCommit() on all nodes and elements in
//! the domain. The domain sets its current loadFactor and time
//! stamp to be equal to the last committed values. The domain decrements
//! the current commitTag by \f$1\f$. Finally it invokes applyLoad()
//! on itself with the current time.
int XC::Domain::revertToLastCommit(void)
  {
    //
    // first invoke revertToLastCommit  on all nodes and elements in the domain
    //
    mesh.revertToLastCommit();

    // set the current time and load factor in the domain to last committed
    setCurrentTime(timeTracker.getCommittedTime());

    // apply load for the last committed time
    applyLoad(timeTracker.getCurrentTime());

    return update();
  }

//! @brief Returns the domain to its initial state and
//! triggers the "restart" method for all the recorders.
int XC::Domain::revertToStart(void)
  {
    //
    // first invoke revertToStart  on all nodes and
    // elements in the domain
    //
    mesh.revertToStart();

// ADDED BY TERJE //////////////////////////////////
    // invoke 'restart' on all recorders
    ObjWithRecorders::restart();
/////////////////////////////////////////////////////

    // set the current time and load factor in the domain to last committed
    timeTracker.Zero();

    // apply load for the last committed time
    applyLoad(timeTracker.getCurrentTime());

    return update();
  }

//! @brief Updates the state of the domain.
//! 
//! Called by the AnalysisModel to update the state of the
//! domain. Iterates over all the elements of the Domain and invokes {\em
//! update()}. 
int XC::Domain::update(void)
  {
    // set the global constants
    FEProblem::theActiveDomain= this;
    return mesh.update();
  }

//! @brief Actualiza el estado del domain.
int XC::Domain::update(double newTime, double dT)
  {
    applyLoad(newTime);
    update();
    return 0;
  }

int XC::Domain::newStep(double dT)
  { return 0; }


//! @brief Sets eigenvalues.
int XC::Domain::setEigenvalues(const Vector &theValues)
  {
    theEigenvalues= theValues;

    // now set the time at which eigen values
    // were determined to be current domain time
    timeTracker.setEigenvalueTime();
    return 0;
  }

//! @brief Returns the eigenvalue of the i-th mode.
const double &XC::Domain::getEigenvalue(int i) const
  { return theEigenvalues(i-1); }

//! @brief Return the angular frequency of the i-th mode.
double XC::Domain::getAngularFrequency(int i) const
  { return sqrt(getEigenvalue(i)); }

//! @brief Returns the period of the i-th mode.
double XC::Domain::getPeriodo(int i) const
  { return 2.0*M_PI/getAngularFrequency(i); }

//! @brief Return the frequency of the i-th mode.
double XC::Domain::getFrecuencia(int i) const
{ return 1./getPeriodo(i); }

//! @brief Returns the eigenvalues vector.
const XC::Vector &XC::Domain::getEigenvalues(void) const
  { return theEigenvalues; }

//! @brief Returns a vector with the computed angular frequencies (for each mode).
XC::Vector XC::Domain::getAngularFrequencies(void) const
  {
    Vector retval= getEigenvalues();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= sqrt(retval(i));
    return retval;
  }

//! @brief Returns a vector with the computed periods (for each mode).
XC::Vector XC::Domain::getPeriodos(void) const
  {
    Vector retval= getAngularFrequencies();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 2.0*M_PI/retval(i);
    return retval;
  }

//! @brief Returns a vector with the computed frequencies (for each mode).
XC::Vector XC::Domain::getFrecuencias(void) const
  {
    Vector retval= getPeriodos();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 1.0/retval(i);
    return retval;
  }

//! @brief Returns the number of computed eigenvalues.
int XC::Domain::getNumModes(void) const
  { return getEigenvalues().Size(); }

//! @brief Sets the values of the modal participation factors.
int XC::Domain::setModalParticipationFactors(const Vector &theValues)
  {
    modalParticipationFactors= theValues;
    return 0;
  }

//! @brief Returns the modal participation factor of the i-th mode.
const double &XC::Domain::getModalParticipationFactor(int i) const
  { return modalParticipationFactors(i-1); }

//! @brief Returns the modal participation factors.
const XC::Vector &XC::Domain::getModalParticipationFactors(void) const
  { return modalParticipationFactors; }

//! @brief Return the effective modal mass of the i-th mode.
const double XC::Domain::getEffectiveModalMass(int i) const
  { return mesh.getEffectiveModalMass(i); }

//! @brief Returns the effective modal masses for each mode.
XC::Vector XC::Domain::getEffectiveModalMasses(void) const
  {
    const int nm= getNumModes();
    return mesh.getEffectiveModalMasses(nm);
  }

//! @brief Return the total effective modal mass.
double XC::Domain::getTotalMass(void) const
  {
    double retval= 0;
    const Vector masas= getEffectiveModalMasses();
    const size_t sz= masas.Size();
    for(size_t i=0;i<sz;i++)
      retval+= masas(i);
    return retval;
  }

//! @brief Set the domain stamp to be \p newStamp. Domain stamp is the
//! integer returned by hasDomainChanged(). 
void XC::Domain::setDomainChangeStamp(int newStamp)
  { currentGeoTag= newStamp; }


//! @brief Sets a flag indicating that the integer returned in the next call to 
//! hasDomainChanged() must be incremented by \f$1\f$.
//! 
//! Sets a flag indicating that the integer returned in the next call to 
//! hasDomainChanged() must be incremented by \f$1\f$. This method is
//! invoked whenever a Node, Element or Constraint object is added to the
//! domain.  
void XC::Domain::domainChange(void)
  { hasDomainChangedFlag= true; }

//! @brief Returns true if the model has changed.
//!
//! To return an integer stamp indicating the state of the
//! domain. Initially \f$0\f$, this integer is incremented by \f$1\f$ if  {\em
//! domainChange()} has been invoked since the last invocation of the
//! method. If the domain has changed it marks the element and node graph
//! flags as not having been built.  
int XC::Domain::hasDomainChanged(void)
  {
    // if the flag indicating the domain has changed since the
    // last call to this method has changed, increment the integer
    // and reset the flag
    bool result = hasDomainChangedFlag;
    hasDomainChangedFlag = false;
    if(result)
      {
        currentGeoTag++;
        mesh.setGraphBuiltFlags(false);
      }
    // return the integer so user can determine if domain has changed
    // since their last call to this method
    return currentGeoTag;
  }

//! @brief Print stuff.
//!
//! To print the state of the domain. The domain invokes {\em Print(s,flag)} on
//! all it's container objects. 
void XC::Domain::Print(std::ostream &s, int flag)
  {

    s << "Current Domain Information\n";
    s << timeTracker << std::endl;

    mesh.Print(s, flag);
    constraints.Print(s,flag);
  }

//! @brief Insertion in an output stream.
std::ostream &XC::operator<<(std::ostream &s, XC::Domain &M)
  {
    M.Print(s);
    return s;
  }

//! @brief Adds a recorder to the model.
//!
//! To add a recorder object \p theRecorder to the domain. {\em
//! record(commitTag)} is invoked on each commit(). The pointers to
//! the recorders are stored in an array which is resized on each
//! invocation of this method.  
int XC::Domain::addRecorder(Recorder &theRecorder)
  {
    if(theRecorder.setDomain(*this) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; recorder could not be added.\n";
        return -1;
      }
    else
      ObjWithRecorders::addRecorder(theRecorder);
    return 0;
  }

//! @brief Adds a region.
int XC::Domain::addRegion(MeshRegion &theRegion)
  {
    if(!theRegions)
      theRegions= new DqMeshRegion();
    theRegions->add(theRegion);
    return 0;
  }

//! @brief Returns a pointer to the la región identified by the argument.
XC::MeshRegion *XC::Domain::getRegion(int tag)
  {
    MeshRegion *retval= nullptr;
    if(theRegions)
      retval= theRegions->getRegion(tag);
    return retval;
  }

//! @brief Builds the element graph.
//!
//! A method which will cause the domain to discard the current element
//! graph and build a new one based on the element connectivity. Returns
//! \f$0\f$ if successful otherwise \f$-1\f$ is returned along with an error
//! message. 
int XC::Domain::buildEleGraph(Graph &theEleGraph)
  { return mesh.buildEleGraph(theEleGraph); }

//! @brief Builds the node graph.
//!
//! A method which will cause the domain to discard the current node
//! graph and build a new one based on the node connectivity. Returns
//! \f$0\f$ if successful otherwise \f$-1\f$ is returned along with an error
//! message. 
int XC::Domain::buildNodeGraph(Graph &theNodeGraph)
  { return mesh.buildNodeGraph(theNodeGraph); }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::Domain::getDbTagData(void) const
  {
    static DbTagData retval(9);
    return retval;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::Domain::sendData(CommParameters &cp)
  {
    int res= ObjWithRecorders::sendData(cp);
    res+= cp.sendMovable(timeTracker,getDbTagData(),CommMetaData(2));
    res+= cp.sendInt(currentGeoTag,getDbTagData(),CommMetaData(3));
    const int tagChannel= cp.getChannel()->getTag();
    if(lastChannel != tagChannel || lastGeoSendTag != currentGeoTag)
      {
        lastChannel= tagChannel;
        //
        // into an ID we are gonna place the class and db tags for each node so can rebuild
        // this ID we then send to the channel
        //
        lastGeoSendTag= currentGeoTag;// now so that we don't do this next time if nothing in the domain has changed
      }
    res+= cp.sendMovable(mesh,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(constraints,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(theEigenvalues,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(modalParticipationFactors,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::Domain::recvData(const CommParameters &cp)
  {
    int res= ObjWithRecorders::recvData(cp);
    res+= cp.receiveMovable(timeTracker,getDbTagData(),CommMetaData(2));
    //
    // now if the currentGeoTag does not agree with whats in the domain
    // we must wipe everything in the domain and recreate the domain based on the info from the channel
    //
    const int tagChannel= cp.getChannel()->getTag();
    const int geoTag= getDbTagDataPos(3);
    if(currentGeoTag == 0 || lastChannel != tagChannel  || geoTag != currentGeoTag)
      {
        // set the currrentGeoTag
        lastChannel= tagChannel;
  
        // if receiving set lastGeoSendTag to be equal to currentGeoTag
        // at time all the data was sent if not we must clear out the objects and rebuild
        lastGeoSendTag= geoTag; currentGeoTag= geoTag;
  
        // mark domainChangeFlag as false
        // this way if restoring froma a database and domain has not changed for the analysis
        // the analysis will not have to to do a domainChanged() operation
        hasDomainChangedFlag= false;
      }

    res+= cp.receiveMovable(mesh,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(constraints,getDbTagData(),CommMetaData(5));
    // now set the domains lastGeoSendTag and currentDomainChangedFlag
    lastGeoSendTag = currentGeoTag;
    res+= cp.receiveVector(theEigenvalues,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(modalParticipationFactors,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Domain::sendSelf(CommParameters &cp)
  {
    // update the commitTag and currentGeoTag
    commitTag= cp.getCommitTag();
    hasDomainChanged();

    inicComm(9);
    int retval= sendData(cp);

    if(dbTag==0)
      dbTag = cp.getChannel()->getDbTag();
    retval+= cp.sendIdData(getDbTagData(),dbTag);
    if(retval < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; channel failed to send data.\n";
    return retval;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Domain::recvSelf(const CommParameters &cp)
  {
    // set the commitTag in the domain to cTag & update the getTag if needed
    commitTag= cp.getCommitTag();
    hasDomainChanged();
  
    // first we get the data about the state of the domain for this commitTag
    inicComm(9);

    int retval= cp.receiveIdData(getDbTagData(),dbTag);
    if(retval < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; channel failed to recv the initial ID.\n";
    else
      retval+= recvData(cp);
    if(retval<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    return retval;
  }
  
//! @brief Returns the value of the dof component of displacement for the node with the tag being passed as parameter.
//! @param nodeTag: node identifier.
//! @param dof: component of the displacement.
//! @param errorFlag: error indicator.
double XC::Domain::getNodeDisp(int nodeTag, int dof,int &errorFlag)
  { return mesh.getNodeDisp(nodeTag,dof,errorFlag); }

//! @brief Set la matriz de masas al nudo identified by the argument.
int XC::Domain::setMass(const XC::Matrix &mass, int nodeTag)
  { return mesh.setMass(mass,nodeTag); }

//! @brief Sets the exception for node reactions checking (see Domain::checkNodalReactions).
void XC::Domain::setNodeReactionException(const int &n)
  { mesh.setNodeReactionException(n); }

//! @brief Check that al free nodes have zero reaction.
void XC::Domain::checkNodalReactions(const double &tol)
  { mesh.checkNodalReactions(tol); }

//! @brief Calculate nodal reaction forces and moments.
int XC::Domain::calculateNodalReactions(bool inclInertia,const double &tol)
  {
    int retval= mesh.calculateNodalReactions(inclInertia,tol);
    retval+= constraints.calculateNodalReactions(inclInertia,tol);
    return retval;
  }

//! @brief Returns (if possible) a pointer to the preprocessor.
const XC::Preprocessor *XC::Domain::getPreprocessor(void) const
  {
    const Preprocessor *retval= nullptr;
    const EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<const Preprocessor *>(owr);
    return retval;
  }

//! @brief Returns (if possible) a pointer to the preprocessor.
XC::Preprocessor *XC::Domain::getPreprocessor(void)
  {
    Preprocessor *retval= nullptr;
    EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<Preprocessor *>(owr);
    return retval;
  }

//! @brief Sends domain through the specified channel.
int XC::sendDomain(Domain &dom,int posDbTag,DbTagData &dt,CommParameters &cp)
  {
    int retval= dom.sendSelf(cp);
    retval+= cp.sendInt(dom.dbTag,dt,CommMetaData(posDbTag));
    return retval;
  }

//! @brief Recibe el domain through the specified channel.
int XC::receiveDomain(Domain &dom,int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    int res= cp.receiveInt(dom.dbTag,dt,CommMetaData(posDbTag));
    res+= dom.recvSelf(cp);
    if(res < 0)
      std::cerr << __FUNCTION__
		<< "; failed to receive vector data.\n";
    return res;
  }
