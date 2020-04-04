// -*-c++-*-
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

// $Revision: 1.6 $
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/Subdomain.h,v $


#ifndef Subdomain_h
#define Subdomain_h

// File: ~/domain/subdomain/Subdomain.h
//
// Written: fmk
// Created:  11/96
// Revision: A
// Revision: B 03/98 - revised to allow parallel model generation
//
// Description: This file contains the class definition for Subdomain.
// Subdomain is a container class.
//
// What: "@(#) Subdomain.h, revA"

#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "utility/Timer.h"
#include "SubdomainNodIter.h"

namespace XC {
class Node;
class ID;
class TaggedObjectStorage;
class DomainDecompositionAnalysis;
class PartitionedModelBuilder;
class EquiSolnAlgo;
class IncrementalIntegrator;
class LinearSOE;
class ConvergenceTest;
class FE_Element;

//! @ingroup Dom
//!
//! @defgroup SubDom sub domains of the finite element model.
//
//! @ingroup SubDom
//
//! @brief Domain enclosed in another domain.
//!
//! A Subdomain is a Domain that can be an Element in an
//! enclosing Domain. For this reason, it inherits from both Domain and
//! Element. It also extends the Domain interface to deal with the
//! distinction between whether Nodes in the Subdomain are internal to the
//! Subdomain, or external. An external Node is a Node shared by two
//! Elements residing in different Subdomains.
//! The class is responsible for holding and providing access to the Elements,
//! Nodes, LoadCases, SFreedom_Constraints, and MFreedom_Constraints, and
//! MRMFreedom_Constraints that have been added to the subdomain.
class Subdomain: public Element, public Domain
  {
  private:
    double realCost;
    double cpuCost;
    int pageCost;
    Timer theTimer;
    DomainDecompositionAnalysis *theAnalysis;
    mutable ID *extNodes;
    FE_Element *theFEele;

    SingleDomNodIter *internalNodeIter;
    SingleDomNodIter *externalNodeIter;
    SubdomainNodIter *theNodIter;

    PartitionedModelBuilder *thePartitionedModelBuilder;
    static Matrix badResult;
  protected:
    virtual int buildMap(void) const;
    mutable bool mapBuilt;
    mutable ID *map;
    mutable Vector *mappedVect;
    mutable Matrix *mappedMatrix;


    FE_Element *getFE_ElementPtr(void);
    TaggedObjectStorage *internalNodes;
    TaggedObjectStorage *externalNodes;

    DomainDecompositionAnalysis *getDDAnalysis(void);
  public:
    Subdomain(int tag,DataOutputHandler::map_output_handlers *oh,CommandEntity *owr);

    virtual ~Subdomain(void);

    // method added for parallel domain generation
    virtual int buildSubdomain(int numSubdomains,PartitionedModelBuilder &theBuilder);

    // Domain methods which must be rewritten
    virtual void clearAll(void);
    virtual bool addNode(Node *);
    virtual bool removeNode(int tag);
    virtual NodeIter &getNodes(void);
    virtual const Node *getNode(int tag) const;
    virtual Node *getNode(int tag);
    virtual NodePtrsWithIDs &getNodePtrs(void);
    virtual const NodePtrsWithIDs &getNodePtrs(void) const;

    virtual bool hasNode(int tag);
    virtual bool hasElement(int tag);

    virtual int getNumNodes(void) const;
    virtual int commit(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    virtual int update(void);
    virtual int update(double newTime, double dT);

#ifdef _PARALLEL_PROCESSING
    virtual  int barrierCheckIN(void) {return 0;};
    virtual  int barrierCheckOUT(int) {return 0;};
#endif

    virtual  void Print(std::ostream &s, int flag =0) const;

    // Domain type methods unique to a Subdomain
    virtual NodeIter &getInternalNodeIter(void);
    virtual NodeIter &getExternalNodeIter(void) const;
    virtual bool addExternalNode(Node *);

    virtual void wipeAnalysis(void);
    virtual void setDomainDecompAnalysis(DomainDecompositionAnalysis &theAnalysis);
    virtual int setAnalysisAlgorithm(EquiSolnAlgo &theAlgorithm);
    virtual int setAnalysisIntegrator(IncrementalIntegrator &theIntegrator);
    virtual int setAnalysisLinearSOE(LinearSOE &theSOE);
    virtual int setAnalysisConvergenceTest(ConvergenceTest &theTest);
    virtual int invokeChangeOnAnalysis(void);

    // Element methods which must be written
    virtual int getNumExternalNodes(void) const;
    virtual const ID &getExternalNodes(void) const;
    virtual int getNumDOF(void) const;

    virtual int commitState(void);

    virtual const Matrix &getTangentStiff(void);
    virtual const Matrix &getInitialStiff(void);
    virtual const Matrix &getDamp(void);
    virtual const Matrix &getMass(void);

    virtual void  zeroLoad(void);
    virtual int addLoad(ElementalLoad *theLoad, double loadFactor);
    virtual int addInertiaLoadToUnbalance(const Vector &accel);

    virtual const Vector &getResistingForce(void) const;
    virtual const Vector &getResistingForceIncInertia(void) const;
    virtual bool isSubdomain(void);
    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

    // Element type methods unique to a subdomain
    virtual int computeTang(void);
    virtual int computeResidual(void);
    virtual const Matrix &getTang(void);

    void setFE_ElementPtr(FE_Element *theFE_Ele);
    virtual const Vector &getLastExternalSysResponse(void);
    virtual int computeNodalResponse(void);
    virtual int newStep(double deltaT);
    virtual bool doesIndependentAnalysis(void);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    virtual double getCost(void);
  };
} // end of XC namespace

#endif


