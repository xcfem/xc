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
//ConstrContainer.h

#ifndef ConstrContainer_h
#define ConstrContainer_h

#include "domain/mesh/MeshComponentContainer.h"
#include "MapActiveLoadPatterns.h"
#include <deque>
#include <set>

namespace XC {
class Constraint;
class SFreedom_Constraint;
class MFreedom_Constraint;
class MRMFreedom_Constraint;
class NodalLoad;
class ElementalLoad;
class LoadPattern;
class NodeLocker;
class SFreedom_ConstraintIter;
class MFreedom_ConstraintIter;
class MRMFreedom_ConstraintIter;
class LoadPatternIter;
class NodeLockerIter;

class SingleDomSFreedom_Iter;
class SingleDomMFreedom_Iter;
class SingleDomMRMFreedom_Iter;
class SingleDomAllSFreedom_Iter;
class FEM_ObjectBroker;

class TaggedObjectStorage;

//! @ingroup CCont
//
//! @brief Constraint (essential and natural boundary conditions) container.
class ConstrContainer: public MeshComponentContainer
  {
  private:
    TaggedObjectStorage *theSPs;//!< Single freedom constraints.
    TaggedObjectStorage *theMPs;//!< Multi-freedom constraints.
    TaggedObjectStorage *theMRMPs;//!< Multi-row multi-freedom constraints.
    MapActiveLoadPatterns<NodeLocker> activeNodeLockers; //!< active node lockers.
    MapActiveLoadPatterns<LoadPattern> activeLoadPatterns; //!< active load patterns.

    SingleDomSFreedom_Iter *theSFreedom_Iter;
    SingleDomMFreedom_Iter *theMFreedom_Iter;
    SingleDomMRMFreedom_Iter *theMRMFreedom_Iter;
    SingleDomAllSFreedom_Iter *allSFreedom_Iter;

    void alloc_containers(void);
    void alloc_iters(void);
    bool check_containers(void) const;
  protected:
    friend class Domain;
    // methods to populate containers
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *);
    virtual bool addMFreedom_Constraint(MFreedom_Constraint *);
    virtual bool addMRMFreedom_Constraint(MRMFreedom_Constraint *);
    virtual bool addLoadPattern(LoadPattern *);
    virtual bool isLoadPatternActive(const LoadPattern *) const;
    virtual bool addNodeLocker(NodeLocker *);

    // methods to add components to a LoadPattern object
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *, int loadPatternTag);
    virtual bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components
    virtual void clearAll(void);
    virtual bool removeSFreedom_Constraint(int theNode, int theDOF, int loadPatternTag);
    virtual bool removeSFreedom_Constraint(int tag);
    virtual bool removeMFreedom_Constraint(int tag);
    virtual bool removeMRMFreedom_Constraint(int tag);
    virtual bool removeLoadPattern(int loadTag,int &numSPs);
    virtual bool removeNodeLocker(int spcpTag,int &numSPs);
    int removeLPs(void);
    int removeNLs(void);

    virtual bool removeNodalLoad(int tag, int loadPattern);
    virtual bool removeElementalLoad(int tag, int loadPattern);
    virtual bool removeSFreedom_Constraint(int tag, int loadPattern);

    void free_mem(void);
    DbTagData &getDbTagData(void) const;
    int sendLPatternsTags(const int &,const int &,Communicator &);
    int recvLPatternsTags(const int &,const int &,const Communicator &);
    int sendNLockersTags(const int &,const int &,Communicator &comm);
    int recvNLockersTags(const int &,const int &,const Communicator &comm);
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    ConstrContainer(Domain *owr);

    virtual ~ConstrContainer(void);

     // methods to access the components
    virtual SFreedom_ConstraintIter &getSPs();
    virtual MFreedom_ConstraintIter &getMPs();
    virtual MRMFreedom_ConstraintIter &getMRMPs();
    const std::map<int,LoadPattern *> &getLoadPatterns(void) const;
    const std::map<int,NodeLocker *> &getNodeLockers(void) const;
    std::map<int,LoadPattern *> &getLoadPatterns(void);
    std::map<int,NodeLocker *> &getNodeLockers(void);
    virtual SFreedom_ConstraintIter &getDomainAndLoadPatternSPs();

    virtual SFreedom_Constraint *getSFreedom_Constraint(int tag);
    virtual MFreedom_Constraint *getMFreedom_Constraint(int tag);
    virtual MRMFreedom_Constraint *getMRMFreedom_Constraint(int tag);
    virtual Constraint *getConstraint(int tag);
    LoadPattern *getLoadPattern(const int &);
    const LoadPattern *getLoadPattern(const int &) const;
    NodeLocker *getNodeLocker(const int &);
    const NodeLocker *getNodeLocker(const int &) const;

     // methods to update the domain
    virtual void applyLoad(double pseudoTime);
    virtual void setLoadConstant(void);

     // methods to query the state of the ConstrContainer
    virtual int getNumSPs(void) const;
    virtual int getNumMPs(void) const;
    virtual int getNumMRMPs(void) const;
    virtual int getNumLoadPatterns(void) const;
    virtual std::string getActiveLoadCaseDescription(void) const;
    virtual int getNumNodeLockers(void) const;

    std::deque<int> getTagsSPsNode(int , int ) const;
    std::deque<int> getTagsSPsNode(int ) const;
    std::deque<int> getTagsMPsNode(int , int ) const;
    std::deque<int> getTagsMPsNode(int ) const;
    std::deque<int> getTagsMRMPsNode(int , int ) const;
    std::deque<int> getTagsMRMPsNode(int ) const;
    std::deque<int> getTagsLPs(void) const;
    std::deque<int> getTagsNLs(void) const;
    bool nodeAffectedBySPs(int) const;
    std::set<int> getTagsNodesffectedBySPs(int ) const;
    bool nodeAffectedByMPs(int) const;
    bool nodeAffectedByMRMPs(int) const;
    bool nodeAffectedByConstraints(int ) const;
    bool isDOFAffectedBySPs(int, int) const;
    bool isDOFAffectedByMPs(int, int) const;
    bool isDOFAffectedByMRMPs(int, int) const;
    bool isDOFAffectedByConstraints(int, int) const;
    std::string getLoadPatternsNames(void) const;

    virtual int calculateNodalReactions(bool inclInertia, const double &);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    virtual void Print(std::ostream &s, int flag =0) const;
    friend std::ostream &operator<<(std::ostream &, const ConstrContainer &);
  };
std::ostream &operator<<(std::ostream &, const ConstrContainer &);
} // end of XC namespace

#endif


