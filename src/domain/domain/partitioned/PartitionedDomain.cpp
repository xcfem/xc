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
// $Date: 2006/01/10 00:33:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/partitioned/PartitionedDomain.cpp,v $


// Written: fmk
// Revision: A
//
// Description: This file contains the class definition for XC::PartitionedDomain.
// PartitionedDomain is an abstract class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints
// and MFreedom_Constraints just like a normal domain. In addition the domain provides
// a method to partition the domain into Subdomains.
//
// ModelBuilder. There are no partitions in a PartitionedDomain.
//
// What: "@(#) PartitionedDomain.C, revA"

#include <domain/domain/partitioned/PartitionedDomain.h>
#include <cstdlib>

#include <domain/partitioner/DomainPartitioner.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <utility/tagged/storage/ArrayOfTaggedObjectsIter.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <domain/partitioner/DomainPartitioner.h>
#include <domain/domain/partitioned/PartitionedDomain.h>
#include <domain/domain/partitioned/PartitionedDomainEleIter.h>
#include <domain/domain/partitioned/PartitionedDomainSubIter.h>
#include <domain/domain/single/SingleDomEleIter.h>
#include <solution/graph/graph/Vertex.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/recorder/Recorder.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"

void XC::PartitionedDomain::free_mem(void)
  {
    if(elements) delete elements;
    elements= nullptr;
    if(theSubdomains) delete theSubdomains;
    theSubdomains= nullptr;
    if(theSubdomainIter) delete theSubdomainIter;
    theSubdomainIter= nullptr;
    if(theEleIter) delete theEleIter;
    theEleIter= nullptr;
  }

//! @brief Allocates memory.
void XC::PartitionedDomain::alloc(void)
  {
    free_mem();
    elements= new ArrayOfTaggedObjects(this,1024,"element");
    theSubdomains= new ArrayOfTaggedObjects(this,32,"subdomains");
    theSubdomainIter= new PartitionedDomainSubIter(theSubdomains);

    mainEleIter= new SingleDomEleIter(elements);
    theEleIter= new PartitionedDomainEleIter(this);

    if(theSubdomains == 0 || elements == 0 ||
        theSubdomainIter == 0 ||
        theEleIter == 0 || mainEleIter == 0)
      {

        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL, ran out of memory.\n";
        exit(-1);
      }
  }

//! @brief Constructor.
//!
//! @param owr: object that encloses this one.
//! @param oh: to DEPRECATE.
XC::PartitionedDomain::PartitionedDomain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  :Domain(owr,oh), theSubdomains(nullptr),theDomainPartitioner(nullptr),
   theSubdomainIter(nullptr), mySubdomainGraph()
  { alloc(); }


//! @brief Constructor.
//!
//! Constructs an empty PartitionedDomain. A link with the domain
//! partitioner \p thePartitioner is set up. The \p thePartitioner
//! is used by the domain to partition and load balance the partitioned domain.
//! 
//! @param owr: object that encloses this one.
//! @param thePartitioner: object that make the partition.
//! @param oh: to DEPRECATE.
XC::PartitionedDomain::PartitionedDomain(EntCmd *owr,DomainPartitioner &thePartitioner,DataOutputHandler::map_output_handlers *oh)
  :Domain(owr,oh), theSubdomains(nullptr),theDomainPartitioner(&thePartitioner),
 theSubdomainIter(nullptr), mySubdomainGraph()
  { alloc(); }


//! @brief Constructor.
//!
//! Constructs an empty PartitionedDomain, storage is allocated for the components
//! that are to be added using the estimated number of components passed
//! as arguments. A link with the domain partitioner \p thePartitioner
//! is set up. The \p thePartitioner is used by the domain to partition
//! and load balance the partitioned domain.
//! 
//! @param owr: object that encloses this one.
//! @param thePartitioner: object that make the partition.
//! @param oh: to DEPRECATE.
XC::PartitionedDomain::PartitionedDomain(EntCmd *owr,int numNodes, int numElements,
                                     int numSPs, int numMPs, int numLoadPatterns, int numNodeLockers,
                                     int numSubdomains,
                                     DomainPartitioner &thePartitioner,DataOutputHandler::map_output_handlers *oh)

  : Domain(owr,numNodes,0,numSPs,numMPs,numLoadPatterns,numNodeLockers,oh),
    theSubdomains(nullptr),theDomainPartitioner(&thePartitioner),theSubdomainIter(nullptr),
    mySubdomainGraph()
  { alloc(); }

//! @brief Destructor.
XC::PartitionedDomain::~PartitionedDomain(void)
  {
    this->clearAll();
    free_mem();
  }

void XC::PartitionedDomain::clearAll(void)
  {
    Domain::clearAll();
    elements->clearAll();

    SubdomainIter &mySubdomains= this->getSubdomains();
    Subdomain *theSub;
    while((theSub= mySubdomains()) != 0)
      theSub->clearAll();
    theSubdomains->clearAll();
  }



//! To add the element pointed to by theElementPtr to the domain.
//!
//! To add the element pointed to by theElementPtr to the domain. If \p check
//! is \p true the domain is responsible for checking to see that: 1)
//! no other element with a similar tag, element number, exists in any of
//! the subdomains. If check is successfull the partitioned domain
//! attempts to add the element to the storage arrey. The call returns
//! \p false if the element was not added, otherwise \p true is 
//! returned.   
bool XC::PartitionedDomain::addElement(Element *elePtr)
  {
    if(elePtr->isSubdomain() == true)
      return this->addSubdomain(dynamic_cast<Subdomain *>(elePtr));

    int eleTag= elePtr->getTag();
#ifdef _DEBUG
    if(check == true)
      {

        // check ele Tag >= 0
        if(eleTag < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; element " << eleTag
		      << " tag must be >= 0\n";
            return false;
          }

        // check its not in this or any of the subdomains
        // MISSING CODE

        // check all the elements nodes exist in the domain
        const ID &nodes= elePtr->getExternalNodes();
        for(int i=0; i<nodes.Size(); i++)
          {
            int nodeTag= nodes(i);
            Node *nodePtr= this->getNode(nodeTag);
            if(nodePtr == 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; in element " << eleTag
			  << " no node with tag: " << nodeTag
			  << " exists in the domain.\n";
                return false;
              }
          }
      }
#endif

    TaggedObject *other= elements->getComponentPtr(eleTag);
    if(other != 0)
      return false;

    bool result= elements->addComponent(elePtr);
    if(result == true)
      {
        elePtr->setDomain(this);
        this->domainChange();
      }
    return result;
  }


//! Adds the node pointed to by theNodePtr to the domain.
//!
//! Adds the node pointed to by theNodePtr to the domain. If \p check
//! is \p true the domain is responsible for checking that no other
//! node with a similar tag, node number, exists in any of the
//! subdomains. If successfull the partition domain attempts to add the
//! node by invoking {\em Domain::addNode}. The call returns \p false if
//! the node was not added, otherwise \p true is returned.  
bool XC::PartitionedDomain::addNode(Node *nodePtr)
  {
#ifdef _DEBUG
    if(check == true)
      {
        // check its not in this or any of the subdomains

        // MISSING CODE
      }
#endif
    return (this->XC::Domain::addNode(nodePtr));
  }



//! @brief Adds the single point constraint pointed to by theSPptr to the
//! domain.
//! 
//! Adds the single point constraint pointed to by theSPptr to the 
//! domain. The domain performs some checks is \p check is true. If
//! successful the domain adds the constraint using {\em
//! Domain::addSFreedom\_Constraint()}. The call returns \p false if
//! the constraint was not added, otherwise \p true is returned.  
bool XC::PartitionedDomain::addSFreedom_Constraint(SFreedom_Constraint *load)
  {
    int nodeTag= load->getNodeTag();

    // check the XC::Node exists in the Domain or one of Subdomains

    // if in XC::Domain add it as external .. ignore Subdomains
    Node *nodePtr= this->getNode(nodeTag);
    if(nodePtr)
      { return (Domain::addSFreedom_Constraint(load)); }

    // find subdomain with node and add it .. break if find as internal node
    SubdomainIter &theSubdomains= this->getSubdomains();
    Subdomain *theSub;
    while((theSub= theSubdomains()) != 0)
      {
        bool res= theSub->hasNode(nodeTag);
        if(res)
          return theSub->addSFreedom_Constraint(load);
      }


    // if no subdomain .. node not in model .. error message and return failure
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; cannot add as node with tag: "
	      << nodeTag << " does not exist in model.\n";
    return false;
  }

bool XC::PartitionedDomain::addSFreedom_Constraint(SFreedom_Constraint *load, int pattern)
  {
    int nodeTag= load->getNodeTag();

    // check the XC::Node exists in the XC::Domain or one of Subdomains

    // if in XC::Domain add it as external .. ignore Subdomains
    Node *nodePtr= this->getNode(nodeTag);
    if(nodePtr)
      { return (Domain::addSFreedom_Constraint(load, pattern)); }

    // find subdomain with node and add it .. break if find as internal node
    SubdomainIter &theSubdomains= this->getSubdomains();
    Subdomain *theSub;
    while((theSub= theSubdomains()) != 0)
      {
        bool res= theSub->hasNode(nodeTag);
        if(res == true)
          return theSub->addSFreedom_Constraint(load, pattern);
      }

    // if no subdomain .. node not in model .. error message and return failure
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; cannot add as node with tag: "
	      << nodeTag << " does not exist in model\n";
    return false;
  }

//! Adds the multiple freedom constraint pointed to by the argument.
//!
//! Adds the multiple freedom constraint pointed to by the argument, to the
//! domain. The domain performs some checks is \p check is true. If
//! successful the domain adds the constraint using {\em
//! Domain::addMP\_Constraint()}. The call returns \p false if
//! the constraint was not added, otherwise \p true is returned.  
bool XC::PartitionedDomain::addMFreedom_Constraint(MFreedom_Constraint *load)
  {
    bool res = false;
    bool getRetained = false;
    bool addedMain = false;

    // to every domain with the constrained node we must
    // 1. add the retained node if not already there & any sp constraints on that node
    // 2. add the constraint.

    int retainedNodeTag = load->getNodeRetained();
    int constrainedNodeTag = load->getNodeConstrained();

    //
    // first we check the main domain
    // if has the constrained but not retained we mark as needing retained
    //

    Node *retainedNodePtr = this->Domain::getNode(retainedNodeTag);
    Node *constrainedNodePtr = this->Domain::getNode(constrainedNodeTag);
    if(constrainedNodePtr)
      {
	if (retainedNodePtr != 0)
	  {
	    res = this->Domain::addMFreedom_Constraint(load);    
	    if (res == false)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; problems adding to main domain.\n";
		return res;
	      }
	    else
	      { addedMain = true; } 
	  }
	else
	  { getRetained = true; }
      }

    //
    // now we check all subdomains
    // if a subdomain has both nodes we add the constraint, if only the
    // constrained node we mark as needing retained
    //

    SubdomainIter &theSubdomains = this->getSubdomains();
    Subdomain *theSub;
    while((theSub = theSubdomains())!=nullptr)
      {
        bool hasConstrained = theSub->hasNode(constrainedNodeTag);
        if(hasConstrained == true)
	  {
  	    bool hasRestrained = theSub->hasNode(retainedNodeTag);

	    if(hasRestrained == true)
	      {
                res = theSub->addMFreedom_Constraint(load);

	        if(res == false)
		  {
	            std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; problems adding to subdomain with retained.\n";
	            return res;
	          }
		else
		  {;}
	      }
	    else
	      { getRetained = true; }
          }
      }

    //
    // if getRetained is true, a subdomain or main domain has the constrained node
    // but no retained node .. we must go get it && SP constraints as well
    // 1. we first go get it
    // 2. then we add to main domain
    // 3. then we add to any subdomain
    // 
    if(getRetained == true)
      {
	// we get a copy of the retained Node, set mass equal 0 (don't want to double it)
	if (retainedNodePtr == 0)
	  {
	    SubdomainIter &theSubdomains2 = this->getSubdomains();
	    while ((theSub = theSubdomains2()) != 0 && retainedNodePtr == 0)
	      {
		bool hasRestrained = theSub->hasNode(retainedNodeTag);

		if (hasRestrained == true)
		  {
		    retainedNodePtr = theSub->getNode(retainedNodeTag);

		    Matrix mass(retainedNodePtr->getNumberDOF(), retainedNodePtr->getNumberDOF());
		    mass.Zero();
		    retainedNodePtr->setMass(mass);
		  }
	      }
	  }
	else
	  {
	    // get a copy & zero the mass
	    retainedNodePtr = new Node(*retainedNodePtr, false);
	  }

	if(retainedNodePtr == nullptr)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; can't find retained node anywhere!\n";
	    return false;
	  }
	else
	  {;}

	//
	// if main has it we add the retained to main & constraint
	//

	if (constrainedNodePtr != 0 && addedMain == false)
	  {
	    res = this->Domain::addNode(retainedNodePtr);

	    if(res == false)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; problems adding retained to main domain.\n";
		return res;
	      } 
	    res = this->Domain::addMFreedom_Constraint(load);

	    if(res == false)
	      {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; problems adding constraint to main domain after adding node.\n";
		return res;
	      } 
	  }

	//
	// to subdmains that have the constrained but no retained
	// 1. we add a copy of retained
	// 2. we add the constraint
	//

	SubdomainIter &theSubdomains3 = this->getSubdomains();
	while((theSub = theSubdomains3()) != nullptr)
	  {
	    bool hasConstrained = theSub->hasNode(constrainedNodeTag);
	    if (hasConstrained == true)
	      {
		bool hasRestrained = theSub->hasNode(retainedNodeTag);
		if (hasRestrained == false)
		  {
		    res = theSub->addExternalNode(retainedNodePtr);

		    if (res == false)
		      {
			std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; problems adding retained to subdomain.\n";
			return res;
		      } 

		    res = theSub->addMFreedom_Constraint(load);

		    if(res == false)
		      {
			std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; problems adding constraint to subdomain"
			          << " after adding node.\n";
			return res;
		      } 
		  }
	      }
	  }
	// clean up memory
	if (addedMain == true && getRetained == true) 
	  delete retainedNodePtr;
      }
    return res;
  }

bool XC::PartitionedDomain::addMRMFreedom_Constraint(MRMFreedom_Constraint *)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "not implemented yet." << std::endl;
    return false;
  }

//! @brief Add the load pattern to the domain.
bool XC::PartitionedDomain::addLoadPattern(LoadPattern *loadPattern)
  {
    bool result= true;

    int tag= loadPattern->getTag();
    if(this->getConstraints().getLoadPattern(tag) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; cannot add as LoadPattern with tag"
		  << tag << "already exists in model.\n";
        return false;
      }

    SubdomainIter &theSubdomains= this->getSubdomains();
    Subdomain *theSub;
    while((theSub= theSubdomains()) != 0)
      {
        bool res= theSub->addLoadPattern(loadPattern);
        if(res != true)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; cannot add as LoadPattern with tag: "
		      << tag << " to subdomain.\n";
            result= res;
          }
      }
    XC::Domain::addLoadPattern(loadPattern);
    return result;
  }


bool XC::PartitionedDomain::addNodalLoad(NodalLoad *load, int pattern)
  {
    int nodeTag= load->getNodeTag();

    // check the XC::Node exists in the XC::Domain or one of Subdomains

    // if in XC::Domain add it as external .. ignore Subdomains
    Node *nodePtr= this->getNode(nodeTag);
    if(nodePtr != 0)
      { return (this->XC::Domain::addNodalLoad(load, pattern)); }


    // find subdomain with node and add it .. break if find as internal node
    SubdomainIter &theSubdomains= this->getSubdomains();
    Subdomain *theSub;
    while((theSub= theSubdomains()) != 0)
      {
        bool res= theSub->hasNode(nodeTag);
        if(res)
         {
          // std::cerr << getClassName() << "::" << __FUNCTION__
	  //           << "(LoadPattern *loadPattern) SUB " << theSub->getTag() << *load;
          return theSub->addNodalLoad(load, pattern);
         }
      }
    // if no subdomain .. node not in model
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; cannot add as node with tag: "
              << nodeTag << " does not exist in model.\n";
    return false;
  }


bool XC::PartitionedDomain::addElementalLoad(ElementalLoad *load, int pattern)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return false;
  }

//! @brief Remove the element whose tag is given by \p tag.
//!
//! To remove the element whose tag is given by \p tag from the
//! domain. The method Returns \f$0\f$ if no such element exists in the domain.
//! Otherwise the domain invokes {\em setDomain(0)} on the element and {\em
//! setDomainChange(true,true,false)} on itself before a pointer to the
//! element is returned.
bool XC::PartitionedDomain::removeElement(int tag)
  {
    // we first see if its in the original domain
    bool res= elements->removeComponent(tag);
    if(res)
      this->domainChange();
    else
      {
        // if not there we must check all the other subdomains
        if(theSubdomains)
          {
            ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
            TaggedObject *theObject;
            while((theObject= theSubsIter()) != 0)
              {
                Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
                res= theSub->removeElement(tag);
                break;
              }
          }
      }
    return res;
  }


//! @brief Remove the node whose tag is given by \p tag.
//!
//! To remove the node whose tag is given by \p tag from the domain. 
//! Returns \f$0\f$ if no such node exists in the domain. Otherwise
//! if the \p checkNeeded is \p true before the node is removed a
//! check is made to see that the node is not referenced by any element,
//! constraint or load. If it is referenced the Node will not be
//! removed and \f$0\f$ is returned. If the node is to be removed the domain
//! invokes {\em setDomain(0)} on the node and {\em
//! setDomainChange(true,false,true)} on itself.
bool XC::PartitionedDomain::removeNode(int tag)
  {
    // we first remove it form the original domain (in case on boundary)
    bool result= this->Domain::removeNode(tag);

    // we must also try removing from the subdomains
    if(theSubdomains)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            result= theSub->removeNode(tag);
            if(result) break;
          }
      }
    if(result)
      this->domainChange();
    return result;
  }
//! @brief To remove the SFreedom\_Constraint whose tag is given by \p tag from
//! the domain.
//!
//! To remove the SFreedom\_Constraint whose tag is given by \p tag from the
//! domain. Returns \f$0\f$ if the constraint was not in the domain, otherwise
//! the domain invokes {\em setDomain(0)} on the constraint and {\em
//! setDomainChange(true,false,false)} on itself before a pointer to the
//! constraint is returned.  
bool XC::PartitionedDomain::removeSFreedom_Constraint(int tag)
  {
    // we first see if its in the original domain
    bool retval= this->Domain::removeSFreedom_Constraint(tag);
    if(retval)
      this->domainChange();
    else if(theSubdomains)  // if not there we must check all the other subdomains
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject= nullptr;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            retval= theSub->removeSFreedom_Constraint(tag);
            if(retval)
              break;
          }
      }
    return retval;
  }

//! To remove the MFreedom\_Constraint whose tag is given by \p tag.
//!
//! To remove the MFreedom\_Constraint whose tag is given by \p tag from the
//! domain. Returns \f$0\f$ if the constraint was not in the domain, otherwise
//! the domain invokes {\em setDomain(0)} on the constraint and {\em
//! setDomainChange(true,false,false)} on itself before a pointer to the
//! constraint is returned.  
bool XC::PartitionedDomain::removeMFreedom_Constraint(int tag)
  {
    // we first see if its in the original domain
    bool result= this->XC::Domain::removeMFreedom_Constraint(tag);
    if(result)
      {
        this->domainChange();
      }
    else
      {
        // if not there we must check all the other subdomains
        if(theSubdomains)
          {
            ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
            TaggedObject *theObject;
            while((theObject= theSubsIter()) != 0)
              {
                Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
                result= theSub->removeMFreedom_Constraint(tag);
                if(result) break;
              }
          }
      }
    return result;
  }


//! @brief To remove the load case whose tag is given by \p tag. 
//!
//! To remove the load case whose tag is given by \p tag from the domain. 
//! Returns \f$0\f$ if the load case was not in the domain, otherwise
//! returns a pointer to the load case that was removed. Invokes {\em
//! setDomain(0)} on the load case before it is returned.
bool XC::PartitionedDomain::removeLoadPattern(int tag)
  {
    // we first see if its in the original domain
    bool result= this->Domain::removeLoadPattern(tag);

    // we must also try removing from the subdomains
    if(theSubdomains)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            bool res= theSub->removeLoadPattern(tag);
            if(res)
              result= res;
          }
      }

    if(result)
      this->domainChange();
    return result;
  }

// public member functions which have to be modified

//! @brief Return an iterator to the element container.
//!
//! It returns an \p PartionedDomEleIter for the elements of the domain. This
//! is an iter which goes through all the subdomains, invoking {\em
//! getElements()} on the subdomain to get an ElementIter. The
//! PartitionedDomEleIter uses this iter to go through the elements of the
//! subdomain until it begins returning \f$0\f$; at which point it goes on to
//! the next subdomain.
XC::ElementIter &XC::PartitionedDomain::getElements()
  {
    theEleIter->reset();
    return *theEleIter;
  }

//! @brief Return the element identified by the argument.
//!
//! Returns a pointer to the element whose tag is given by \p tag. If
//! no such element exists \f$0\f$ is returned. This is done by invoking {\em
//! getElement(tag)} on the subdomains until the element is found or no
//! more subdomains exist; in which case a \f$0\f$ is returned.
XC::Element  *XC::PartitionedDomain::getElement(int tag)
  {
    // we first see if its in the original domain
    TaggedObject *res= elements->getComponentPtr(tag);
    Element *result= nullptr;
    if(res)
      {
        result= dynamic_cast<Element *>(res);
        return result;
      }

    // go through the other subdomains until we find it or we run out of subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            result= theSub->getElement(tag);
            if(result != 0)
                return result;
          }
      }
    // its not there
    return 0;
  }

//! @brief Return the number of elements.
int XC::PartitionedDomain::getNumElements(void) const
  {
    int result= elements->getNumComponents();

    // add the number of subdomains
    result +=  theSubdomains->getNumComponents();
    return result;
  }

//! @brief Applies the load to all the subdomains.
//!
//! The partioned domain iterates through all the subdomains invoking {\em
//! applyLoad(double timeStamp)} on them.
void XC::PartitionedDomain::applyLoad(double timeStep)
  {
    this->XC::Domain::applyLoad(timeStep);

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->applyLoad(timeStep);
          }
      }
  }


void XC::PartitionedDomain::setCommitTag(int newTag)
{
    this->XC::Domain::setCommitTag(newTag);

    // do the same for all the subdomains
    if(theSubdomains != 0) {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0) {
          Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->setCommitTag(newTag);
        }
    }
}



//! @brief Sets the current pseudo-time of the domain.
//!
//! Sets the current pseudo-time of the domain to be that whose tag is given
//! by \p newTime. It iterates through all the subdomains invoking the
//! same operation on them.
void XC::PartitionedDomain::setCurrentTime(double newTime)
  {
    this->XC::Domain::setCurrentTime(newTime);

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->setCurrentTime(newTime);
          }
      }
  }


void XC::PartitionedDomain::setCommittedTime(double newTime)
{
    this->XC::Domain::setCommittedTime(newTime);

    // do the same for all the subdomains
    if(theSubdomains != 0) {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0) {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->setCommittedTime(newTime);
        }
    }
}


void XC::PartitionedDomain::setLoadConstant(void)
{
    this->XC::Domain::setLoadConstant();

    // do the same for all the subdomains
    if(theSubdomains != 0) {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0) {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->setLoadConstant();
        }
    }
}


int XC::PartitionedDomain::update(void)
  {
    const int res= this->XC::Domain::update();
    if(res != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; domain failed in update\n";

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->computeNodalResponse();
            theSub->update();
          }
      }
#ifdef _PARALLEL_PROCESSING
    return this->barrierCheck(res);
#endif
    return 0;
  }




#ifdef _PARALLEL_PROCESSING
int XC::PartitionedDomain::barrierCheck(int res)
{
  int result= res;

  // do the same for all the subdomains
  if(theSubdomains != 0) {
    ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
    TaggedObject *theObject;
    while((theObject= theSubsIter()) != 0) {
      Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
      int subResult= theSub->barrierCheckIN();
      if(subResult != 0)
        result= subResult;
    }

    ArrayOfTaggedObjectsIter theSubsIter1(*theSubdomains);
    while((theObject= theSubsIter1()) != 0) {
      Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
      theSub->barrierCheckOUT(result);
    }
  }

  return result;
}
#endif

int XC::PartitionedDomain::update(double newTime, double dT)
  {
    this->applyLoad(newTime);
    const int res= this->XC::Domain::update();
    if(res != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "(newTime,dT) - domain failed in update\n";

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            theSub->computeNodalResponse();
            theSub->update(newTime, dT);
          }
      }
#ifdef _PARALLEL_PROCESSING
  return this->barrierCheck(res);
#endif
  return 0;
  }


int XC::PartitionedDomain::newStep(double dT)
  {
    Domain::newStep(dT);

    int res= 0;
    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            res += theSub->newStep(dT);
            if(res != 0)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; subdomain " << theSub->getTag()
			<< " failed in step\n";
          }
      }
    return res;
  }

//! @brief The partioned domain iterates through all the subdomains invoking
//! {\em commit()} on them.
int XC::PartitionedDomain::commit(void)
  {
    int result= this->XC::Domain::commit();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; failed in Domain::commit().\n";
        return result;
      }

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
	TaggedObject *theObject;
	while((theObject= theSubsIter()) != 0)
	  {
	    Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
	    int res= theSub->commit();
	    if(res < 0)
	      {
	        std::cerr << getClassName() << "::" << __FUNCTION__
		          << "(void); failed in Subdomain::commit().\n";
	        return res;
	      }
	  }
      }

    // now we load balance if we have subdomains and a partitioner
    int numSubdomains= this->getNumSubdomains();
    if(numSubdomains != 0 && theDomainPartitioner != 0)
      {
        Graph &theSubGraphs= this->getSubdomainGraph();
        theDomainPartitioner->balance(theSubGraphs);
      }
    return 0;
  }


int
XC::PartitionedDomain::revertToLastCommit(void)
{
    int result= this->XC::Domain::revertToLastCommit();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(void); failed in Domain::revertToLastCommit().\n";
        return result;
      }

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            int res= theSub->revertToLastCommit();
            if(res < 0)
	      {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "(void); "
		          << "failed in Subdomain::revertToLastCommit().\n";
                return res;
              }
          }
      }

    return 0;
  }

int XC::PartitionedDomain::revertToStart(void)
  {
    int result= this->XC::Domain::revertToStart();
    if(result < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(void) - failed in Domain::revertToLastCommit().\n";
        return result;
      }

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            int res= theSub->revertToStart();
            if(res < 0)
	      {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "(void); "
			  << "failed in Subdomain::revertToLastCommit().\n";
                return res;
            }
        }
    }

    return 0;
}


int XC::PartitionedDomain::addRecorder(Recorder &theRecorder)
  {
    if(this->XC::Domain::addRecorder(theRecorder) < 0)
      return -1;

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            int res= theSub->addRecorder(theRecorder);
            if(res < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "(void); "
			  << "failed in Subdomain::revertToLastCommit().\n";
                return res;
              }
          }
      }
    return 0;
  }

int XC::PartitionedDomain::removeRecorders(void)
  {
    if(this->XC::Domain::removeRecorders() < 0)
      return -1;

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
	  {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
            const int res= theSub->removeRecorders();
            if(res < 0)
	      {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "(void); "
			  << "failed in Subdomain::revertToLastCommit().\n";
                return res;
              }
          }
      }
    return 0;
  }

void XC::PartitionedDomain::Print(std::ostream &s, int flag)
  {
    XC::Domain::Print(s, flag);

    s << "\nELEMENT DATA: NumEle: " << elements->getNumComponents() << "\n";
    elements->Print(s);

    // print all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          { theObject->Print(s, flag); }
      }
  }


int XC::PartitionedDomain::setPartitioner(DomainPartitioner *thePartitioner)
  {
    theDomainPartitioner= thePartitioner;
    return 0;
  }

//! @brief Triggers the partition of the domain.
//! 
//! Method which first checks that subdomains with tags 1 through \p
//! numPartitions exist in the PartitionedDomain. Then it invokes
//! {\em setPartitionedDomain(*this)} on the DomainPartitioner
//! and finally it returns the result of invoking {\em partition(numPartitions}
//! on the DomainPartitioner, which will return 0 if succesfull, a negative
//! number if not.
int XC::PartitionedDomain::partition(int numPartitions, bool usingMain, int mainPartitionID)
  {
    int result= 0;
    // We must create element graph before the creation of new subdomains.
    // DO NOT REMOVE THIS LINE __ EVEN IF COMPILER WARNS ABOUT UNUSED VARIABLE
    //const Graph &theEleGraph= getElementGraph();
    getElementGraph(); //But this is Ok, isn't it?

    // now we call partition on the domainPartitioner which does the partitioning
    DomainPartitioner *thePartitioner= this->getPartitioner();
    if(thePartitioner != 0)
      {
        thePartitioner->setPartitionedDomain(*this);
        result=  thePartitioner->partition(numPartitions, usingMain, mainPartitionID);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(int numPartitions); no associated partitioner.\n";
        return -1;
      }

    //
    // add recorder objects
    //

    // do the same for all the subdomains
    if(theSubdomains != 0)
      {
        ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
        TaggedObject *theObject;
        while((theObject= theSubsIter()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);

            for(recorder_iterator irec= recorder_begin();irec!=recorder_end();irec++)
              {
                int res= theSub->addRecorder(**irec);
                if(res != 0)
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "(void); "
			      << "failed in Subdomain::revertToLastCommit().\n";
                    return res;
                  }
              }
          }
      }
    return result;
  }

//! @brief Adds the subdomain pointed to by theSubdomainPtr to the domain.
//!
//! Adds the subdomain pointed to by theSubdomainPtr to the domain. The domain
//! is responsible for checking that no other subdomain with a similar tag,
//! has been previously added to the domain. If successfull
//! the domain is responsible for invoking {\em setDomain(this)} on the
//! Subdomain. The domain is also responsible for invoking {\em
//! domainChange()}. The call returns \p false if the subdomain was not added, 
//! otherwise \p true is returned.  
bool XC::PartitionedDomain::addSubdomain(Subdomain *theSubdomain)
  {
    int eleTag= theSubdomain->getTag();
    TaggedObject *other= theSubdomains->getComponentPtr(eleTag);
    if(other != 0)
        return false;

    bool result= theSubdomains->addComponent(theSubdomain);
    if(result == true)
      {
        theSubdomain->setDomain(this);
        this->domainChange();
      }
    return result;
  }

//! @brief Return the number of Subdomains (partitions).
int XC::PartitionedDomain::getNumSubdomains(void)
  {
    return theSubdomains->getNumComponents();
  }

//! @brief Return the Subdomain whose tag is given by \p tag.
XC::Subdomain *XC::PartitionedDomain::getSubdomainPtr(int tag)
  {
    TaggedObject *mc= theSubdomains->getComponentPtr(tag);
    if(mc == 0) return 0;
    Subdomain *result= dynamic_cast<Subdomain *>(mc);
    return result;
  }

//! @brief Return an iterator for the Subdomains of the PartitionedDomain.
XC::SubdomainIter &XC::PartitionedDomain::getSubdomains(void)
  {
    theSubdomainIter->reset();
    return *theSubdomainIter;
  }


//! @brief Return a pointer to the DomainPartitioner object associated with the
//! PartitionedDomain.
XC::DomainPartitioner *XC::PartitionedDomain::getPartitioner(void) const
  { return theDomainPartitioner; }




//! @brief A method which will cause the domain to discard the current element
//! graph and build a new one based on the element connectivity.
//! 
//! A method which will cause the domain to discard the current element
//! graph and build a new one based on the element connectivity. Returns
//! \f$0\f$ if successfull otherwise \f$-1\f$ is returned along with an error
//! message to std::cerr.
int XC::PartitionedDomain::buildEleGraph(Graph &theEleGraph)
  {
    const int numVertex= elements->getNumComponents();

    // see if quick return

    if(numVertex == 0)
      return 0;

    // create another vertices array which aids in adding edges

    int maxEleNum= 0;

    TaggedObject *tagdObjPtr;
    TaggedObjectIter &theEles= elements->getComponents();
    while((tagdObjPtr= theEles()) != 0)
        if(tagdObjPtr->getTag() > maxEleNum)
            maxEleNum= tagdObjPtr->getTag();

    std::vector<int> theElementTagVertices(maxEleNum+1);

    for(int j=0; j<=maxEleNum; j++)
      theElementTagVertices[j]= -1;

    // now create the vertices with a reference equal to the element number.
    // and a tag which ranges from 0 through numVertex-1

    TaggedObjectIter &theEles2= elements->getComponents();

    int count= START_VERTEX_NUM;
    while((tagdObjPtr= theEles2()) != 0)
      {
        int ElementTag= tagdObjPtr->getTag();
        Vertex newVertex(count,ElementTag);


        theEleGraph.addVertex(newVertex);
        theElementTagVertices[ElementTag]= count++;
      }

    // We now need to determine which elements are asssociated with each node.
    // As this info is not in the XC::Node interface we must build it; which we
    // do using vertices for each node, when we addVertex at thes nodes we
    // will not be adding vertices but element tags.

    int maxNodNum= 0;
    Node *nodPtr;
    NodeIter &nodeIter= this->getNodes();
    while((nodPtr= nodeIter()) != 0)
        if(nodPtr->getTag() > maxNodNum)
            maxNodNum= nodPtr->getTag();

    std::vector<Vertex> theNodeTagVertices(maxNodNum+1);

    // now create the vertices with a reference equal to the node number.
    // and a tag which ranges from 0 through numVertex-1 and placed in
    // theNodeTagVertices at a position equal to the node's tag.

    NodeIter &nodeIter2= this->getNodes();
    count= START_VERTEX_NUM;
    while((nodPtr= nodeIter2()) != 0)
      {
        const int nodeTag= nodPtr->getTag();
        Vertex vrt(count++,nodeTag);
        theNodeTagVertices[nodeTag]= vrt;
      }

    // now add the the Elements to the nodes
    Element *elePtr= nullptr;
    TaggedObjectIter &theEles3= elements->getComponents();

    while((tagdObjPtr= theEles3()) != 0)
      {
        elePtr= dynamic_cast<Element *>(tagdObjPtr);
        int eleTag= elePtr->getTag();
        const ID &id= elePtr->getNodePtrs().getExternalNodes();

        int size= id.Size();
        for(int i=0; i<size; i++)
          theNodeTagVertices[id(i)].addEdge(eleTag);
      }

    // now add the edges to the vertices of our element graph;
    // this is done by looping over the Node vertices, getting their
    // Adjacenecy and adding edges between elements with common nodes
    for(int k=0; k<=maxNodNum; k++)
      {
        const std::set<int> &id= theNodeTagVertices[k].getAdjacency();
        for(std::set<int>::const_iterator i=id.begin(); i!=id.end(); i++)
          {
            const int Element1= *i;
            const int vertexTag1= theElementTagVertices[Element1];
            for(std::set<int>::const_iterator j=id.begin(); j!=id.end(); j++)
              if(i!=j)
                {
                  const int Element2= *j;
                  const int vertexTag2= theElementTagVertices[Element2];
                  // addEdge() adds for both vertices - do only once
                  if(vertexTag1 > vertexTag2)
                    theEleGraph.addEdge(vertexTag1,vertexTag2);
                  theEleGraph.addEdge(vertexTag2,vertexTag1);
                }
          }
      }
    return 0;
  }



//! @brief a method which will only remove a node from the partitioned domain
//! it does not touch the subdomains .. can be dangerous to use.
//!
//! A method to remove a Node whose tag is given by \p tag from the
//! PartitionedDomain, but will not remove the Node from any Subdomains.
bool XC::PartitionedDomain::removeExternalNode(int tag)
  {
    return (this->Domain::removeNode(tag));
  }

//! This will create a new graph each time it is invoked; deleting the old
//! graph. THIS WILL BE CHANGED.  A vertex is created for each Subdomain, with
//! an edge to each Subdomain the Subdomain is connected to, a tag equal to the
//! Subdomain tag,  and a weight equal to the result of invoking getCost() on
//! the Subdomain.
XC::Graph &XC::PartitionedDomain::getSubdomainGraph(void)
  {
    // delete the old always - only object that will
    // use this is a DomainBalancer & it is always looking for latest

    mySubdomainGraph= Graph(this->getNumSubdomains()+START_VERTEX_NUM);

    int numVertex= theSubdomains->getNumComponents();

    // see if quick return

    if(numVertex!=0)
      {
        // create another vertices array which aids in adding edges

        int maxEleNum= 0;
        TaggedObject *tagdObjPtr= nullptr;
        TaggedObjectIter &theEles= theSubdomains->getComponents();
        while((tagdObjPtr= theEles()) != 0)
          if(tagdObjPtr->getTag() > maxEleNum)
            maxEleNum= tagdObjPtr->getTag();

        std::vector<int> theElementTagVertices(maxEleNum+1);

        for(int j=0; j<=maxEleNum; j++)
          theElementTagVertices[j]= -1;

        // now create the vertices with a reference equal to the subdomain number.
        // and a tag equal to the subdomain number and a weighed according to
        // the subdomain cost

        TaggedObjectIter &theEles2= theSubdomains->getComponents();

        while((tagdObjPtr= theEles2()) != 0)
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(tagdObjPtr); // upward cast ok as
                                                 // only subdomais can be added
            int ElementTag= tagdObjPtr->getTag();

            Vertex vrt(ElementTag, ElementTag, theSub->getCost());
            mySubdomainGraph.addVertex(vrt);
            theElementTagVertices[ElementTag]= ElementTag;
         }

        // We now need to determine which theSubdomains are asssociated with each node.
        // As this info is not in the XC::Node interface we must build it; which we
        // do using vertices for each node, when we addVertex at thes nodes we
        // will not be adding vertices but element tags.

        int maxNodNum= 0;
        Node *nodPtr= nullptr;
        NodeIter &nodeIter= this->getNodes();
        while((nodPtr= nodeIter()) != 0)
          if(nodPtr->getTag() > maxNodNum)
            maxNodNum= nodPtr->getTag();

        std::vector<Vertex> theNodeTagVertices(maxNodNum+1);


        // now create the vertices with a reference equal to the node number.
        // and a tag which ranges from 0 through numVertex-1 and placed in
        // theNodeTagVertices at a position equal to the node's tag.

        NodeIter &nodeIter2= this->getNodes();
        int count= START_VERTEX_NUM;
        while((nodPtr= nodeIter2()) != 0)
          {
            const int nodeTag= nodPtr->getTag();
            Vertex vrt(count++,nodeTag);
            theNodeTagVertices[nodeTag]= vrt;
          }

        // now add the the TheSubdomains to the nodes
        Element *elePtr= nullptr;
        TaggedObjectIter &theEles3= theSubdomains->getComponents();

        while((tagdObjPtr= theEles3()) != 0)
          {
            elePtr= (Element *)tagdObjPtr;
            const int eleTag= elePtr->getTag();
            const ID &id= elePtr->getNodePtrs().getExternalNodes();

            const int size= id.Size();
            for(int i=0; i<size; i++)
              theNodeTagVertices[id(i)].addEdge(eleTag);
          }

        // now add the edges to the vertices of our element graph;
        // this is done by looping over the Node vertices, getting their
        // Adjacency and adding edges between theSubdomains with common nodes

        for(int k=0; k<=maxNodNum; k++)
          {
            const std::set<int> &id= theNodeTagVertices[k].getAdjacency();
            for(std::set<int>::const_iterator i=id.begin(); i!=id.end(); i++)
              {
                const int Element1= *i;
                const int vertexTag1= theElementTagVertices[Element1];
                for(std::set<int>::const_iterator j=id.begin(); j!=id.end(); j++)
                  if(i!=j)
                    {
                      const int Element2= *j;
                      const int vertexTag2= theElementTagVertices[Element2];
                      // addEdge() adds for both vertices - do only once
                      if(vertexTag1 > vertexTag2)
                        mySubdomainGraph.addEdge(vertexTag1,vertexTag2);
                      mySubdomainGraph.addEdge(vertexTag2,vertexTag1);
                    }
              }
          }
      }
    return mySubdomainGraph;
  }


double XC::PartitionedDomain::getNodeDisp(int nodeTag, int dof, int &errorFlag)
  {
    double result= this->XC::Domain::getNodeDisp(nodeTag, dof, errorFlag);

    if(errorFlag != 0)
      {
	// do the same for all the subdomains
	if(theSubdomains != 0)
	  {
	    ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
	    TaggedObject *theObject;
	    while((theObject= theSubsIter()) != 0 && errorFlag != 0)
	      {
		Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
		result= theSub->getNodeDisp(nodeTag, dof, errorFlag);
		if(errorFlag == 0)
		  return result;
	      }
	  }
      }
    return result;
  }


int XC::PartitionedDomain::setMass(const Matrix &mass, int nodeTag)
  {
    int result= Domain::setMass(mass, nodeTag);
    if(result != 0)
      {
        // do the same for all the subdomains
        if(theSubdomains != 0)
          {
            ArrayOfTaggedObjectsIter theSubsIter(*theSubdomains);
            TaggedObject *theObject;
            while((theObject= theSubsIter()) != 0 && result != 0)
              {
                Subdomain *theSub= dynamic_cast<Subdomain *>(theObject);
                result= theSub->setMass(mass, nodeTag);
              }
          }
      }
    return result;
  }
