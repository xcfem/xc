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


void XC::Domain::libera(void)
  {
    //delete the objects in the domain
    clearAll();
  }

//! @brief Constructor.
//!
//! Constructs an empty domain. The storage for the DomainComponents uses
//! ArrayOfTaggedObjects objects for each type of object to be stored.
XC::Domain::Domain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(),CallbackCommit(""), dbTag(0),
   currentGeoTag(0), hasDomainChangedFlag(false), commitTag(0),
   mesh(this), constraints(this), theRegions(nullptr),
   nmbCombActual(""), lastChannel(0), lastGeoSendTag(-1) {}

//! @brief Constructor.
//!
//! Constructs an empty Domain. The initial sizes specified
//! for these objects are no more needed (so ignored)
XC::Domain::Domain(EntCmd *owr,int numNodes, int numElements, int numSPs, int numMPs, int numLoadPatterns,int numNodeLockers,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(), CallbackCommit(""), dbTag(0),
   currentGeoTag(0), hasDomainChangedFlag(false), commitTag(0), mesh(this),
   constraints(this), theRegions(nullptr), nmbCombActual(""), lastChannel(0),
   lastGeoSendTag(-1) {}

//! @brief Removes all components from domain (nodes, elements, loads & constraints).
//! GENERAL NOTE ON REMOVAL OF COMPONENTS:
//!   downward casts (while bad) are o.k. as only the type
//!  of components can be added to the storage objects, e.g.
//!  only elements can be added to theElements therefore
//!  casting a XC::DomainComponent from theElements to an XC::Element is o.k.
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
  { libera(); }

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

//! @brief Adds to the domain el nodo being passed as parameter.
bool XC::Domain::addNode(Node * node)
  { return mesh.addNode(node); }

//! @brief Adds a single freedom constraint to the domain.
//!
//! To add the single point constraint pointed to by spConstraint to the
//! domain. 
//! In addition the container always checks to ensure that no other
//! constraint with a similar tag exists in the domain. The
//! domain then invokes setDomain(this) on the 
//! constraint and domainChange() on itself. The call returns true
//! if the constraint was added, otherwise the warning() method of
//! the global ErrorHandler is invoked and false is returned.
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

//! @brief Adds to the domain una constraint multipunto.
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

//! @brief Adds to the domain una constraint multi retained node.
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
        std::cerr << "Domain::addSFreedom_Constraint - " << pattern
                  << " pattern could not add the XC::SFreedom_Constraint\n";
        return false;
      }

    spConstraint->setDomain(this);
    this->domainChange();
    return true;
  }

//! @brief Appends a nodal load to the pattern being passed as parameter.
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

//! @brief Adds to the caso being passed as parameter una load over elements.
bool XC::Domain::addElementalLoad(ElementalLoad *load, int pattern)
  {
    bool result= constraints.addElementalLoad(load,pattern);
    if(result == false)
      {
        std::cerr << "Domain::addElementalLoad() - no pattern with tag " <<
        pattern << " in  the model, not adding the ele load" << *load << std::endl;
        return false;
      }

    // load->setDomain(this); // done in LoadPattern::addElementalLoad()
    this->domainChange();
    return result;
  }

//! @brief Clears the element identified by the tag being passed as parameter.
bool XC::Domain::removeElement(int tag)
  { return mesh.removeElement(tag); }

//! @brief Elimina the node identified by the tag being passed as parameter.
bool XC::Domain::removeNode(int tag)
  { return mesh.removeNode(tag); }

bool XC::Domain::removeSFreedom_Constraint(int theNode, int theDOF, int loadPatternTag)
  {
    bool retval= constraints.removeSFreedom_Constraint(theNode,theDOF,loadPatternTag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Elimina del domain la constraint monopunto cuyo tag being passed as parameter.
bool XC::Domain::removeSFreedom_Constraint(int tag)
  {
    bool retval= constraints.removeSFreedom_Constraint(tag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Elimina del domain la constraint multipunto cuyo tag being passed as parameter.
bool XC::Domain::removeMFreedom_Constraint(int tag)
  {
    bool result = constraints.removeMFreedom_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }


//! @brief Elimina del domain la constraint multi retained node cuyo tag being passed as parameter.
bool XC::Domain::removeMRMFreedom_Constraint(int tag)
  {
    bool result = constraints.removeMRMFreedom_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }

//! @brief Adds al modelo la hipótesis simple being passed as parameter.
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

//! @brief Adds al modelo 
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
	  std::clog << getClassName() << __FUNCTION__
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

//! @brief Elimina del domain el load pattern cuyo tag being passed as parameter.
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

//! @brief Elimina del domain el 
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

//! @brief Elimina del domain el load pattern being passed as parameter.
bool XC::Domain::removeLoadPattern(LoadPattern *lp)
  {
    bool retval= false;
    if(lp)
      retval= removeLoadPattern(lp->getTag());
    return retval;
  }

//! @brief Elimina del domain el load pattern being passed as parameter.
bool XC::Domain::removeNodeLocker(NodeLocker *nl)
  {
    bool retval= false;
    if(nl)
      retval= removeNodeLocker(nl->getTag());
    return retval;
  }

//! @brief Returns the name of the current load combination.
const std::string &XC::Domain::getNombreCombActual(void) const
  { return nmbCombActual; }

//! @brief Removes from the domain the load combination
//! being passed as parameter.
void XC::Domain::removeLoadCombination(LoadCombination *comb)
  {
    if(comb)
      {
        if((nmbCombActual!= comb->getNombre())&& (!nmbCombActual.empty()))
	  std::clog << "Domain::removeLoadCombination; ¡ojo! "
                    << "removing load combination: " << comb->getNombre()
                    << " without removing: " << nmbCombActual
                    << ".\n";
        nmbCombActual= "";
        if(comb->getDomain()!=this)
          comb->setDomain(this);
        comb->removeFromDomain();
      }
  }

//! @brief Elimina del domain todos los load patterns.
void XC::Domain::removeLPs(void)
  {
    int numSPs= constraints.removeLPs();
    // mark the domain has having changed if numSPs > 0
    // as the constraint handlers have to be redone
    if(numSPs>0)
      domainChange();
  }

//! @brief Elimina del domain todos los bloqueos de nodos.
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
//! @param elemLoadTag: Identificador de the load over elements a eliminar.
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

//! @brief Returns an iterator a los nodos del domain.
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

//! @brief Returns a pointer to the element identified
//! by the tag being passed as parameter.
XC::Element *XC::Domain::getElement(int tag)
  { return mesh.getElement(tag); }

//! @brief Returns a const pointer to the element identified
//! by the tag being passed as parameter.
const XC::Element *XC::Domain::getElement(int tag) const
  { return mesh.getElement(tag); }


//! @brief Returns true if the mesh has a node with the tag being passed as parameter.
bool XC::Domain::existNode(int tag)
 { return mesh.existNode(tag); }


//! @brief Returns a pointer to the nodo cuyo tag being passed as parameter.
XC::Node *XC::Domain::getNode(int tag)
  { return mesh.getNode(tag); }

//! @brief Returns a pointer to the nodo cuyo tag being passed as parameter.
const XC::Node *XC::Domain::getNode(int tag) const
  { return mesh.getNode(tag); }

int XC::Domain::getCommitTag(void) const
  { return commitTag; }

//! @brief Returns the number of elements.
int XC::Domain::getNumElements(void) const
  { return mesh.getNumElements(); }

//! @brief Returns the número de nodos.
int XC::Domain::getNumNodes(void) const
  { return mesh.getNumNodes(); }

//! @brief Returns the BND of the model.
const XC::Vector &XC::Domain::getPhysicalBounds(void)
  { return mesh.getPhysicalBounds(); }

//! @brief Builds (if necessary) the domain elements graph and
//! returns a reference to it.
XC::Graph &XC::Domain::getElementGraph(void)
  { return mesh.getElementGraph(); }

//! @brief Builds (if necessary) the domain nodes graph and
//! returns a reference to it.
XC::Graph &XC::Domain::getNodeGraph(void)
  { return mesh.getNodeGraph(); }

void XC::Domain::setCommitTag(int newTag)
  { commitTag = newTag; }

void XC::Domain::setCurrentTime(double newTime)
  { timeTracker.setCurrentTime(newTime); }

void XC::Domain::setCommittedTime(double newTime)
  { timeTracker.setCommittedTime(newTime); }

void XC::Domain::setTime(double newTime)
  {
    setCurrentTime(newTime);
    setCommittedTime(newTime);
  }

void XC::Domain::applyLoad(double timeStep)
  {
    // set the current pseudo time in the domain to be newTime
    setCurrentTime(timeStep);

    // first loop over nodes and elements getting them to first zero their loads
    mesh.zeroLoads();

    constraints.applyLoad(timeStep);
  }

//! @brief Set all the loads as constant.
void XC::Domain::setLoadConstant(void)
  { constraints.setLoadConstant(); }

//! @brief Inicializa.
int XC::Domain::initialize(void)
  { return mesh.initialize(); }

//! @brief Asigna valores a los coeficientes de amortiguamiento de Rayleigh.
int XC::Domain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { return mesh.setRayleighDampingFactors(rF); }

//! @brief Commits domain state and triggers "record" method
//! for all defined recorders.
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

//! @brief Updates the state of the mesh.
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

void XC::Domain::setDomainChangeStamp(int newStamp)
  { currentGeoTag= newStamp; }


//! @brief Establece que the model ha cambiado.
void XC::Domain::domainChange(void)
  { hasDomainChangedFlag= true; }

//! @brief Returns true if the modelo ha cambiado.
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

//! @brief Imprime el domain.
void XC::Domain::Print(std::ostream &s, int flag)
  {

    s << "Current Domain Information\n";
    s << timeTracker << std::endl;

    mesh.Print(s, flag);
    constraints.Print(s,flag);
  }

std::ostream &XC::operator<<(std::ostream &s, XC::Domain &M)
  {
    M.Print(s);
    return s;
  }

//! @brief Adds a recorder to the model.
int XC::Domain::addRecorder(Recorder &theRecorder)
  {
    if(theRecorder.setDomain(*this) != 0)
      {
        std::cerr << "Domain::addRecorder() - recorder could not be added\n";
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

//! @brief Returns a pointer to the la región cuyo tag being passed as parameter.
XC::MeshRegion *XC::Domain::getRegion(int tag)
  {
    MeshRegion *retval= nullptr;
    if(theRegions)
      retval= theRegions->getRegion(tag);
    return retval;
  }

//! @brief Builds the element graph.
int XC::Domain::buildEleGraph(Graph &theEleGraph)
  { return mesh.buildEleGraph(theEleGraph); }

//! @brief Builds the node graph.
int XC::Domain::buildNodeGraph(Graph &theNodeGraph)
  { return mesh.buildNodeGraph(theNodeGraph); }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
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
      std::cerr << "Domain::sendSelf() - ch failed to send data.\n";
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
      std::cerr << "Domain::recvSelf() - ch failed to recv the initial ID\n";
    else
      retval+= recvData(cp);
    if(retval<0)
      std::cerr << "Domain::recvSelf() - data could not be received.\n" ;
    return retval;
  }
  
//! @brief Returns the value of dof component of displacement for the node with the tag being passed as parameter.
//! @param nodeTag: node identifier.
//! @param dof: component of the displacement.
//! @param errorFlag: error indicator.
double XC::Domain::getNodeDisp(int nodeTag, int dof,int &errorFlag)
  { return mesh.getNodeDisp(nodeTag,dof,errorFlag); }

//! @brief Asigna la matriz de masas al nudo cuyo tag being passed as parameter.
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
      std::cerr << "receiveDomain - failed to receive vector data\n";
    return res;
  }
