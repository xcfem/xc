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
// $Date: 2006/01/10 18:17:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/partitioner/DomainPartitioner.cpp,v $

// Written: fmk
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::DomainPartitioner.
// A XC::DomainPartitioner is an object used to partition a XC::PartitionedDomain.
//
// What: "@(#) DomainPartitioner.C, revA"

#include <domain/partitioner/DomainPartitioner.h>

#include <cstdlib>
#include "solution/graph/partitioner/GraphPartitioner.h"
#include <domain/domain/partitioned/PartitionedDomain.h>
#include "domain/partitioner/loadBalancer/LoadBalancer.h"
#include "domain/domain/subdomain/Subdomain.h"
#include <domain/domain/SubdomainIter.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <domain/constraints/MFreedom_ConstraintIter.h>
#include <domain/constraints/SFreedom_ConstraintIter.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include "solution/graph/graph/Graph.h"
#include <utility/matrix/Vector.h>
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/LoadPattern.h>

#include <utility/Timer.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "NodeLocations.h"

//! @brief Constructor.
//! 
//! Constructs a DomainPartitioner which will use {\em
//! theGraphPartitioner} to initially partition the PartitionedDomain. The
//! max number of subdomains that the Domain can be partitioned is
//! currently set at 8. 
XC::DomainPartitioner::DomainPartitioner(GraphPartitioner &theGraphPartitioner)
:myDomain(0),thePartitioner(theGraphPartitioner),theBalancer(0),
 theElementGraph(0), theBoundaryElements(0), theNodeLocations(0), elementPlace(0), numPartitions(0), partitionFlag(false), usingMainDomain(false)
  {}

//! @brief Constructor.
//! 
//! Constructs a DomainPartitioner which will use {\em
//! theGraphPartitioner} to initially partition the PartitionedDomain
//! using the element graph and the \p theLoadBalancer to load balance 
//! the PartitionedDomain. The max number of subdomains that the Domain can be 
//! partitioned is currently set at 8.
XC::DomainPartitioner::DomainPartitioner(GraphPartitioner &theGraphPartitioner,
                                 LoadBalancer &theLoadBalancer)
:myDomain(0),thePartitioner(theGraphPartitioner),theBalancer(&theLoadBalancer),
 theElementGraph(0), theBoundaryElements(0),theNodeLocations(0), elementPlace(0), numPartitions(0), partitionFlag(false), usingMainDomain(false)
  {
    // set the links the loadBalancer needs
    theLoadBalancer.setLinks(*this);
  }


int XC::DomainPartitioner::inic(const size_t &numParts)
  {
    theBoundaryElements= std::vector<Graph>(numParts,Graph(2048));
    return 0;
  }

void XC::DomainPartitioner::setPartitionedDomain(PartitionedDomain &theDomain)
  { myDomain = &theDomain; }

//! @brief Method invoked to partition the Domain.
//!
//! Method invoked to partition the Domain. It first checks to see that
//! the PartitionedDomain has at least \p numParts Subdomains, with tags
//! 1 through \p numParts; if not prints an error message and returns -1. 
//! It then asks the domain for the element graph. This graph is then
//! partitioned 
//! using the GraphPartitioner into \p numParts; if partitioning fails an error 
//! message is printed and  \f$-10 +\f$ number returned from GraphPartitioner is
//! returned. If successfull the domain is partitioned according to the
//! following rules: \begin{itemize}
//! \item All nodes which are internal to a partition are added using the
//! addNode() method of the Subdomain. These nodes are removed from
//! the PartionedDomain using removeNode(). 
//! \item External nodes (these are nodes shared across partitions as a
//! result of element connectivity or MP\_Constraints are added to those
//! Subdomains whose elements reference them. They are added using the
//! addExternalNode() command. 
//! \item SFreedom\_Constraints whose node is interior to a Subdomain are
//! removed from the PartitionedDomain and added to the Subdomain. 
//! \item MP\_Constraints whose two nodes are interior to a Subdomain are
//! removed from the PartitionedDomain and added to the Subdomain.
//! \item The elements are sent to the partition whose tag is given by the
//! color of the vertex in the partitioned (colored) element graph. The
//! elements are removed from the PartitionedDomain using {\em
//! removeElement()} and added to the Subdomain using addElement().
//! \item For the loads, a check is made to ensure that each Subdomain has
//! a LoadCase with a tag equal to the tags in the LoadCases that have
//! been added to the PartitionedDomain; if not new LoadCases are created
//! and added to the Subdomain. It then iterates through all the
//! NodalLoads in the LoadCases in the PartionedDomain, if the
//! corresponding node is external the NodalLoad is removed and added to
//! the corresponding LoadCase in the Subdomain. ELEMENTAL LOADS are not
//! yet dealt with. 
//! \end{itemize}
//! 
//! The DomainPartitioner invokes hasDomainChanged() on each Subdomain; if the
//! Subdomain has changed invokeChangeOnAnalysis(). Finally {\em
//! hasDomainChanged()} is invoked on the PartitionedDomain; if it has
//! changed invokeChangeOnAnalysis(). \p partitionFlag is set to
//! true.
int XC::DomainPartitioner::partition(int numParts, bool usingMain, int mainPartitionTag)
  {
    usingMainDomain = usingMain;
    mainPartition = mainPartitionTag;

    // first we ensure the partitioned domain has numpart subdomains
  // with tags 1 through numparts
    for(int i=1; i<=numParts; i++)
      {
        if(i != mainPartition)
          {
	     Subdomain *subdomainPtr = myDomain->getSubdomainPtr(i);
	     if(!subdomainPtr)
	       {
		 std::cerr << getClassName() << "::" << __FUNCTION__
			   << "; no subdomain: " << i << " exists.\n";
		 return -1;
	       }
          }
      }

    // we get the ele graph from the domain and partition it
    theElementGraph= &(myDomain->getElementGraph());

    int theError = thePartitioner.partition(*theElementGraph, numParts);
    if(theError < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the graph partioner failed to partition the "
                  << "element graph.\n";
        return -10+theError;
      }

    // we create empty graphs for the numParts subdomains,
    // in the graphs we place the vertices for the elements on the boundaries

    // we do not invoke the destructor on the individual graphs as
    // this would invoke the destructor on the individual vertices

    inic(numParts);

    numPartitions= numParts;

    // we now create a MapOfTaggedObjectStorage to store the NodeLocations
    // and create a new NodeLocation for each node; adding it to the map object
    theNodeLocations= new MapOfTaggedObjects(nullptr,"nodLocation");
    if(!theNodeLocations)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "(int numParts)"
                  << "; ran out of memory creating MapOfTaggedObjectStorage"
	          << " for node locations.\n";
        numPartitions = 0;
        return -1;
      }

    NodeIter &theNodes = myDomain->getNodes();
    Node *nodePtr;
    while((nodePtr = theNodes()) != 0)
      {
        NodeLocations *theNodeLocation= new NodeLocations(nodePtr->getTag());
        if(theNodeLocation == 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "(int numParts)"
                      << "; ran out of memory creating NodeLocation for node: "
                      << nodePtr->getTag() << std::endl;
            numPartitions = 0;
            return -1;
          }
        if(theNodeLocations->addComponent(theNodeLocation) == false)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "(int numParts)"
                      << "; failed to add NodeLocation to Map for Node: "
                      << nodePtr->getTag() << std::endl;
            numPartitions = 0;
            return -1;
          }
      }

    //
    // we now iterate through the vertices of the element graph
    // to see if the vertex is a boundary vertex or not - if it is
    // we add to the appropriate graph created above. We also set the
    // value the color variable of each of the external nodes connected
    // to the element to a value which will indicate that that node will
    // have to be added to the subdomain.
    //

    VertexIter &theVertexIter = theElementGraph->getVertices();
    Vertex *vertexPtr= nullptr;
    while((vertexPtr = theVertexIter()) != 0)
      {
        int eleTag = vertexPtr->getRef();
        int vertexColor = vertexPtr->getColor();

        const std::set<int> &adjacency= vertexPtr->getAdjacency();
        int size= adjacency.size();
        for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
          {
            Vertex *otherVertex = theElementGraph->getVertexPtr(*i);
            if(otherVertex->getColor() != vertexColor)
              {
                theBoundaryElements[vertexColor-1].addVertex(*vertexPtr,false);
                i= adjacency.end(); //break; //i = size;
              }
          }
        Element *elePtr = myDomain->getElement(eleTag);
        const ID &nodes = elePtr->getNodePtrs().getExternalNodes();
        size= nodes.Size();
        for(int j=0; j<size; j++)
          {
            int nodeTag = nodes(j);
            TaggedObject *theTaggedObject = theNodeLocations->getComponentPtr(nodeTag);
            if(theTaggedObject == 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "(int numParts)"
			  << "; failed to find NodeLocation in Map for Node: "
                          << nodePtr->getTag() << " -- A BUG!!\n";
                numPartitions = 0;
                return -1;
              }
            NodeLocations *theNodeLocation= dynamic_cast<NodeLocations *>(theTaggedObject);
            theNodeLocation->addPartition(vertexColor);
          }
      }

    // now go through the MFreedom_Constraints and ensure the retained node is in every
    // partition the constrained node is in
    MFreedom_ConstraintIter &theMPs = myDomain->getConstraints().getMPs();
    MFreedom_Constraint *mpPtr;
    while((mpPtr = theMPs()) != 0)
      {
        int retained = mpPtr->getNodeRetained();
        int constrained = mpPtr->getNodeConstrained();

        TaggedObject *theRetainedObject = theNodeLocations->getComponentPtr(retained);
        TaggedObject *theConstrainedObject = theNodeLocations->getComponentPtr(constrained);

        if(theRetainedObject == nullptr || theConstrainedObject == nullptr)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__;
            if(!theRetainedObject)
              std::cerr << " - failed to find NodeLocation in Map for Node: "
                        << retained << " -- A BUG!!\n";
            if(!theConstrainedObject)
              std::cerr << "; failed to find NodeLocation in Map for XC::Node: "
                        << constrained << " -- A BUG!!\n";
              numPartitions = 0;
            return -1;
          }

        NodeLocations *theRetainedLocation = (NodeLocations *)theRetainedObject;
        NodeLocations *theConstrainedLocation = (NodeLocations *)theConstrainedObject;
	const std::set<int> &theConstrainedNodesPartitions= theConstrainedLocation->nodePartitions;
        for(std::set<int>::const_iterator i= theConstrainedNodesPartitions.begin(); i!=theConstrainedNodesPartitions.end(); i++)
          { theRetainedLocation->addPartition(*i); }
      }

    // we now add the nodes,
    TaggedObjectIter &theNodeLocationIter = theNodeLocations->getComponents();
    TaggedObject *theNodeObject;
    while((theNodeObject = theNodeLocationIter()) != 0)
      {
        NodeLocations *theNodeLocation = (NodeLocations *)theNodeObject;
        int nodeTag = theNodeLocation->getTag();
	const std::set<int> &nodePartitions = theNodeLocation->nodePartitions;

        for(std::set<int>::const_iterator i= nodePartitions.begin(); i!=nodePartitions.end(); i++)
          {
            int partition = *i;
            if(partition != mainPartition)
              {
                Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
                Node *nodePtr = myDomain->getNode(nodeTag);
                if(numPartitions == 1)
                  {
                    theSubdomain->addNode(nodePtr->getCopy());
                    myDomain->removeNode(nodeTag);
                  }
                else
                  theSubdomain->addExternalNode(nodePtr);
              }
          }
      }
    // we now move the elements
    VertexIter &theVertices = theElementGraph->getVertices();
    while((vertexPtr = theVertices()) != 0)
      {
        // move the element
        int partition = vertexPtr->getColor();
        if(partition != mainPartition)
          {
            int eleTag = vertexPtr->getRef();
            Element *elePtr = myDomain->getElement(eleTag);
            Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
            theSubdomain->addElement(elePtr->getCopy());
            myDomain->removeElement(eleTag);
          }
      }

    // now we go through the load patterns and move NodalLoad
    // 1) make sure each subdomain has a copy of the partitioneddomains load patterns.
    // 2) move nodal loads
    // 3) move SFreedom_Constraints

    std::map<int,LoadPattern *> &theLoadPatterns = myDomain->getConstraints().getLoadPatterns();
    for(std::map<int,LoadPattern *>::iterator theLoadPattern= theLoadPatterns.begin();
        theLoadPattern!= theLoadPatterns.end();theLoadPattern++)
      {
        int loadPatternTag = theLoadPattern->second->getTag();

        // check that each subdomain has a loadPattern with a similar tag and class tag
        for(int i=1; i<=numParts; i++)
          {
            if(i != mainPartition)
              {
                Subdomain *theSubdomain = myDomain->getSubdomainPtr(i);
                LoadPattern *loadPatternCopy = theSubdomain->getConstraints().getLoadPattern(loadPatternTag);
                if(loadPatternCopy == 0)
                  {
                    LoadPattern *newLoadPattern = theLoadPattern->second->getCopy();
                    if(newLoadPattern == 0)
                      {
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; out of memory creating LoadPatterns.\n";
                        return -1;
                      }
                    theSubdomain->addLoadPattern(newLoadPattern);
                  }
              }
          }

        // now remove any nodal loads that correspond to internal nodes in a subdomain
        // and add them to the appropriate loadpattern in the subdomain

        NodalLoadIter theNodalLoads= theLoadPattern->second->getLoads().getNodalLoads();
        NodalLoad *theNodalLoad;
        while((theNodalLoad = theNodalLoads()) != 0)
          {
            int nodeTag = theNodalLoad->getNodeTag();

            TaggedObject *theTaggedObject = theNodeLocations->getComponentPtr(nodeTag);
            if(theTaggedObject == 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; failed to find node location in Map for node: "
                          << nodeTag << " -- A BUG!!\n";
                numPartitions = 0;
                return -1;
              }

            NodeLocations *theNodeLocation = (NodeLocations *)theTaggedObject;
	    const std::set<int> &nodePartitions = theNodeLocation->nodePartitions;
            for(std::set<int>::const_iterator i= nodePartitions.begin(); i!=nodePartitions.end(); i++)
              {
                int partition = *i;
                if(partition != mainPartition)
                  {
                    if(numPartitions == 1)
                      {
                        Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
                        theLoadPattern->second->removeNodalLoad(theNodalLoad->getTag());
                        if((theSubdomain->addNodalLoad(theNodalLoad, loadPatternTag)) != true)
                          std::cerr << getClassName() << "::" << __FUNCTION__
				    << "; failed to add nodal load.\n";
                      }
                  }
              }
         }
        SFreedom_ConstraintIter &theSPs = theLoadPattern->second->getSPs();
        SFreedom_Constraint *spPtr;
        while ((spPtr = theSPs()) != 0)
          {
            int nodeTag = spPtr->getNodeTag();
            TaggedObject *theTaggedObject = theNodeLocations->getComponentPtr(nodeTag);
            if(theTaggedObject == 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; failed to find NodeLocation in Map for node: "
                          << nodeTag << " -- A BUG!!\n";
                numPartitions = 0;
                return -1;
              }
            NodeLocations *theNodeLocation = (NodeLocations *)theTaggedObject;
            const std::set<int> &nodePartitions = theNodeLocation->nodePartitions;
            int numPartitions = theNodeLocation->numPartitions;
            for(std::set<int>::const_iterator i= nodePartitions.begin(); i!=nodePartitions.end(); i++)
              {
                int partition= *i;
                if(partition != mainPartition)
                  {
                    Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
                    if(numPartitions == 1)
                      theLoadPattern->second->removeSFreedom_Constraint(spPtr->getTag());
                    int res = theSubdomain->addSFreedom_Constraint(spPtr, loadPatternTag);
                    if(res < 0)
                      std::cerr << getClassName() << "::" << __FUNCTION__
				<< "; failed to add SP Constraint\n";
                  }
              }
          }
        ElementalLoadIter &theLoads = theLoadPattern->second->getLoads().getElementalLoads();
        ElementalLoad *theLoad;
        while((theLoad = theLoads()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; REMOVE ELEMENTAL LOADS\n";
            // std::cerr << getClassName() << "::" << __FUNCTION__
	    //           << "; REMOVE ELEMENTAL LOADS\n";
            //        if (theLoad->getElementTag() == eleTag)
            //          theLoadPattern->second->removeElementalLoad(theLoad->getTag());
            // theSubdomain->addElementalLoad(theLoad, loadPatternTag);
          }
      }

    // add the single point constraints,
    SFreedom_ConstraintIter &theDomainSP = myDomain->getConstraints().getSPs();
    SFreedom_Constraint *spPtr;
    while((spPtr = theDomainSP()) != 0)
      {
        int nodeTag = spPtr->getNodeTag();

        TaggedObject *theTaggedObject = theNodeLocations->getComponentPtr(nodeTag);
        if(theTaggedObject == 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "(int numParts)"
		      << " - failed to find NodeLocation in Map for Node: "
		      << nodeTag << " -- A BUG!!\n";
            numPartitions = 0;
            return -1;
          }

        NodeLocations *theNodeLocation = (NodeLocations *)theTaggedObject;
        const std::set<int> &nodePartitions = theNodeLocation->nodePartitions;
        for(std::set<int>::const_iterator i= nodePartitions.begin(); i!=nodePartitions.end(); i++)
          {
            int partition = *i;
            if(partition != mainPartition)
              {
                Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
                if(numPartitions == 1)
                  { myDomain->removeSFreedom_Constraint(spPtr->getTag()); }
                int res = theSubdomain->addSFreedom_Constraint(spPtr);
                if(res < 0)
                  std::cerr << getClassName() << "::" << __FUNCTION__
			    << "; failed to add SP Constraint.\n";
              }
          }
      }

    // move MFreedom_Constraints - add an XC::MFreedom_Constraint to every partition a constrained node is in
    MFreedom_ConstraintIter &moreMPs = myDomain->getConstraints().getMPs();
    while((mpPtr = moreMPs()) != 0)
      {
        int constrained = mpPtr->getNodeConstrained();
        TaggedObject *theConstrainedObject = theNodeLocations->getComponentPtr(constrained);
        NodeLocations *theConstrainedLocation = (NodeLocations *)theConstrainedObject;
        const std::set<int> &theConstrainedNodesPartitions = theConstrainedLocation->nodePartitions;
        for(std::set<int>::const_iterator i= theConstrainedNodesPartitions.begin(); i!= theConstrainedNodesPartitions.end(); i++)
          {
            int partition= *i;
            if(partition != mainPartition)
              {
                Subdomain *theSubdomain = myDomain->getSubdomainPtr(partition);
                if(numPartitions == 1)
                  myDomain->removeMFreedom_Constraint(mpPtr->getTag());
                int res = theSubdomain->addMFreedom_Constraint(mpPtr);
                if(res < 0)
                  std::cerr << getClassName() << "::" << __FUNCTION__
			    << "; failed to add MP Constraint.\n";
              }
          }
      }

    // now we go through all the subdomains and tell them to update
    // their analysis for the new_ layouts
    SubdomainIter &theSubDomains= myDomain->getSubdomains();
    Subdomain *theSubDomain;
    while((theSubDomain = theSubDomains()) != 0)
      theSubDomain->domainChange();

    // we invoke change on the XC::PartitionedDomain
    myDomain->domainChange();

    // we are done
    partitionFlag = true;
    return 0;
  }

//! Load balance.
//!
//! Checks first to see that the \p partitionFlag has been set; if it
//! hasn'nt an error message is printed and a \f$-1\f$ is returned. If a
//! LoadBalancer was passed in the constructor balance() is invoked
//! on this object; if no LoadBalancer was passed nothing is done and
//! method returns\f$0\f$. If balancing is performed, the DomainPartitioner
//! invokes hasDomainChanged() on each Subdomain; if the Subdomain
//! has changed invokeChangeOnAnalysis(). Finally {\em
//! hasDomainChanged()} is invoked on the PartitionedDomain; if it has
//! changed invokeChangeOnAnalysis(). \p partitionFlag is set to
//! true. 
//! 
//! Method which invokes {\em setPartitioner(this)} on the
//! LoadBalancingAlgo. It then invokes {\em balance(load)} on this
//! object, where \p load is vector of size \p numParts
//! containing the load of each subdomain. 
int XC::DomainPartitioner::balance(Graph &theWeightedPGraph)
  {
    int res = 0;

    // check that the object did the partitioning
    if (partitionFlag == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "(const Vector &load)"
		  << "; not partitioned or DomainPartitioner"
	          << " did not partition.\n";
        return -1;
      }

    if(theBalancer)
      {
        // call on the LoadBalancer to partition
        res = theBalancer->balance(theWeightedPGraph);

        // now invoke domainChanged on Subdomains and XC::PartitionedDomain
        SubdomainIter &theSubDomains = myDomain->getSubdomains();
        Subdomain *theSubDomain= nullptr;
        while ((theSubDomain = theSubDomains()) != 0)
          theSubDomain->domainChange();

        // we invoke change on the XC::PartitionedDomain
        myDomain->domainChange();
      }
    return res;
  }


//! @brief Returns the number of partitions in the PartitionedDomain.
int XC::DomainPartitioner::getNumPartitions(void) const
  { return numPartitions; }

//! @brief Return the partition graph.
//! 
//! Method which returns the partition graph. This is a graph with a
//! vertex for every partition and an edge between partitions if there
//! exists an element in one partition which is connected to an element
//! in the other partition.
XC::Graph &XC::DomainPartitioner::getPartitionGraph(void)
  {
    if(myDomain == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ERROR: no domain has been set.\n";
        exit(-1);
      }
    return myDomain->getSubdomainGraph();
  }

//! @brief Return the colored graph.
//! 
//! A method which returns the current colored graph representing the
//! partitioning of the elements in the subdomains. Does this by invoking
//! getElementGraph() on the PartitionedDomain. Note that this is
//! the same graph that was colored by the DomainPartitioner in
//! partitioning the PartitionedDomain. 
XC::Graph &XC::DomainPartitioner::getColoredGraph(void)
  {
    if(myDomain == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; ERROR: no domain has been set.";
        exit(0);
      }
    return myDomain->getElementGraph();
  }

//! @brief Moves a vertes from subdomain from to subdomain to.
//!
//! Method which will take the element given by vertex reference of the
//! vertex whose tag is given by \p vertexTag from subdomain \p from
//! and place it in subdomain \p to. If \p notAdjacentOther is {\em
//! true} a check is made to ensure that the vertex to be swapped is not
//! adjacent to a vertex in any other partition. Returns \f$0\f$ if
//! successfull, returns an error message and \f$-1\f$ if PartitionedDomain
//! has not been partitioned, \f$-2\f$ if \p from Subdomain does not exist,
//! \f$-3\f$ if {\em to Subdomain} does not exist, \f$-4\f$ if a vertex given by
//! \p tag does not exist, returns \f$-5\f$ if \p notAdjacentOther was
//! true and the vertex was adjacent to a vertex in another partition, and
//! returns \f$-6\f$ if no Element with a tag similar to \p tag exists
//! (this should not happen if element graph is built correctly).
//! The Element, Nodes, NodalLoads, SFreedom\_Constraints and MP\_Constraints
//! that need to be moved between the two Subdomains, or between the
//! PartitionedDomain and Subdomains are also moved. NO ELEMENTAL LOADS 
//! are moved yet.
int XC::DomainPartitioner::swapVertex(int from, int to, int vertexTag, bool adjacentVertexNotInOther)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "() " << from << " " << to << " " << vertexTag
	      << std::endl;

    /*
    // check that the object did the partitioning
    if (partitionFlag == false) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "(const XC::Vector &load)"
                << "; not partitioned or DomainPartitioner did not partition.\n";
      return -1;
    }

    // check that the subdomain exist in partitioned domain
    Subdomain *fromSubdomain = myDomain->getSubdomainPtr(from);
    if (fromSubdomain == 0) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no from XC::Subdomain: ";
      std::cerr << from << " exists\n";
      return -2;
    }
    Subdomain *toSubdomain = myDomain->getSubdomainPtr(to);
    if (toSubdomain == 0) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no to Subdomain: " << to << " exists.\n";
      return -3;
    }

    // remove the vertex from the boundary if there
    // if not there we have to get a pointer to it from the ele graph.

    //    Graph &theEleGraph = myDomain->getElementGraph();
    Graph *fromBoundary = theBoundaryElements[from-1];
    Graph *toBoundary = theBoundaryElements[to-1];
    Vertex *vertexPtr;

    // get a pointer to the vertex in the element graph
    if (adjacentVertexNotInOther  == false) {
      vertexPtr = fromBoundary->removeVertex(vertexTag,false);
      if (vertexPtr == 0)
	vertexPtr = theElementGraph->getVertexPtr(vertexTag);

      if (vertexPtr == 0)  // if still 0 no vertex given by tag exists
	return -4;
    } else { // get a pointer and check vertex not adjacent to another partition
      vertexPtr = fromBoundary->getVertexPtr(vertexTag);
      if (vertexPtr == 0)
	vertexPtr = theElementGraph->getVertexPtr(vertexTag);
      if (vertexPtr == 0)  // if still 0 no vertex given by tag exists
	return -4;
      const std::set<int> &adjacent= vertexPtr->getAdjacency();
      bool inTo = false;
      bool inOther = false;
      int adjacentSize = adjacent.size();
      for(std::set<int>::const_iterator i= adjacent.begin(); i!= adjacent.end(); i++)
	{
	Vertex *other = theElementGraph->getVertexPtr(*i);
	if (other->getColor() == to)
	  inTo = true;
	else if (other->getColor() != from) {
	  inOther = true;
	  i = adjacentSize;
	}
      }
      if (inTo != true || inOther == true) // we cannot remove the vertex
	return -5;
      // if can remove the vertex from the fromBoundary
      Vertex *theVertex = fromBoundary->removeVertex(vertexTag,false);
    }

    int eleTag = vertexPtr->getRef();

    //
    // in the FROM subdomain we:
    //  1. remove the element
    //  2. remove all nodes connected to the element divide color by prime
    //  3. see if have to add nodes back as external
    //  4. remove from the XC::PartitionedDomain if node was external
    //  5. add new_ elements to the boundary vertices for from

    //  1. remove the element from the fromSubdomain
      //          this gives us a pointer to the element as well.

      Element *elePtr = fromSubdomain->removeElement(eleTag);
      if (elePtr == 0) // if ele not there we can just return but ERROR it should be
    return -6;

      //  2. remove all nodes connected to the element divide color by prime
      //  3. add back if still in subdomain
      //  4. if node external remove from PartitionedDomains external nodes
      int primeFrom = primes(from);
      const XC::ID  &nodes1 = elePtr->getNodePtrs().getExternalNodes(); // create a copy, need after send ele
      ID  nodes(nodes1);

      int nodesSize = nodes.Size();
      std::vector<Node *> nodesArray(nodesSize);
      for (int i=0; i<nodesSize; i++) {
    int nodeTag = nodes(i);

    // remove the node
    Node *nodePtr = fromSubdomain->removeNode(nodeTag);

    (*nodePlace)(nodeTag) /= primeFrom;
    nodesArray[i] = nodePtr;
	  // std::cerr << getClassName() << "::" << __FUNCTION__
		       << "; NODE " << *nodePtr;
    // add back as external if still needed
    if ((*nodePlace)(nodeTag)%primeFrom == 0)
	fromSubdomain->addExternalNode(nodePtr);

    // if it was external remove from PartitionedDomains external nodes
    int partition = (*nodePlace)(nodeTag);
    for (int j=1; j<=numPartitions; j++)
	if (j != from) {
      int prime = primes(j);
      if (partition%prime == 0) {
	  myDomain->removeExternalNode(nodeTag);
	  j = numPartitions+1;
      }
	}
      }

      // 5. add new_ elements to boundary vertices of from if connected to
      //    vertex we just removed and of color of from and not already in boundary

      const std::set<int> &eleAdjacent= vertexPtr->getAdjacency();
      int eleAdjacentSize = eleAdjacent.size();

      for(std::set<int>::const_iterator a= eleAdjacent.begin(); a!= eleAdjacent.end(); a++)
	{
	  int otherEleVertexTag= *a;
	  Vertex *other = fromBoundary->getVertexPtr(otherEleVertexTag);
	  if (other == 0) {
	      other = theElementGraph->getVertexPtr(otherEleVertexTag);
	      if (other->getColor() == from)
		  fromBoundary->addVertex(other,false);
	  }
      }



      // in the TO subdomain we:
      //  1. remove all nodes connected to the element, may or may not be there
      //     if there don't divide by primeTo.
      //  2. mark all connecting nodes of elements as belonging to To
      //  3. add the nodes, checking if external or internal
      //  3. add the element
      //  4. change the vertex color to be to and add vertex to boundary
      //     also see if we can reduce the size of boundary
      //     of boundary of to vertices

      //  1. remove all nodes connected to the element, may or may not be there
      //      if there divide by primeTo, do this as may have been added as extnl
      int primeTo = primes(to);
      for (int l=0; l<nodesSize; l++) {
    int nodeTag = nodesArray[l]->getTag();
    int nodeColor = (*nodePlace)(nodeTag);
    if (nodeColor % primeTo == 0) {
	Node *nodePtr;
	nodePtr = toSubdomain->removeNode(nodes(l));
    }
      }


      for (int m=0; m<nodesSize; m++) {
    Node *nodePtr = nodesArray[m];
    int nodeTag = nodePtr->getTag();

    //  2. mark all connecting nodes of elements as belonging to To
    (*nodePlace)(nodeTag) *= primeTo;

    //  3. add the nodes, checking if external or internal
    int internal = 0;
    for (int n=1; n<=numPartitions; n++)
	if (n != to) {
      int prime = primes(n);
      if ((*nodePlace)(nodeTag)%prime == 0) { // its external
	  internal = 1;
	  n = numPartitions+1;
      }
	}

    if (internal == 0)
	toSubdomain->addNode(nodePtr);
    else {
	toSubdomain->addExternalNode(nodePtr);
	myDomain->addNode(nodePtr);
    }
      }

      delete [] nodesArray; // DELETE nodesArray here should not use nodePtrs
			    // after here as node objects deleted if parallel

      //  3. add the element

      toSubdomain->addElement(elePtr);

      //  4. change the vertex color to be to and add vertex to boundary
      //     also see if we can reduce the size of to's boundary vertices


      vertexPtr->setColor(to);
      toBoundary->addVertex(vertexPtr,false);

      for (int n=0; n<eleAdjacentSize; n++) {
	  int otherEleVertexTag = eleAdjacent(n);
	  Vertex *other = toBoundary->removeVertex(otherEleVertexTag,false);
	  if (other != 0) {
	      const std::set<int> &othersAdjacency= other->getAdjacency();
	      int otherSize = othersAdjacency.size();
	      for(std::set<int>::const_iterator b= othersAdjacency.begin(); b!=othersAdjacency.end(); b++)
		 {
		    int anotherEleVertexTag= *b;
		  Vertex *otherOther = theElementGraph->getVertexPtr(anotherEleVertexTag);
		  if(otherOther->getColor() != to)
		    {
		      toBoundary->addVertex(other,false);
		      b=otherSize;
		  }
	      }
	  }
      }


      // now remove any SFreedom_Constraints that may have been external to
      // PartitionedDomain and are now internal to toSubdomain
      SFreedom_ConstraintIter &theSPs = myDomain->getSPs();
      SFreedom_Constraint *spPtr;
      while ((spPtr = theSPs()) != 0) {
    int nodeTag = spPtr->getNodeTag();
    for (int i=0; i<nodesSize; i++) {
	if (nodeTag == nodes(i)) {
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
	  if (j != to) {
	int prime = primes(j);
	if ((*nodePlace)(nodeTag)%prime == 0) {
	    internal = 1;
	    j = numPartitions+1;
	}
	  }
      if (internal == 0) { // add to toSubdomain if inteernal
	  myDomain->removeSFreedom_Constraint(spPtr->getTag());
	  toSubdomain->addSFreedom_Constraint(spPtr);
      }
	}
    }
      }



      // now remove any SFreedom_Constraints that may have been internal to fromSubdomain
      // and are now external to XC::PartitionedDomain or internal to toSubdomain
      SFreedom_ConstraintIter &theSPs2 = fromSubdomain->getSPs();
      while ((spPtr = theSPs2()) != 0) {
    int nodeTag = spPtr->getNodeTag();
    for (int i=0; i<nodesSize; i++) {
	if (nodeTag == nodes(i)) {
      fromSubdomain->removeSFreedom_Constraint(spPtr->getTag());
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
	  if (j != to) {
	int prime = primes(j);
	if ((*nodePlace)(nodeTag)%prime == 0) {
	    internal = 1;
	    j = numPartitions+1;
	}
	  }
      if (internal == 0)
	  toSubdomain->addSFreedom_Constraint(spPtr);
      else
	  myDomain->addSFreedom_Constraint(spPtr);
	}
    }
      }



    */
      /*
      // now remove any NodalLoads that may have been external to
      // PartitionedDomain and are now internal to toSubdomain
      NodalLoadIter &theNodalLoads = myDomain->getNodalLoads();
      NodalLoad *loadPtr;
      while ((loadPtr = theNodalLoads()) != 0) {
    int nodeTag = loadPtr->getNodeTag();
    for (int i=0; i<nodesSize; i++) {
	if (nodeTag == nodes(i)) {
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
	  if (j != to) {
	int prime = primes(j);
	if ((*nodePlace)(nodeTag)%prime == 0) {
	    internal = 1;
	    j = numPartitions+1;
	}
	  }
      if (internal == 0) { // add to toSubdomain if inteernal
	  myDomain->removeNodalLoad(loadPtr->getTag());
	  toSubdomain->addNodalLoad(loadPtr);
      }
	}
    }
      }


      // now remove any NodalLoads that may have been internal to fromSubdomain
      // and are now external to XC::PartitionedDomain or internal to toSubdomain

      NodalLoadIter &theNodalLoads2 = myDomain->getNodalLoads();
      while ((loadPtr = theNodalLoads2()) != 0) {
    int nodeTag = loadPtr->getNodeTag();
    for (int i=0; i<nodesSize; i++) {
	if (nodeTag == nodes(i)) {
      fromSubdomain->removeNodalLoad(loadPtr->getTag());
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
	  if (j != to) {
	int prime = primes(j);
	if ((*nodePlace)(nodeTag)%prime == 0) {
	    internal = 1;
	    j = numPartitions+1;
	}
	  }
      if (internal == 0)
	  toSubdomain->addNodalLoad(loadPtr);
      else
	  myDomain->addNodalLoad(loadPtr);
	}
    }
      }
      */

    return 0;

  }



//! @brief Method to move from from to to, all elements on the interface of
//! from that are adjacent with to.
//! 
//! Method which when invoked will take all the boundary elements in
//! subdomain \p from that are connected to elements in subdomain {\em
//! to} and place them in subdomain \p to. If \p adjacentVertexOther is
//! \p true no Elements which are connected to elements in subdomains other 
//! than \p to and \p from are moved. Returns \f$0\f$ if successfull, returns 
//! an error message and \f$-1\f$ if PartitionedDomain has not been partitioned,
//! \f$-2\f$ if \p from Subdomain does not exist, \f$-3\f$ if {\em to Subdomain}
//! does not exist. The Elements, Nodes, NodalLoads, SFreedom\_Constraints
//! and MP\_Constraints that need to be moved between the two Subdomains,
//! or between the PartitionedDomain and Subdomains are also moved. NO
//! ELEMENTAL LOADS are moved yet. It performs the operation by creating
//! an ID of vertices and then using code similar to that used in {\em
//! swapVertex()}; swapVErtex() is not called repeatedly as this was
//! found to be too slow. 
int XC::DomainPartitioner::swapBoundary(int from, int to, bool adjacentVertexNotInOther)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; from "  << from << " to " << to << std::endl;
  /***********************************************************************************
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " from " << from << "  to " << to << std::endl;
    Timer timer;
    timer.start();

    // check that the object did the partitioning
    if (partitionFlag == false) {
  std::cerr << getClassName() << "::" << __FUNCTION__
            << "(const XC::Vector &load)"
            << "; not partitioned or DomainPartitioner did not partition.\n";
  return -1;
    }

    // check that the subdomains exist in partitioned domain
    Subdomain *fromSubdomain = myDomain->getSubdomainPtr(from);
    if (fromSubdomain == 0) {
  std::cerr << getClassName() << "::" << __FUNCTION__
            << "; no from subdomain: "
            << from << " exists.\n";
  return -2;
    }

    Subdomain *toSubdomain = myDomain->getSubdomainPtr(to);
    if (toSubdomain == 0) {
  std::cerr << getClassName() << "::" << __FUNCTION__
            << "; no to subdomain: " << to << " exists\n";
  return -3;
    }

    // get the element graph
    //    Graph &theEleGraph = myDomain->getElementGraph();
    Graph *fromBoundary = theBoundaryElements[from-1];
    Graph *toBoundary = theBoundaryElements[to-1];

    // into a new_ graph place the vertices that are to be swapped
    Graph swapVertices(fromBoundary->getNumVertex());

    VertexIter &swappableVertices = fromBoundary->getVertices();
    Vertex *vertexPtr;

    while ((vertexPtr = swappableVertices()) != 0) {
        if (adjacentVertexNotInOther == false) {
            const std::set<int> &adjacency= vertexPtr->getAdjacency();
            const int size= adjacency.size();
            for(std::set<int>::const_iterator i= adjacency.begin(); i!=adjacency.end(); i++)
              {
                int otherTag= *i;
                Vertex *otherVertex = toBoundary->getVertexPtr(otherTag);
                if (otherVertex != 0) {
                    swapVertices->addVertex(vertexPtr,false);
                    i = size;
                }
            }
        }
        else
          {
            const std::set<int> &adjacent= vertexPtr->getAdjacency();
            bool inTo = false;
            bool inOther = false;
            int adjacentSize = adjacent.Size();
            for(std::set<int>::const_iterator i=adjacent.begin(); i!=adjacent.end(); i++)
              {
                Vertex *other = theElementGraph->getVertexPtr(*i);
                if (other->getColor() == to)
                    inTo = true;
                else if (other->getColor() != from) {
                    inOther = true;
                    i = adjacentSize;
                }
            }
            if (inTo == true && inOther == false) { // we remove the vertex
                swapVertices->addVertex(vertexPtr,false);
                vertexPtr->setColor(to);
            }
        }
    }

    //
    // in the FROM subdomain we:
    //  1. remove the elements corresponding to the vertices
    //  2. remove all nodes connected to the elements divide color by prime
    //  3. see if have to add nodes back as external
    //  4. remove from the XC::PartitionedDomain if node was external
    //  5. add new_ elements to the boundary vertices for from

    // 1. first remove all the elements from from subdomain
    VertexIter &verticesToSwap = swapVertices->getVertices();

    int numEleToSwap = swapVertices->getNumVertex();
    std::vector<Element *> elementsArray(numEleToSwap);

    int count =0;
    int numCannotRemove =0;
    for (int i=0; i<numEleToSwap; i++) {
      vertexPtr = verticesToSwap();
  if (vertexPtr == 0)
      return -1;

  int vertexTag = vertexPtr->getTag();
  int eleTag = vertexPtr->getRef();

  // remove the vertex from fromBoundary, set color to to
  // and add to toBoundary
  bool inFrom = true;
  vertexPtr = fromBoundary->removeVertex(vertexTag,false);


  if (inFrom == true) {
      vertexPtr->setColor(to);
      toBoundary->addVertex(vertexPtr,false);
      Element *elePtr = fromSubdomain->removeElement(eleTag);
      if (elePtr == 0) // if ele not there we can just return IT SHOULD BE
    return -4;
      elementsArray[count] = elePtr;
      count++;
  }
  else  // we cannot remove the element
      numCannotRemove++;
    }
    numEleToSwap -= numCannotRemove;

    //  2. remove all nodes connected to the elements divide color by prime
    //     for each element and add back if still in subdomain
    //  3. add back if still in subdomain
    //  4. if node external remove from PartitionedDomains external nodes

    // first determine which nodes to remove
    int primeFrom = primes(from);
    int primeTo = primes(to);
    int numNodToSwap = 0;
    ID nodesToRemove(0,128);
    for (int j=0; j<numEleToSwap; j++) {
  Element *elePtr = elementsArray[j];
  const XC::ID  &nodes = elePtr->getNodePtrs().getExternalNodes();

  int nodesSize = nodes.Size();

  for (int k=0; k<nodesSize; k++) {
      int nodeTag = nodes(k);
      int loc = nodesToRemove.getLocation(nodeTag);
      if (loc < 0) {
    nodesToRemove[numNodToSwap] = nodeTag;
    numNodToSwap++;
      }
      (*nodePlace)(nodeTag) /= primeFrom;
      (*nodePlace)(nodeTag) *= primeTo;
  }
    }

    // remove the nodes
    std::vector<Node *> nodesArray(numNodToSwap);
    for (int k=0; k<numNodToSwap; k++) {
  int nodeTag = nodesToRemove(k);
  Node *nodePtr = fromSubdomain->removeNode(nodeTag);
  nodesArray[k] = nodePtr;

  // add the node back to from if still external
  if ((*nodePlace)(nodeTag)%primeFrom == 0)
      fromSubdomain->addExternalNode(nodePtr);

  // if it was external remove from PartitionedDomains external nodes
  int partition = (*nodePlace)(nodeTag);
  for (int j=1; j<=numPartitions; j++)
      if (j != from) {
    int prime = primes(j);
    if (partition%prime == 0) {
        myDomain->removeExternalNode(nodeTag);
        j = numPartitions+1;
    }
      }
    }

    // 5. add new_ vertices to boundary vertices of from if connected to
    //    vertex we just removed and of color of from and not already in boundary

    VertexIter &verticesToSwap2 = swapVertices->getVertices();
    while ((vertexPtr = verticesToSwap2()) != 0) {
  const std::set<int> &vertexAdjacent= vertexPtr->getAdjacency();
  int vertexAdjacentSize = vertexAdjacent.size();

        for(std::set<int>::const_iterator a=vertexAdjacent.begin(); a!=vertexAdjacent.end(); a++)
          {
            const int otherEleVertexTag= *a;
            Vertex *other = fromBoundary->getVertexPtr(otherEleVertexTag);
            if (other == 0) {
                other = theElementGraph->getVertexPtr(otherEleVertexTag);
                if (other->getColor() == from)
                    fromBoundary->addVertex(other,false);
            }
        }
    }


    // in the TO subdomain we:
    //  1. remove all nodes connected to the element, may or may not be there
    //     if there don't divide by primeTo.
    //  2. mark all connecting nodes of elements as belonging to To
    //  3. add the nodes, checking if external or internal
    //  3. add the element
    //  4. change the vertex color to be to and add vertex to boundary
    //     also see if we can reduce the size of boundary
    //     of boundary of to vertices

    //  1. remove all nodes connected to the element, may or may not be there
    //      if there divide by primeTo, do this as may have been added as extnl

    for (int l=0; l<numNodToSwap; l++) {
  int nodeTag = nodesArray[l]->getTag();
  Node *nodePtr;
  nodePtr = toSubdomain->removeNode(nodesToRemove(l));
    }


    for (int m=0; m<numNodToSwap; m++) {
  Node *nodePtr = nodesArray[m];
  int nodeTag = nodePtr->getTag();

  //  3. add the nodes, checking if external or internal
  int internal = 0;
  for (int n=1; n<=numPartitions; n++)
      if (n != to) {
    int prime = primes(n);
    if ((*nodePlace)(nodeTag)%prime == 0) { // its external
        internal = 1;
        n = numPartitions+1;
    }
      }

  if (internal == 0)
      toSubdomain->addNode(nodePtr);
  else {
      toSubdomain->addExternalNode(nodePtr);
      myDomain->addNode(nodePtr);
  }
    }


    delete [] nodesArray; // DELETE nodesArray here should not use nodePtrs
                          // after here as node objects deleted if parallel

    //  3. add the elements
    for (int a=0; a<numEleToSwap; a++)
  toSubdomain->addElement(elementsArray[a]);


    //  4. change the vertex color to be to and add vertex to boundary
    //     also see if we can reduce the size of to's boundary vertices

    VertexIter &verticesToSwap3 = swapVertices->getVertices();
    while ((vertexPtr = verticesToSwap3()) != 0) {
        const std::set<int> &vertexAdjacent= vertexPtr->getAdjacency();
        int vertexAdjacentSize= vertexAdjacent.size();
        for(std::set<int>::const_iterator n= vertexAdjacent.begin(); n!=vertexAdjacent.end(); n++)
          {
            const int otherEleVertexTag= *n;
            Vertex *other = toBoundary->removeVertex(otherEleVertexTag,false);
            if (other != 0) {
                const std::set<int> &othersAdjacency = other->getAdjacency();
                int otherSize = othersAdjacency.Size();
                for(std::set<int>::const_iterator b= othersAdjacency.begin(); b!=othersAdjacency.end(); b++)
                   {
                    int anotherEleVertexTag = *b;
                    Vertex *otherOther
                        = theElementGraph->getVertexPtr(anotherEleVertexTag);
                    if (otherOther->getColor() != to) {
                        toBoundary->addVertex(other,false);
                        b=otherSize;
                    }
                }
            }
        }
    }

    // now remove any SFreedom_Constraints that may have been external to
    // PartitionedDomain and are now internal to toSubdomain
    SFreedom_ConstraintIter &theSPs = myDomain->getSPs();
    SFreedom_Constraint *spPtr;
    while ((spPtr = theSPs()) != 0) {
  int nodeTag = spPtr->getNodeTag();
  int loc = nodesToRemove.getLocation(nodeTag);
  if (loc >= 0) {
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
    if (j != to) {
        int prime = primes(j);
        if ((*nodePlace)(nodeTag)%prime == 0) {
      internal = 1;
      j = numPartitions+1;
        }
    }
      if (internal == 0) { // add to toSubdomain if inteernal
    myDomain->removeSFreedom_Constraint(spPtr->getTag());
    toSubdomain->addSFreedom_Constraint(spPtr);
      }
  }
    }


    // now remove any SFreedom_Constraints that may have been internal to fromSubdomain
    // and are now external to XC::PartitionedDomain or internal to toSubdomain
    SFreedom_ConstraintIter &theSPs2 = fromSubdomain->getSPs();
    while ((spPtr = theSPs2()) != 0) {
  int nodeTag = spPtr->getNodeTag();
  int loc = nodesToRemove.getLocation(nodeTag);
  if (loc >= 0) {
      fromSubdomain->removeSFreedom_Constraint(spPtr->getTag());
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
    if (j != to) {
        int prime = primes(j);
        if ((*nodePlace)(nodeTag)%prime == 0) {
      internal = 1;
      j = numPartitions+1;
        }
    }
      if (internal == 0)
    toSubdomain->addSFreedom_Constraint(spPtr);
      else
    myDomain->addSFreedom_Constraint(spPtr);
  }
    }

    // now remove any NodalLoads that may have been external to
    // PartitionedDomain and are now internal to toSubdomain
    NodalLoadIter &theNodalLoads = myDomain->getNodalLoads();
    NodalLoad *loadPtr;
    while ((loadPtr = theNodalLoads()) != 0) {
  int nodeTag = loadPtr->getNodeTag();
  int loc = nodesToRemove.getLocation(nodeTag);
  if (loc >= 0) {

      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
    if (j != to) {
        int prime = primes(j);
        if ((*nodePlace)(nodeTag)%prime == 0) {
      internal = 1;
      j = numPartitions+1;
        }
    }
      if (internal == 0) { // add to toSubdomain if inteernal
    myDomain->removeNodalLoad(loadPtr->getTag());
    toSubdomain->addNodalLoad(loadPtr);
      }
  }
    }


    // now remove any NodalLoads that may have been internal to fromSubdomain
    // and are now external to XC::PartitionedDomain or internal to toSubdomain

    NodalLoadIter &theNodalLoads2 = myDomain->getNodalLoads();
    while ((loadPtr = theNodalLoads2()) != 0) {
  int nodeTag = loadPtr->getNodeTag();
  int loc = nodesToRemove.getLocation(nodeTag);
  if (loc >= 0) {
      fromSubdomain->removeNodalLoad(loadPtr->getTag());
      int internal = 0;
      for (int j=1; j<=numPartitions; j++)
    if (j != to) {
        int prime = primes(j);
        if ((*nodePlace)(nodeTag)%prime == 0) {
      internal = 1;
      j = numPartitions+1;
        }
    }
      if (internal == 0)
    toSubdomain->addNodalLoad(loadPtr);
      else
    myDomain->addNodalLoad(loadPtr);
  }
    }


    delete swapVertices;

    timer.pause();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " DONE" << timer.getReal() << std::endl;

  ***************************************************************************/
    return 0;
}


//! Method which when invoked will take the element given by vertex
//! reference of the vertex whose tag is given by \p vertexTag from
//! subdomain \p from and place it in the subdomain to which it is most
//! attracted (to which it is most connected). If it is equally attracted
//! to two subdomains it is sent to the one with the lightest load (the
//! loads on the subdomains are given in the {\em
//! theWeightedPartitionGraph}. If the \p mustReleaseToLighter is {\em
//! true} a check is first made to see if the load on the intended
//! subdomain is lighter than the load in \p from and that the ratio in
//! load between from and the new domain is greater than {\em
//! factorGreater}; if this is the case the element is swapped, if not the
//! element is not swapped. An additional requirement that the vertex that
//! is to be swapped is not adjacent to any other partition can also be
//! set. 
//! The method determines which partition the vertex is to be sent to and
//! then returns the result of invoking swapVertex() on itself, with
//! this partition used as the \p to argument in the arguments.
int XC::DomainPartitioner::releaseVertex(int from,
                                 int vertexTag,
                                 Graph &theWeightedPartitionGraph,
                                 bool mustReleaseToLighter,
                                 double factorGreater,
                                 bool adjacentVertexNotInOther)
{
  // check that the object did the partitioning
  if (partitionFlag == false) {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "(const Vector &load)"
	      << "; not partitioned or DomainPartitioner did not partition.\n";
    return -1;
  }

  // we first check the vertex is on the fromBoundary
  Subdomain *fromSubdomain = myDomain->getSubdomainPtr(from);
  if (fromSubdomain == nullptr)
    {
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; no from subdomain: " << from << " exists\n";
      return -1;
    }

  // get the vertex from the boundary vertices of from
  // Graph &theEleGraph = myDomain->getElementGraph();
  Graph &fromBoundary= theBoundaryElements[from-1];

  Vertex *vertexPtr= fromBoundary.getVertexPtr(vertexTag);
  if(!vertexPtr)
    vertexPtr= theElementGraph->getVertexPtr(vertexTag);

  if(vertexPtr == 0)  // if still 0 no vertex given by tag exists
    return -3;

  ID attraction(numPartitions+1);
  attraction.Zero();

  // determine the attraction to the other partitions
  const std::set<int> &adjacent= vertexPtr->getAdjacency();
  for(std::set<int>::const_iterator i= adjacent.begin(); i!=adjacent.end(); i++)
    {
      const int otherTag= *i;
      Vertex *otherVertex = theElementGraph->getVertexPtr(otherTag);
      int otherPartition = otherVertex->getColor();
      if (otherPartition != from)
        attraction(otherPartition) += 1;
    }

  // determine the other partition the vertex is most attracted to
  int partition = 1;
  int maxAttraction = attraction(1);
  for (int j=2; j<=numPartitions; j++)
    if (attraction(j) > maxAttraction) {
      partition = j;
      maxAttraction = attraction(j);
    }

  // swap the vertex
  if (mustReleaseToLighter == false)
    return swapVertex(from, partition, vertexTag, adjacentVertexNotInOther);

  else { // check the other partition has a lighter load
    Vertex *fromVertex = theWeightedPartitionGraph.getVertexPtr(from);
    Vertex *toVertex = theWeightedPartitionGraph.getVertexPtr(partition);

    double fromWeight = fromVertex->getWeight();
    double toWeight  = toVertex->getWeight();

    if (fromWeight == toWeight)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; TO CHANGE >= to >\n";

    if (fromWeight >= toWeight)
      {
      if (toWeight == 0.0)
        return swapVertex(from,partition,vertexTag,adjacentVertexNotInOther);
      if (fromWeight/toWeight > factorGreater)
        return swapVertex(from,partition,vertexTag,adjacentVertexNotInOther);
    }
  }

  return 0;
}

//! @brief Release all the elements on the boundary of subdomain \p from.
//! 
//! Method which when invoked will release all the elements on the
//! boundary of subdomain \p from. It performs the operation by
//! creating an ID of all the vertices on the boundary of the \p from
//! Subdomain. Then releaseBoundary() is invoked on all these vertices.
int XC::DomainPartitioner::releaseBoundary(int from,
           Graph &theWeightedPartitionGraph,
           bool mustReleaseToLighter,
           double factorGreater,
           bool adjacentVertexNotInOther)
  {
    // check that the object did the partitioning
    if(partitionFlag == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "(const XC::Vector &load)"
		  << "; not partitioned or DomainPartitioner"
	          << " did not partition.\n";
        return -1;
      }

    // we first get a pointer to fromSubdomain & the fromBoundary
    Subdomain *fromSubdomain = myDomain->getSubdomainPtr(from);
    if(fromSubdomain == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no from subdomain: " << from << " exists.\n";
        return -1;
      }

    //    Graph &theEleGraph = myDomain->getElementGraph();
    Graph &fromBoundary= theBoundaryElements[from-1];

    // into a new_ graph place the vertices on the fromBoundary
    // we cannot use fromBoundary as this would empty all the nodes
    // as fromBoundary changes in called methods

    Graph swapVertices(fromBoundary.getNumVertex());

    VertexIter &swappableVertices= fromBoundary.getVertices();
    Vertex *vertexPtr= nullptr;

    while((vertexPtr = swappableVertices()) != 0)
      swapVertices.addVertex(*vertexPtr,false);

    // release all the vertices in the swapVertices
    VertexIter &verticesToSwap= swapVertices.getVertices();
    while((vertexPtr = verticesToSwap()) != 0)
      releaseVertex(from,
                    vertexPtr->getTag(),
                    theWeightedPartitionGraph,
                    mustReleaseToLighter,
                    factorGreater,
                    adjacentVertexNotInOther);
    return 0;
  }

