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
//ConstrContainer.cc

#include <cstdlib>

#include "ConstrContainer.h"
#include <domain/domain/Domain.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <domain/constraints/MRMFreedom_Constraint.h>
#include <domain/mesh/node/Node.h>
#include "domain/mesh/element/Element.h"
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/NodeLocker.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

#include <domain/domain/single/SingleDomSFreedom_Iter.h>
#include <domain/domain/single/SingleDomMFreedom_Iter.h>
#include <domain/domain/single/SingleDomMRMFreedom_Iter.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/domain/single/SingleDomAllSFreedom_Iter.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>


#include "utility/actor/actor/MovableID.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/loaders/LoadLoader.h"

//@brief Frees memory.
void XC::ConstrContainer::libera(void)
  {
    //delete the objects in the domain
    clearAll();

    // delete all the storage objects
    // SEGMENT FAULT WILL OCCUR IF THESE OBJECTS WERE NOT CONSTRUCTED
    // USING NEW
    if(theSPs) delete theSPs;
    theSPs= nullptr;
    if(theMPs) delete theMPs;
    theMPs= nullptr;
    if(theSFreedom_Iter) delete theSFreedom_Iter;
    theSFreedom_Iter= nullptr;
    if(theMFreedom_Iter) delete theMFreedom_Iter;
    theMFreedom_Iter= nullptr;
    if(theMRMFreedom_Iter) delete theMRMFreedom_Iter;
    theMRMFreedom_Iter= nullptr;
    if(allSFreedom_Iter) delete allSFreedom_Iter;
    allSFreedom_Iter= nullptr;
  }

//! @brief Allocates memory for constraint containers.
void XC::ConstrContainer::alloc_containers(void)
  {
    // init the arrays for storing the constraints components
    theSPs= new MapOfTaggedObjects(this,"SPs");
    theMPs= new MapOfTaggedObjects(this,"MPs");
    theMRMPs= new MapOfTaggedObjects(this,"MRMPs");
  }

//! @brief Allocates memory for constraint iterators.
void XC::ConstrContainer::alloc_iters(void)
  {
    // init the iters
    theSFreedom_Iter= new SingleDomSFreedom_Iter(theSPs);
    theMFreedom_Iter= new SingleDomMFreedom_Iter(theMPs);
    theMRMFreedom_Iter= new SingleDomMRMFreedom_Iter(theMRMPs);
    allSFreedom_Iter= new SingleDomAllSFreedom_Iter(*getDomain());
  }

//! @brief Check that pointers are not null.
bool XC::ConstrContainer::check_containers(void) const
  {
    // check that there was space to create the data structures
    if((theSPs == nullptr) || (theMPs == nullptr) || (theMRMPs == nullptr) ||
       (theMFreedom_Iter == nullptr) || (theSFreedom_Iter == nullptr) || (theMRMFreedom_Iter == nullptr))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " - out of memory\n";
        return false;
      }
    else
      return true;
  }

//! @brief Constructor.
//! @param owr: domain that owns the container.
XC::ConstrContainer::ConstrContainer(Domain *owr)
  :MeshComponentContainer(owr,DOMAIN_TAG_ConstrContainer), activeNodeLockers(this), activeLoadPatterns(this)
  {
    // init the arrays for storing the domain components
    alloc_containers();
    alloc_iters();
    if(!check_containers())
      { exit(-1); }
  }

//! @brief Deletes all the members of the container
void XC::ConstrContainer::clearAll(void)
  {
    // clear the loads and constraints from any load pattern
    for(MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      (*i).second->clearAll();

    for(MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      (*i).second->clearAll();

    // clean out the containers
    if(theSPs) theSPs->clearAll();
    if(theMPs) theMPs->clearAll();
    activeNodeLockers.clear();
    activeLoadPatterns.clear(); //y estos en LoadLoader::clearAll
  }

//! @brief Destructor.
XC::ConstrContainer::~ConstrContainer(void)
  { libera(); }


//! @brief Appends a single freedom constraint.
//! @param spConstraint: pointer to the single freedom constraint to append.
bool XC::ConstrContainer::addSFreedom_Constraint(SFreedom_Constraint *spConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= spConstraint->getTag();
    TaggedObject *other= theSPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << " - cannot add a constraint with tag "
                  << tag << " already exists in model\n";
        retval= false;
      }
    else
      retval= theSPs->addComponent(spConstraint);
    return retval;
  }


//! @brief Appends a multiple freedom constraint.
//! @param mfConstraint: pointer to the multi-freedom constraint to append.
bool XC::ConstrContainer::addMFreedom_Constraint(MFreedom_Constraint *mfConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= mfConstraint->getTag();
    TaggedObject *other= theMPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << " - cannot add as constraint with tag "
                  << tag << " already exists in model";
      }
    else
      retval= theMPs->addComponent(mfConstraint);
    return retval;
  }

//! @brief Appends a multi-retained nodes constraint.
//! @param mrmfConstraint: pointer to the multi-retained nodes constraint to append.
bool XC::ConstrContainer::addMRMFreedom_Constraint(MRMFreedom_Constraint *mrmfConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= mrmfConstraint->getTag();
    TaggedObject *other= theMRMPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << " - cannot add as constraint with tag "
                  << tag << " already exists in model";
      }
    else
      retval= theMRMPs->addComponent(mrmfConstraint);
    return retval;
  }

//! @brief Appends a load pattern.
//! @param load: pointer to the load pattern to append.
bool XC::ConstrContainer::addLoadPattern(LoadPattern *load)
  {
    bool retval= true;
    // first check if a load pattern with a similar tag exists in model
    const int tag= load->getTag();
    MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.find(tag);
    if(i!=activeLoadPatterns.end())
      {
        if(verbosity>3)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << " - cannot add as LoadPattern with tag "
                    << tag << " already exists in model\n";
        retval= false;
      }
    else
      activeLoadPatterns[tag]= load;// now we add the load pattern
    return retval;
  }

//! @brief Appends a node locker.
//! @param nl: pointer to the node locker to append.
bool XC::ConstrContainer::addNodeLocker(NodeLocker *nl)
  {
    bool retval= true;
    // first check if a locker with a similar tag exists in model
    const int tag= nl->getTag();
    MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.find(tag);
    if(i!=activeNodeLockers.end())
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << " - cannot add as LoadPattern with tag "
                  << tag << " already exists in model\n";
        retval= false;
      }
    else
      activeNodeLockers[tag]= nl;
    return retval;
  }

//! @brief Appends a single freedom constraint to a load pattern.
//! @param spConstraint: pointer to the single freedom constraint to append.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::addSFreedom_Constraint(SFreedom_Constraint *spConstraint, int loadPatternTag)
  {
    bool retval= false;
    // now add it to the pattern
    LoadPattern *caso= getLoadPattern(loadPatternTag);
    if(caso)
      retval= caso->addSFreedom_Constraint(spConstraint);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " - cannot add as pattern with tag "
                << loadPatternTag << " does not exist in domain\n";
    return retval;
  }

//! @brief Appends a nodal load to a load pattern.
//! @param load: pointer to the nodal load to append.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::addNodalLoad(NodalLoad *load, int loadPatternTag)
  {
    bool retval= false;
    int nodTag= load->getNodeTag();
    const Node *res= getDomain()->getNode(nodTag);
    if(res == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " - no node with tag: " << nodTag
		  << " exists in  the model, not adding the nodal load"
		  << *load << std::endl;
      }
    else
      {
        // now add it to the pattern
        LoadPattern *caso= getLoadPattern(loadPatternTag);
        if(caso)
          retval= caso->addNodalLoad(load);
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << " - no pattern with tag: " << loadPatternTag
		      << " in  the model, not adding the nodal load"
		      << *load << std::endl;
          }
      }
    return retval;
  }

//! @brief Appends an elemental load to a load pattern.
//! @param load: pointer to the elemental load to append.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::addElementalLoad(ElementalLoad *load, int loadPatternTag)
  {
    bool retval= false;
    // now add it to the pattern
    LoadPattern *caso= getLoadPattern(loadPatternTag);
    if(caso)
      retval= caso->addElementalLoad(load);
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " - no pattern with tag " << loadPatternTag
		  << "exits in  the model, not adding the ele load "
		  << *load << std::endl;
      }
    return retval;
  }


//! @brief Removes a single freedom constraint from this container or from a load pattern.
//! @param theNode: node tag.
//! @param theDOF: degree of freedom identifier.
//! @param loadPatternTag: load pattern identifier (if -1 then remove from domain).
bool XC::ConstrContainer::removeSFreedom_Constraint(int theNode, int theDOF, int loadPatternTag)
  {
    SFreedom_Constraint *theSP= nullptr;

    if(loadPatternTag == -1)
      {
	theSP= getSPs().search(theNode,theDOF);
	if(theSP)
	  this->removeSFreedom_Constraint(theSP->getTag());
      }
    else
      {
        LoadPattern *thePattern= this->getLoadPattern(loadPatternTag);
        if(thePattern)
          {
  	    theSP= thePattern->getSPs().search(theNode,theDOF);
	    if(theSP)
	      thePattern->removeSFreedom_Constraint(theSP->getTag());
          }
      }
    return 0;
  }

//! @brief Removes the single freedom constraint whose identifier is being passed as parameter.
bool XC::ConstrContainer::removeSFreedom_Constraint(int tag)
  {
    SFreedom_Constraint *theSP= dynamic_cast<SFreedom_Constraint *>(theSPs->getComponentPtr(tag));
    if(theSP)
      theSP->setDomain(nullptr);
    return theSPs->removeComponent(tag);
  }

//! @brief Removes the multi-freedom constraint whose identifier is being passed as parameter.
bool XC::ConstrContainer::removeMFreedom_Constraint(int tag)
  {
    MFreedom_Constraint *theMP= dynamic_cast<MFreedom_Constraint *>(theMPs->getComponentPtr(tag));
    if(theMP)
      theMP->setDomain(nullptr);
    return theMPs->removeComponent(tag);
  }

//! @brief Removes the multi-row multi-freedom constraint whose identifier is being passed as parameter.
bool XC::ConstrContainer::removeMRMFreedom_Constraint(int tag)
  {
    MRMFreedom_Constraint *theMRMP= dynamic_cast<MRMFreedom_Constraint *>(theMRMPs->getComponentPtr(tag));
    if(theMRMP)
      theMRMP->setDomain(nullptr);
    return theMRMPs->removeComponent(tag);
  }

//! @brief Removes the load pattern whose identifier is being passed as parameter.
bool XC::ConstrContainer::removeLoadPattern(int tag,int &numSPs)
  { 
    MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.find(tag);

    bool retval= false;
    numSPs= 0;

    if(i!=activeLoadPatterns.end())
      {
        // perform a downward cast, set the objects domain pointer to 0
        // and return the result of the cast
        const LoadPattern *result= (*i).second;
        if(result)
          {
            numSPs= result->getNumSPs();
            activeLoadPatterns.erase(i);
            retval= true;
          }
        else
	  std::cerr << "error en ConstrContainer::removeLoadPattern" << std::endl;
      }
    // finally return the load pattern
    return retval;
  }

//! @brief Removes the node locker whose identifier is being passed as parameter.
bool XC::ConstrContainer::removeNodeLocker(int tag,int &numSPs)
  {
    // remove the object from the container
    MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.find(tag);

    bool retval= false;
    numSPs= 0;

    if(i!=activeNodeLockers.end())
      {
        // perform a downward cast, set the objects domain pointer to 0
        // and return the result of the cast
        const NodeLocker *result= (*i).second;
        if(result)
          {
            numSPs= result->getNumSPs();
            activeNodeLockers.erase(i);
            retval= true;
          }
        else
	  std::cerr << "error en ConstrContainer::removeNodeLocker" << std::endl;
      }
    return retval;
  }

//! @brief Removes all load patterns.
int XC::ConstrContainer::removeLPs(void)
  {
    const std::deque<int> tags= getTagsLPs();
    int numSPs= 0;
    int tmp;
    for(std::deque<int>::const_iterator i= tags.begin(); i!=tags.end();i++)
      {
	removeLoadPattern(*i,tmp);
        numSPs+= tmp;
      }
    return numSPs;
  }

//! @brief Removes all nodel lockers.
int XC::ConstrContainer::removeNLs(void)
  {
    const std::deque<int> tags= getTagsNLs();
    int numSPs= 0;
    int tmp;
    for(std::deque<int>::const_iterator i= tags.begin(); i!=tags.end();i++)
      {
	removeNodeLocker(*i,tmp);
        numSPs+= tmp;
      }
    return numSPs;
  }

//! @brief Removes a nodal load from a load pattern.
//! @param tag: identifier of the load.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::removeNodalLoad(int tag, int loadPatternTag)
  {
    bool retval= false;
    // remove the object from the container
    LoadPattern *theLoadPattern= getLoadPattern(loadPatternTag);

    // if not there return 0
    if(theLoadPattern)
      retval= theLoadPattern->removeNodalLoad(tag);
    return retval;
  }

//! @brief Removes a load over elements from a load pattern.
//! @param tag: identifier of the load.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::removeElementalLoad(int tag, int loadPatternTag)
  {
    bool retval= false;
    // remove the object from the container
    LoadPattern *theLoadPattern= this->getLoadPattern(loadPatternTag);
    // if not there return 0
    if(theLoadPattern)
      retval= theLoadPattern->removeElementalLoad(tag);
    return retval;
  }

//! @brief Removes a single freedom constraint from al load pattern.
//! @param tag: identifier of the single freedom constraint.
//! @param loadPatternTag: load pattern identifier.
bool XC::ConstrContainer::removeSFreedom_Constraint(int tag, int loadPattern)
  {
    bool retval= false;
    LoadPattern *theLoadPattern= this->getLoadPattern(loadPattern);// remove the object from the container
    if(theLoadPattern)
      retval= theLoadPattern->removeSFreedom_Constraint(tag);
    return retval;
  }

//! @brief Domain single freedom constraints iterator.
XC::SFreedom_ConstraintIter &XC::ConstrContainer::getSPs(void)
  {
    theSFreedom_Iter->reset();
    return *theSFreedom_Iter;
  }

//! @brief All (domain and load cases) single freedom constraints iterator.
XC::SFreedom_ConstraintIter &XC::ConstrContainer::getDomainAndLoadPatternSPs(void)
  {
    allSFreedom_Iter->reset();
    return *allSFreedom_Iter;
  }

//! @brief Domain multi-freedom constraints iterator.
XC::MFreedom_ConstraintIter &XC::ConstrContainer::getMPs(void)
  {
    theMFreedom_Iter->reset();
    return *theMFreedom_Iter;
  }

//! @brief Domain multi-row multi-freedom constraints iterator.
XC::MRMFreedom_ConstraintIter &XC::ConstrContainer::getMRMPs(void)
  {
    theMRMFreedom_Iter->reset();
    return *theMRMFreedom_Iter;
  }


//! @brief Returns the active load patterns container.
std::map<int,XC::LoadPattern *> &XC::ConstrContainer::getLoadPatterns(void)
  { return activeLoadPatterns; }

//! @brief Returns the active node lockers container.
std::map<int,XC::NodeLocker *> &XC::ConstrContainer::getNodeLockers(void)
  { return activeNodeLockers; }

//! @brief Returns the active load patterns container.
const std::map<int,XC::LoadPattern *> &XC::ConstrContainer::getLoadPatterns(void) const
  { return activeLoadPatterns; }

//! @brief Returns the active node lockers container.
const std::map<int,XC::NodeLocker *> &XC::ConstrContainer::getNodeLockers(void) const
  { return activeNodeLockers; }


/* GENERAL NOTE ON RETRIEVAL OF COMPONENT PTRs:
**   downward casts (while bad) are o.k. as only the type
**   of components can be added to the storage objects, e.g.
**   only elements can be added to theElements
*/

//! @brief Returns a pointer to the single freedom constraint whose identifier is being passed as parameter.
XC::SFreedom_Constraint *XC::ConstrContainer::getSFreedom_Constraint(int tag)
  {
    TaggedObject *mc= theSPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    SFreedom_Constraint *result= dynamic_cast<SFreedom_Constraint *>(mc);
    return result;
  }

//! @brief Returns a pointer to the multi-freedom constraint whose identifier is being passed as parameter.
XC::MFreedom_Constraint *XC::ConstrContainer::getMFreedom_Constraint(int tag)
  {
    TaggedObject *mc= theMPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    MFreedom_Constraint *result= dynamic_cast<MFreedom_Constraint *>(mc);
    return result;
  }

//! @brief Returns a pointer to the multi-row multi-freedom constraint whose identifier is being passed as parameter.
XC::MRMFreedom_Constraint *XC::ConstrContainer::getMRMFreedom_Constraint(int tag)
  {
    TaggedObject *mc= theMRMPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    MRMFreedom_Constraint *result= dynamic_cast<MRMFreedom_Constraint *>(mc);
    return result;
  }

//! @brief Returns a pointer to the load pattern whose identifier is being passed as parameter.
XC::LoadPattern *XC::ConstrContainer::getLoadPattern(const int &tag)
  {
    LoadPattern *retval= nullptr;
    MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.find(tag);
    if(i==activeLoadPatterns.end())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " - load pattern with tag: " << tag << " not found\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern whose identifier is being passed as parameter.
const XC::LoadPattern *XC::ConstrContainer::getLoadPattern(const int &tag) const
  {
    const LoadPattern *retval= nullptr;
    MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.find(tag);
    if(i==activeLoadPatterns.end())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " - load pattern with tag: "
	        << tag << " not found\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the node locker whose identifier is being passed as parameter.
XC::NodeLocker *XC::ConstrContainer::getNodeLocker(const int &tag)
  {
    NodeLocker *retval= nullptr;
    MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.find(tag);
    if(i==activeNodeLockers.end())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " - node locker with tag: "
	        << tag << " not found\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the node locker whose identifier is being passed as parameter.
const XC::NodeLocker *XC::ConstrContainer::getNodeLocker(const int &tag) const
  {
    const NodeLocker *retval= nullptr;
    MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.find(tag);
    if(i==activeNodeLockers.end())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " - node locker with tag: "
	        << tag << " not found\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Returns the number of single freedom constraints.
int XC::ConstrContainer::getNumSPs(void) const
  { return theSPs->getNumComponents(); }

//! @brief Returns the number of multi-freedom constraints.
int XC::ConstrContainer::getNumMPs(void) const
  { return theMPs->getNumComponents(); }

//! @brief Returns the number of multi-row multi-freedom constraints.
int XC::ConstrContainer::getNumMRMPs(void) const
  { return theMRMPs->getNumComponents(); }

//! @brief Returns the number of load patterns.
int XC::ConstrContainer::getNumLoadPatterns(void) const
  { return activeLoadPatterns.size(); }

//! @brief Returns the number of node lockers.
int XC::ConstrContainer::getNumNodeLockers(void) const
  { return activeNodeLockers.size(); }

//! @brief Apply loads from active node lockers.
void XC::ConstrContainer::applyLoad(double timeStep)
  {
    // now loop over node lockers, invoking applyLoad on them
    for(MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      i->second->applyLoad();



    // now loop over load patterns, invoking applyLoad on them
    for(MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      i->second->applyLoad(timeStep);

    //
    // finally loop over the MFreedom_Constraints and SFreedom_Constraints of the domain
    //

    MFreedom_ConstraintIter &theMPs= getMPs();
    MFreedom_Constraint *theMP;
    while((theMP= theMPs()) != 0)
        theMP->applyConstraint(timeStep);

    MRMFreedom_ConstraintIter &theMRMPs= getMRMPs();
    MRMFreedom_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
        theMRMP->applyConstraint(timeStep);

    SFreedom_ConstraintIter &theSPs= getSPs();
    SFreedom_Constraint *theSP;
    while((theSP= theSPs()) != 0)
      theSP->applyConstraint(timeStep);
  }

//! @brief Search on the container all the single freedom constraints with the node and degree of freedom being passed as parameter.
std::deque<int> XC::ConstrContainer::getTagsSPsNode(int theNode, int theDOF) const
  {
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    return this_no_const->getSPs().searchAll(theNode,theDOF); 
  }

//! @brief Search on the container all the single freedom constraints that affect the node whose tag is being passed as parameter.
std::deque<int> XC::ConstrContainer::getTagsSPsNode(int theNode) const
  {
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    return this_no_const->getSPs().searchAll(theNode); 
  }

//! @brief Search on the container all the multi-freedom constraints with the node and degree of freedom being passed as parameter.
std::deque<int> XC::ConstrContainer::getTagsMPsNode(int theNode, int theDOF) const
  {
    std::deque<int> retval; 

    std::cerr << "XC::ConstrContainer::getTagsMPsNode not implemented." << std::endl;
    return retval;
  }

//! @brief Search on the container all the multi-freedom constraints that affect the node whose tag is being passed as parameter.
std::deque<int> XC::ConstrContainer::getTagsMPsNode(int theNode) const
  {
    std::deque<int> retval; 

    std::cerr << "XC::ConstrContainer::getTagsMPsNode not implemented." << std::endl;
    return retval;
  }

//! @brief Loop over all the load patterns that are currently added to the domain getting their tag.
std::deque<int> XC::ConstrContainer::getTagsLPs(void) const
  {
    std::deque<int> retval;   
    for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      retval.push_back(i->second->getTag());
    return retval;
  }

//! @brief Loop over all the load patterns that are currently added to the domain getting their names.
std::string XC::ConstrContainer::getLoadPatternsNames(void) const
  {
    std::string retval;
    const size_t sz= activeLoadPatterns.size();
    if(sz>0)
      {
        const Domain *dom= getDomain();
        if(dom)
          {
            const Preprocessor *preprocessor= dom->getPreprocessor();
            if(preprocessor)
              {
                const LoadLoader &loadLoader= preprocessor->getLoadLoader();
                const MapLoadPatterns &casos= loadLoader.getLoadPatterns();
                for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();
                    i!= activeLoadPatterns.end();i++)
                  retval+= casos.getLoadPatternName((*i).second) + " ";
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << " preprocessor not defined." << std::endl;
          }
        else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << " domain not defined." << std::endl;
      }
    return retval;
  }

//! @brief Loop over all the load patterns that are currently added to the domain getting their tag.
std::deque<int> XC::ConstrContainer::getTagsNLs(void) const
  {
    // loop over all the nodel lockers that are currently added to the domain
    // getting theit tag
    std::deque<int> retval;   
    for(MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      retval.push_back(i->second->getTag());
    return retval;
  }

//! @brief Returns true if the node is affected by one or more single freedom constraints.
bool XC::ConstrContainer::nodeAffectedBySPs(int nodeTag) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    SFreedom_ConstraintIter &theSPs= this_no_const->getDomainAndLoadPatternSPs();
    SFreedom_Constraint *theSP= nullptr;
    while((theSP= theSPs()) != 0)
      if(theSP->getNodeTag() == nodeTag)
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Returns the tags of the nodes that are which theDOF
//! constrained by one or more single freedom constraints.
std::set<int> XC::ConstrContainer::getTagsNodesffectedBySPs(int theDOF) const
  {
    std::set<int> retval;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    SFreedom_ConstraintIter &theSPs= this_no_const->getDomainAndLoadPatternSPs();
    SFreedom_Constraint *theSP= nullptr;
    while((theSP= theSPs()) != 0)
      {
	if(theSP->getDOF_Number() == theDOF)
	  retval.insert(theSP->getNodeTag());
      }
    return retval;
  }

//! @brief Returns true if the node is affected by one or more multi-freedom constraints.
bool XC::ConstrContainer::nodeAffectedByMPs(int nodeTag) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    MFreedom_ConstraintIter &theMPs= this_no_const->getMPs();
    MFreedom_Constraint *theMP;
    while((theMP= theMPs()) != 0)
      if(theMP->affectsNode(nodeTag))
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Returns true if the node is affected by one or more multi-row multi-freedom constraints.
bool XC::ConstrContainer::nodeAffectedByMRMPs(int nodeTag) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    MRMFreedom_ConstraintIter &theMRMPs= this_no_const->getMRMPs();
    MRMFreedom_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
      if(theMRMP->affectsNode(nodeTag))
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Returns true if the node is affected by any constraint.
bool XC::ConstrContainer::nodeAffectedBySPsOMPs(int nodeTag) const
  {
    bool retval= nodeAffectedBySPs(nodeTag);
    if(!retval)
      retval= nodeAffectedByMPs(nodeTag);
    if(!retval)
      retval= nodeAffectedByMRMPs(nodeTag);
    return retval;
  }

//! @brief Set as constant all the active load patterns (used in pushover analysis).
void XC::ConstrContainer::setLoadConstant(void)
  {
    for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();i!= activeLoadPatterns.end();i++)
      i->second->setLoadConstant(); //set load as constant.
  }

//! @brief Reactions due to constraints.
int XC::ConstrContainer::calculateNodalReactions(bool inclInertia, const double &tol)
  {
    MFreedom_ConstraintIter &theMPs= this->getMPs();
    MFreedom_Constraint *theMP;
    while((theMP= theMPs()) != 0)
      theMP->addResistingForceToNodalReaction(inclInertia);

    MRMFreedom_ConstraintIter &theMRMPs= this->getMRMPs();
    MRMFreedom_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
      theMRMP->addResistingForceToNodalReaction(inclInertia);

    return 0;
  }

//! @brief Returns a vector to store the dbTags of class members.
XC::DbTagData &XC::ConstrContainer::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Send the active load patterns tags through the channel being passed as parameter.
int XC::ConstrContainer::sendLPatternsTags(const int &posFlag,const int &posDbTag,CommParameters &cp)
  {
    int res= 0;
    static ID loadPatternsTags;
    const std::deque<int> tags= getTagsLPs();
    const size_t sz= tags.size();
    if(sz>0)
      {
        setDbTagDataPos(posFlag,1);
        loadPatternsTags.resize(sz);
        for(size_t i= 0; i<sz;i++)
          loadPatternsTags[i]= tags[i];
        res+= cp.sendID(loadPatternsTags,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posFlag,0);
    return res;
  }

//! @brief Receives the active load patterns tags through the channel being passed as parameter.
int XC::ConstrContainer::recvLPatternsTags(const int &posFlag,const int &posDbTag,const CommParameters &cp)
  {
    Domain *dom= getDomain();
    int res= 0;
    static ID loadPatternsTags;
    const int flag= getDbTagDataPos(posFlag);
    if(flag != 0)
      {
        res= cp.receiveID(loadPatternsTags,getDbTagData(),CommMetaData(posDbTag));
    
        assert(dom);
        Preprocessor *preprocessor= dom->getPreprocessor();
        if(preprocessor)
          {
            LoadLoader &loadLoader= preprocessor->getLoadLoader();
            LoadPattern *load= nullptr;
            const size_t sz= loadPatternsTags.Size();
            for(size_t i=0;i<sz;i++)
              {
                load= loadLoader.getLoadPatterns().buscaLoadPattern(loadPatternsTags[i]);
                if(load)
                  {
                    if(addLoadPattern(load))
                      load->setDomain(dom);
                    else
                      {
                        if(verbosity>3)
                          {
                            const MapLoadPatterns &casos= loadLoader.getLoadPatterns();
	                    std::cerr << getClassName() << "::" << __FUNCTION__
			              << "; could not add load pattern: '"
                                      << casos.getLoadPatternName(load)
                                      << "' with tag: " << load->getTag() << std::endl;
                          }
                      }
                  }
                else
	          std::cerr << getClassName() << "::" << __FUNCTION__
		            << " load with tag: " << loadPatternsTags[i]
			    << "not found." << std::endl;
              }
          }
      }
    return res;
  }

//! @brief Send the node lockers tags through the channel being passed as parameter.
int XC::ConstrContainer::sendNLockersTags(const int &posFlag,const int &posDbTag,CommParameters &cp)
  {
    int res= 0;
    static ID nLockersTags;
    const std::deque<int> tags= getTagsNLs();
    const size_t sz= tags.size();
    if(sz>0)
      {
        setDbTagDataPos(posFlag,1);
        nLockersTags.resize(sz);
        for(size_t i= 0; i<sz;i++)
          nLockersTags[i]= tags[i];
        res+= cp.sendID(nLockersTags,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posFlag,0);
    return res;
  }

//! @brief Receives the node lockers tags through the channel being passed as parameter.
int XC::ConstrContainer::recvNLockersTags(const int &posFlag,const int &posDbTag,const CommParameters &cp)
  {
    Domain *dom= getDomain();
    int res= 0;
    static ID nLockersTags;
    const int flag= getDbTagDataPos(posFlag);
    if(flag != 0)
      {
        res= cp.receiveID(nLockersTags,getDbTagData(),CommMetaData(posDbTag));
    
        assert(dom);
        Mesh &mesh= dom->getMesh();
        NodeLocker *nl= nullptr;
        const size_t sz= nLockersTags.Size();
        for(size_t i=0;i<sz;i++)
          {
            nl= mesh.getNodeLockers().buscaNodeLocker(nLockersTags[i]);
            if(nl)
              {
                if(addNodeLocker(nl))
                  nl->setDomain(dom);
              }
            else
              std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; node locker identified by : "
                        << nLockersTags[i] << " not found." << std::endl;
          }
      }
    return res;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::ConstrContainer::sendData(CommParameters &cp)
  {
    int res= cp.sendMovable(*theSPs,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(*theMPs,getDbTagData(),CommMetaData(1));
    res+= cp.sendMovable(*theMRMPs,getDbTagData(),CommMetaData(2));
    
    res+= sendNLockersTags(3,4,cp); //Active node lockers.
    res+= sendLPatternsTags(5,6,cp); //Active load patterns.
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ConstrContainer::recvData(const CommParameters &cp)
  {
    int res= theSPs->recibe<SFreedom_Constraint>(getDbTagDataPos(0),cp,&FEM_ObjectBroker::getNewSP);
    res+= theMPs->recibe<MFreedom_Constraint>(getDbTagDataPos(1),cp,&FEM_ObjectBroker::getNewMP);
    res+= theMRMPs->recibe<MRMFreedom_Constraint>(getDbTagDataPos(2),cp,&FEM_ObjectBroker::getNewMRMP);
    res+= recvNLockersTags(3,4,cp);
    res+= recvLPatternsTags(5,6,cp);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ConstrContainer::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " - channel failed to send the initial ID\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::ConstrContainer::recvSelf(const CommParameters &cp)
  {
    // first we get the data about the state of the cc for this cTag
    inicComm(7);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " - channel failed to recv the initial ID\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Prints object information.
void XC::ConstrContainer::Print(std::ostream &s, int flag)
  {

    s << "Current ConstrContainer Information\n";

    s << "\nSFreedom_Constraints: numConstraints: ";
    s << theSPs->getNumComponents() << "\n";
    theSPs->Print(s, flag);

    s << "\nMFreedom_Constraints: numConstraints: ";
    s << theMPs->getNumComponents() << "\n";
    theMPs->Print(s, flag);

    s << "\nMRMFreedom_Constraints: numConstraints: ";
    s << theMRMPs->getNumComponents() << "\n";
    theMRMPs->Print(s, flag);

    s << "\nLOAD PATTERNS: num Patterns: ";
    s << activeLoadPatterns.size() << "\n\n";
    activeLoadPatterns.Print(s, flag);

    s << "\nNode lockers: num lockers: ";
    s << activeNodeLockers.size() << "\n\n";
    activeNodeLockers.Print(s, flag);
  }

std::ostream &XC::operator<<(std::ostream &s, XC::ConstrContainer &o)
  {
    o.Print(s);
    return s;
  }

