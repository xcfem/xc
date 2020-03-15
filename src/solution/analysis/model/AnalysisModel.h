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

// $Revision: 1.9 $
// $Date: 2005/11/28 21:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/AnalysisModel.h,v $


#ifndef AnalysisModel_h
#define AnalysisModel_h

// Written: fmk
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for AnalysisModel.
// AnalysisModel is a container class. This class is responsible for holding
// and providing access to the FE_Element and DOF_Group objects that the
// ConstraintHandler creates. It is also responsible for updating the
// response quantities at the DOF_Groups and for triggering methods
// in the associated Domain.
//
// What: "@(#) AnalysisModel.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/kernel/CommandEntity.h"
#include "solution/graph/graph/DOF_Graph.h"
#include "solution/graph/graph/DOF_GroupGraph.h"
#include "utility/tagged/storage/ArrayOfTaggedObjects.h"
#include "solution/analysis/model/FE_EleIter.h"
#include "solution/analysis/model/FE_EleConstIter.h"
#include "solution/analysis/model/DOF_GrpIter.h"
#include "solution/analysis/model/DOF_GrpConstIter.h"

namespace XC {
class Domain;
class Graph;
class FE_Element;
class LagrangeSFreedom_FE;
class LagrangeMFreedom_FE;
class LagrangeMRMFreedom_FE;
class PenaltySFreedom_FE;
class PenaltyMFreedom_FE;
class PenaltyMRMFreedom_FE;
class MFreedom_ConstraintBase;
class TransformationFE;
class DOF_Group;
class LagrangeDOF_Group;
class TransformationDOF_Group;
class Vector;
class DOF_GroupGraph;
class FEM_ObjectBroker;
class ConstraintHandler;
class TransformationConstraintHandler;
class RayleighDampingFactors;
class ModelWrapper;

//! @ingroup Solu
//! 
//! @defgroup Analysis Problem analysis procedures.
//
//! @ingroup Analysis
//!
//! @defgroup AnalysisModel Model as seen by the analysis.
//
//! @ingroup AnalysisModel
//
//! @brief Container for FE_Element and DOF_Group objects created by the
//! constraint handler.
//! 
//! AnalysisModel is a container class. This class is responsible
//! for holding and providing access to the FE\_Element and DOF\_Group
//! objects that the ConstraintHandler creates. It is also responsible
//! for updating the response quantities at the DOF\_Groups and for
//! triggering methods in the associated Domain. It provides operations
//! for the following:
//!
//! - Population: methods so that the ConstraintHandler can add the
//!   FE\_Element and DOF\_Group objects to the analysis model. 
//! - Access: methods so that other classes in the analysis aggregation
//!   can access the components of the AnalysisModel. 
//! - Connectivity: methods such that the SysOfEqn can determine the
//!   connectivity of the dof, which is needed for storage, sparsity, etc.
//! - Update: methods for updating the individual DOFs with the
//!   response quantities given by the AnalysisMethod.
//! - Trigger: methods which trigger events in the domain.
//! 
//! Each subclass of AnalysisModel must have its own subclasses
//! of FE\_ELEIter and DOF\_GrpIter.
class AnalysisModel: public MovableObject, public CommandEntity
  {
  private:
    int numFE_Ele; //!< number of FE_Elements objects added
    int numDOF_Grp; //!< number of DOF_Group objects added
    int numEqn; //!< numEqn set by the ConstraintHandler typically

    ArrayOfTaggedObjects theFEs;
    ArrayOfTaggedObjects theDOFGroups;
    FE_EleIter theFEiter;
    DOF_GrpIter theDOFGroupiter;
    mutable FE_EleConstIter theFEconst_iter;
    mutable DOF_GrpConstIter theDOFGroupconst_iter;

    mutable DOF_Graph myDOFGraph;
    mutable DOF_GroupGraph myGroupGraph;
    mutable bool updateGraphs;

    ModelWrapper *getModelWrapper(void);
    const ModelWrapper *getModelWrapper(void) const;
  protected:


    friend class Integrator;
    friend class Analysis;
    virtual void applyLoadDomain(double newTime);
    virtual int updateDomain(void);
    virtual int updateDomain(double newTime, double dT);
    virtual int newStepDomain(double dT =0.0);
    virtual int commitDomain(void);
    virtual int revertDomainToLastCommit(void);
    virtual double getCurrentDomainTime(void) const;
    virtual void setCurrentDomainTime(double newTime);
    virtual void setRayleighDampingFactors(const RayleighDampingFactors &rF);

    virtual bool addDOF_Group(DOF_Group *theDOF_Grp);
    virtual bool addFE_Element(FE_Element *theFE_Ele);

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    AnalysisModel(ModelWrapper *owr= nullptr);
    AnalysisModel(int classTag,CommandEntity *owr);
    AnalysisModel(const AnalysisModel &);
    AnalysisModel &operator=(const AnalysisModel &);
    AnalysisModel *getCopy(void) const;
  public:
    virtual ~AnalysisModel(void);

    // methods to populate/depopulate the AnalysisModel
    virtual DOF_Group *createDOF_Group(const int &, Node *);
    virtual LagrangeDOF_Group *createLagrangeDOF_Group(const int &, SFreedom_Constraint *);
    virtual LagrangeDOF_Group *createLagrangeDOF_Group(const int &, MFreedom_Constraint *);
    virtual LagrangeDOF_Group *createLagrangeDOF_Group(const int &, MRMFreedom_Constraint *);
    virtual TransformationDOF_Group *createTransformationDOF_Group(const int &, Node *, MFreedom_ConstraintBase *, TransformationConstraintHandler*);
    //virtual TransformationDOF_Group *createTransformationDOF_Group(const int &, Node *, MRMFreedom_Constraint *, TransformationConstraintHandler*);
    virtual TransformationDOF_Group *createTransformationDOF_Group(const int &, Node *, TransformationConstraintHandler*);
    virtual FE_Element *createFE_Element(const int &, Element *);
    virtual LagrangeSFreedom_FE *createLagrangeSFreedom_FE(const int &, SFreedom_Constraint &, DOF_Group &,const double &);
    virtual LagrangeMFreedom_FE *createLagrangeMFreedom_FE(const int &, MFreedom_Constraint &, DOF_Group &,const double &);
    virtual LagrangeMRMFreedom_FE *createLagrangeMRMFreedom_FE(const int &,MRMFreedom_Constraint &,DOF_Group &,const double &);
    virtual PenaltySFreedom_FE *createPenaltySFreedom_FE(const int &, SFreedom_Constraint &, const double &);
    virtual PenaltyMFreedom_FE *createPenaltyMFreedom_FE(const int &, MFreedom_Constraint &, const double &);
    virtual PenaltyMRMFreedom_FE *createPenaltyMRMFreedom_FE(const int &, MRMFreedom_Constraint &, const double &);
    virtual FE_Element *createTransformationFE(const int &, Element *, const std::set<int> &,std::set<FE_Element *> &);
    virtual void clearAll(void);

    // methods to access the FE_Elements and DOF_Groups and their numbers
    virtual int getNumDOF_Groups(void) const;
    virtual DOF_Group *getDOF_GroupPtr(int tag);
    virtual const DOF_Group *getDOF_GroupPtr(int tag) const;
    virtual FE_EleIter &getFEs();
    virtual DOF_GrpIter &getDOFGroups();
    virtual FE_EleConstIter &getConstFEs() const;
    virtual DOF_GrpConstIter &getConstDOFs() const;

    // method to access the connectivity for SysOfEqn to size itself
    virtual void setNumEqn(int) ;
    virtual int getNumEqn(void) const ;
    virtual Graph &getDOFGraph(void);
    virtual Graph &getDOFGroupGraph(void);
    virtual const Graph &getDOFGraph(void) const;
    virtual const Graph &getDOFGroupGraph(void) const;

    // methods to update the response quantities at the DOF_Groups,
    // which in turn set the new_ nodal trial response quantities.
    virtual void setResponse(const Vector &, const Vector &, const Vector &);
    virtual void setDisp(const Vector &disp);
    virtual void setVel(const Vector &vel);
    virtual void setAccel(const Vector &vel);

    virtual void incrDisp(const Vector &disp);
    virtual void incrVel(const Vector &vel);
    virtual void incrAccel(const Vector &vel);

    // methods added to store the modal analysis quantities in the domain
    virtual void setNumEigenvectors(int numEigenvectors);
    virtual void setEigenvector(int mode, const Vector &);
    virtual void setEigenvalues(const Vector &);
    virtual void setModalParticipationFactors(const Vector &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    Domain *getDomainPtr(void);
    const Domain *getDomainPtr(void) const;
    ConstraintHandler *getHandlerPtr(void);
    const ConstraintHandler *getHandlerPtr(void) const;
  };
} // end of XC namespace

#endif
