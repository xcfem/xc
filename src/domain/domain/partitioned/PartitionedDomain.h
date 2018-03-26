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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/partitioned/PartitionedDomain.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Wed Sep 25 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for PartitionedDomain.
// PartitionedDomain is an abstract class. The class is responsible for holding
// and providing access to the Elements, Nodes, SFreedom_Constraints 
// and MFreedom_Constraints just like a normal domain. In addition the domain provides
// a method to partition the domain into Subdomains.
//
// ModelBuilder. There are no partitions in a PartitionedDomain.
//
// What: "@(#) PartitionedDomain.h, revA"

#ifndef PartitionedDomain_h
#define PartitionedDomain_h

#include <domain/domain/Domain.h>
#include "solution/graph/graph/Graph.h"

namespace XC {
class DomainPartitioner;
class Subdomain;
class SubdomainIter; 
class ArrayOfTaggedObjects;
class PartitionedDomainSubIter;
class PartitionedDomainEleIter;
class SingleDomEleIter;

//! @brief Partitioned domain (aggregation of subdomains).
//! 
//! PartitionedDomain is an extension of Domain. A partitioned
//! domain is an aggregation of subdomains. All elements, nodes, loadcases
//! are added to the PartitionedDomain. The components can be moved among 
//! subdomains (keeping in mind that subdomains are themselves subclasses
//! of domain and therefore must obey the requirements for adding and
//! removing elements specified in the interface for Domain) by invoking
//! the remove..() and add...() methods on the subdomain.
class PartitionedDomain: public Domain
  {
  private:
    TaggedObjectStorage  *elements;    
    ArrayOfTaggedObjects *theSubdomains;
    DomainPartitioner    *theDomainPartitioner;

    SingleDomEleIter	       *mainEleIter;  // for ele that belong to elements
    PartitionedDomainSubIter   *theSubdomainIter;
    PartitionedDomainEleIter   *theEleIter;
    
    Graph mySubdomainGraph; //! Grafo de conectividad de subdomains.
    void alloc(void);
    void free_mem(void);
  protected:
    int barrierCheck(int result);
    DomainPartitioner *getPartitioner(void) const;
    virtual int buildEleGraph(Graph &theEleGraph);
  public:
    PartitionedDomain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh);    
    PartitionedDomain(EntCmd *owr,DomainPartitioner &thePartitioner,DataOutputHandler::map_output_handlers *oh);    

    PartitionedDomain(EntCmd *owr,int numNods, int numElems, 
		      int numSPs, int numMPs, int numLPatterns, int numNLockers,
		      int numSubdom,
		      DomainPartitioner &thePartitioner,DataOutputHandler::map_output_handlers *oh);
    
    virtual  ~PartitionedDomain(void);    

    // public methods to populate a domain	
    virtual  bool addElement(Element *elePtr);
    virtual  bool addNode(Node *nodePtr);

    virtual  bool addLoadPattern(LoadPattern *);            
    virtual  bool addSFreedom_Constraint(SFreedom_Constraint *); 
    virtual  bool addSFreedom_Constraint(SFreedom_Constraint *, int loadPatternTag); 
    virtual  bool addMFreedom_Constraint(MFreedom_Constraint *);
    virtual  bool addMRMFreedom_Constraint(MRMFreedom_Constraint *);
    virtual  bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual  bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components     
    virtual void clearAll(void);
    virtual bool removeElement(int tag);
    virtual bool removeNode(int tag);        
    virtual bool removeSFreedom_Constraint(int tag);
    virtual bool removeMFreedom_Constraint(int tag);
    virtual bool removeLoadPattern(int loadTag);
    
    // methods to access the elements
    virtual  ElementIter       &getElements();
    virtual  Element           *getElement(int tag);
    
    virtual  int 		getNumElements(void) const;

    // public methods to update the domain
    virtual  void setCommitTag(int newTag);    	
    virtual  void setCurrentTime(double newTime);    
    virtual  void setCommittedTime(double newTime);        
    virtual  void applyLoad(double pseudoTime);
    virtual  void setLoadConstant(void);    

    virtual  int commit(void);    
    virtual  int revertToLastCommit(void);        
    virtual  int revertToStart(void);    
    virtual  int update(void);        
    virtual  int update(double newTime, double dT);
    virtual  int newStep(double dT);

    virtual int  addRecorder(Recorder &theRecorder);    	
    virtual int  removeRecorders(void);
    
    virtual  void Print(std::ostream &s, int flag =0);    

    // public member functions in addition to the standard domain
    virtual int setPartitioner(DomainPartitioner *thePartitioner);
    virtual int partition(int numPartitions, bool usingMain = false, int mainPartitionID = 0);
			
    virtual bool addSubdomain(Subdomain *theSubdomain);
    virtual int getNumSubdomains(void);
    virtual Subdomain *getSubdomainPtr(int tag);
    virtual SubdomainIter &getSubdomains(void);
    virtual bool removeExternalNode(int tag);        
    virtual Graph &getSubdomainGraph(void);

    // nodal methods required in domain interface for parallel interprter
    virtual double getNodeDisp(int nodeTag, int dof, int &errorFlag);
    virtual int setMass(const Matrix &mass, int nodeTag);
    
    // friend classes
    friend class PartitionedDomainEleIter;    
  };
} // end of XC namespace

#endif


