//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
#include "xc_utils/src/nucleo/EntCmd.h"
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

//!  @ingroup Solu
//! 
//! @defgroup Analisis Procedimientos de análisis del problema.
//
//! @ingroup Analisis
//
//! @defgroup AnalisisModel Modelo a analizar.
//
//! @ingroup AnalisisModel
//
//! @brief Los objetos de esta clase, dan acceso a los
//! objetos FE_Element y DOF_Group creados por el Constraint Handler.
class AnalysisModel: public MovableObject, public EntCmd
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
    virtual double getCurrentDomainTime(void);
    virtual void setCurrentDomainTime(double newTime);
    virtual void setRayleighDampingFactors(const RayleighDampingFactors &rF);

    virtual bool addDOF_Group(DOF_Group *theDOF_Grp);
    virtual bool addFE_Element(FE_Element *theFE_Ele);

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    AnalysisModel(ModelWrapper *owr= NULL);
    AnalysisModel(int classTag,EntCmd *owr);
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
    virtual TransformationDOF_Group *createTransformationDOF_Group(const int &, Node *, MFreedom_Constraint *, TransformationConstraintHandler*);
    virtual TransformationDOF_Group *createTransformationDOF_Group(const int &, Node *, MRMFreedom_Constraint *, TransformationConstraintHandler*);
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
