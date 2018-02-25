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

// $Revision: 1.16 $
// $Date: 2005/11/30 23:32:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/Domain.h,v $

// Written: fmk
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for Domain.
// Domain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, SFreedom_Constraints
// MFreedom_Constraints, and LoadPatterns.
//
// What: "@(#) Domain.h, revA"

#ifndef Domain_h
#define Domain_h

#include "utility/recorder/ObjWithRecorders.h"
#include "PseudoTimeTracker.h"
#include "../mesh/Mesh.h"
#include "../constraints/ConstrContainer.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Element;
class Node;
class Preprocessor;

class ElementIter;
class NodeIter;

class SingleDomEleIter;
class SingleDomNodIter;

class LoadCombination;

class MeshRegion;
class DqMeshRegion;
class Recorder;
class Graph;
class NodeGraph;
class ElementGraph;
class FEM_ObjectBroker;
class RayleighDampingFactors;

//!  @defgroup Dom Domain for the finite element problem.

//! @ingroup Dom
//
//! @brief Domain (mesh and boundary conditions) of the finite element model.
//! Is a container class for storing and providing access to
//! the components of a domain, i.e. nodes, elements, boundary conditions,
//! and load patterns.
//!
//! A Domain is a container class which contains the  all elements,
//! nodes, load cases, single point constraints and multiple point
//! constraints that the model builder uses to create the model of the
//! structure. It provides operations for the following: 
//! 
//! - Population: Methods so that the DomainComponents can be addled
//! to the Domain. 
//! - Depopulation: Methods so that the DomainComponents can be removed
//! from the Domain.
//! - Access: Methods so that other classes, i.e. analysis and design,
//! can access the DomainComponents.
//! - Query: Methods for determining the state of the domain.
//! - Update: Methods for updating the state of the Domain
//! - Analysis: Methods added for the Analysis class.
class Domain: public ObjWithRecorders, public DistributedBase
  {
  private:
    PseudoTimeTracker timeTracker;//!< pseudo time
    std::string CallbackCommit; //!< Instrucciones que se ejecutan en cada llamada a commit.

    int dbTag; //!< Tag for the database.
    int currentGeoTag; //!< an integer used to mark if domain has changed
    bool hasDomainChangedFlag; //!< a bool flag used to indicate if GeoTag needs to be ++
    int commitTag;
    Mesh mesh; //!< Nodes and element container.
    ConstrContainer constraints;//!< Constraint container.
    Vector theEigenvalues; //!< Eigenvalues.
    Vector modalParticipationFactors; //!< Modal participation factors.
    DqMeshRegion *theRegions;
    std::string nmbCombActual;//!< Current load combination.

    int lastChannel;
    int lastGeoSendTag; //!< the value of currentGeoTag when sendSelf was last invoked

  protected:
    virtual int buildEleGraph(Graph &theEleGraph);
    virtual int buildNodeGraph(Graph &theNodeGraph);
    inline virtual Domain *get_domain_ptr(void)
      { return this; }

    void libera(void);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    Domain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh);
    Domain(EntCmd *owr,int numNods, int numElements, int numSPs, int numMPs,int numLPatterns,int numNLockers,DataOutputHandler::map_output_handlers *oh);

    virtual ~Domain(void);

    // methods to populate a domain
    virtual bool addElement(Element *);
    virtual bool addNode(Node *);
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *);
    virtual bool addMFreedom_Constraint(MFreedom_Constraint *);
    virtual bool addMRMFreedom_Constraint(MRMFreedom_Constraint *);
    virtual bool addLoadPattern(LoadPattern *);
    virtual bool addNodeLocker(NodeLocker *);
    virtual bool addLoadCombination(LoadCombination *);

    void setNodeReactionException(const int &);
    void checkNodalReactions(const double &);

    // methods to add components to a LoadPattern object
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *, int loadPatternTag);
    virtual bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components
    virtual void clearAll(void);
    virtual bool removeElement(int tag);
    virtual bool removeNode(int tag);
    virtual bool removeSFreedom_Constraint(int theNode, int theDOF, int loadPatternTag);
    virtual bool removeSFreedom_Constraint(int tag);
    virtual bool removeMFreedom_Constraint(int tag);
    virtual bool removeMRMFreedom_Constraint(int tag);
    virtual bool removeLoadPattern(int loadTag);
    virtual bool removeNodeLocker(int nlTag);
    bool removeLoadPattern(LoadPattern *lp);
    bool removeNodeLocker(NodeLocker *lp);
    void removeLoadCombination(LoadCombination *comb);
    void removeLPs(void);
    void removeNLs(void);

    virtual bool removeNodalLoad(int tag, int loadPattern);
    virtual bool removeElementalLoad(int tag, int loadPattern);
    virtual bool removeSFreedom_Constraint(int tag, int loadPattern);

    virtual void clearDOF_GroupPtr(void);

     // methods to access the components of a domain
    virtual ElementIter &getElements(void);
    virtual NodeIter &getNodes(void);
    virtual Mesh &getMesh(void);
    virtual const Mesh &getMesh(void) const;
    virtual ConstrContainer &getConstraints(void);
    virtual const ConstrContainer &getConstraints(void) const;

    const std::string &getCurrentCombinationName(void) const;

    bool existElement(int tag);
    virtual Element *getElement(int tag);
    virtual const Element *getElement(int tag) const;
    bool existNode(int tag);
    virtual Node *getNode(int tag);
    virtual const Node *getNode(int tag) const;


    // methods to query the state of the domain
    inline const PseudoTimeTracker &getTimeTracker(void) const
      { return timeTracker; }
    inline int getCurrentGeoTag(void) const
      { return currentGeoTag; }
    virtual int getCommitTag(void) const;
    virtual int getNumElements(void) const;
    virtual int getNumNodes(void) const;
    virtual const Vector &getPhysicalBounds(void);


     // methods to get element and node graphs
    virtual Graph &getElementGraph(void);
    virtual Graph &getNodeGraph(void);

     // methods to update the domain
    virtual void setCommitTag(int newTag);
    virtual void setCurrentTime(double newTime);
    virtual void setCommittedTime(double newTime);
    virtual void setTime(double newTime);
    virtual void applyLoad(double pseudoTime);
    virtual void setLoadConstant(void);
    virtual int initialize(void);
    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

    virtual int commit(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    virtual int update(void);
    virtual int update(double newTime, double dT);
    virtual int newStep(double dT);

    void resetLoadCase(void);

     // methods for eigenvalue analysis
    int getNumModes(void) const;
    virtual int setEigenvalues(const Vector &);
    virtual const double &getEigenvalue(int) const;
    double getAngularFrequency(int) const;
    double getPeriodo(int) const;
    double getFrecuencia(int) const;
    virtual const Vector &getEigenvalues(void) const;
    Vector getAngularFrequencies(void) const;
    Vector getPeriodos(void) const;
    Vector getFrecuencias(void) const;
    virtual int setModalParticipationFactors(const Vector &);
    virtual const double &getModalParticipationFactor(int mode) const;
    virtual const Vector &getModalParticipationFactors(void) const;
    const double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;
    double getTotalMass(void) const;

     // methods for other objects to determine if model has changed
    virtual void domainChange(void);
    virtual int hasDomainChanged(void);
    virtual void setDomainChangeStamp(int newStamp);

    virtual int addRegion(MeshRegion &theRegion);
    virtual MeshRegion *getRegion(int region);

    virtual void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, Domain &M);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    friend int sendDomain(Domain &, int posDbTag,DbTagData &,CommParameters &cp);
    friend int receiveDomain(Domain &, int posDbTag,DbTagData &,const CommParameters &cp);

    const Preprocessor *getPreprocessor(void) const;
    Preprocessor *getPreprocessor(void);

    // nodal methods required in domain interface for parallel interprter
    virtual double getNodeDisp(int nodeTag, int dof, int &errorFlag);
    virtual int setMass(const Matrix &mass, int nodeTag);

    virtual int calculateNodalReactions(bool inclInertia,const double &);
    virtual int addRecorder(Recorder &theRecorder);

    static void setDeadSRF(const double &);
  };

int sendDomain(Domain &,int posDbTag,DbTagData &,CommParameters &cp);
int receiveDomain(Domain &, int posDbTag,DbTagData &,CommParameters &cp);


} // end of XC namespace

#endif


