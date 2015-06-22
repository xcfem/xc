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
// Subdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SP_Constraints
// and MP_Constraints that have been added to the subdomain.
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

    //    TaggedObjectStorage  *realExternalNodes;

    SingleDomNodIter   *internalNodeIter;
    SingleDomNodIter   *externalNodeIter;
    SubdomainNodIter   *theNodIter;

    PartitionedModelBuilder *thePartitionedModelBuilder;
    static Matrix badResult;
  protected:
    virtual int buildMap(void) const;
    mutable bool mapBuilt;
    mutable ID *map;
    mutable Vector *mappedVect;
    mutable Matrix *mappedMatrix;


    FE_Element *getFE_ElementPtr(void);
    TaggedObjectStorage  *internalNodes;
    TaggedObjectStorage *externalNodes;

    DomainDecompositionAnalysis *getDDAnalysis(void);
  public:
    Subdomain(int tag,DataOutputHandler::map_output_handlers *oh,EntCmd *owr);

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

    virtual  void Print(std::ostream &s, int flag =0);

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

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual double getCost(void);
  };
} // fin namespace XC

#endif


