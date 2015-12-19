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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/ShadowSubdomain.h,v $
                                                                        
                                                                        
#ifndef ShadowSubdomain_h
#define ShadowSubdomain_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ShadowSubdomain.
// ShadowSubdomain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SP_Constraints 
// and MP_Constraints that have been added to the ShadowSubdomain.
//
// What: "@(#) ShadowSubdomain.h, revA"

#include "Subdomain.h"
#include <utility/actor/shadow/Shadow.h>
#include "utility/matrix/ID.h"

namespace XC {
class ShadowSubdomain: public Shadow, public Subdomain
  {
  private:
    mutable ID msgData;
    ID theElements;
    ID theNodes;
    ID theExternalNodes;    
    ID theLoadCases;

    TaggedObjectStorage  *theShadowSPs;    
    TaggedObjectStorage  *theShadowMPs;    
    TaggedObjectStorage  *theShadowLPs;        
    
    int numDOF;
    int numElements;
    int numNodes;
    int numExternalNodes;
    int numSPs;
    int numMPs;
    int numLoadPatterns;

    bool buildRemote;
    bool gotRemoteData;
    
    FE_Element *theFEele;

    mutable Vector *theVector; // for storing residual info
    mutable Matrix *theMatrix; // for storing tangent info
    
    static char *shadowSubdomainProgram;

    static int count;  // MHS
    static int numShadowSubdomains;
    static std::deque<ShadowSubdomain *> theShadowSubdomains;

    void libera_vectors(void) const;
    void alloc_vectors(const size_t &) const;
    void resize_vectors(const size_t &) const;
    void libera_arrays(void);
    void alloc_arrays(const size_t &,const size_t &);
  protected:    
    virtual int buildMap(void) const;
    virtual int buildEleGraph(Graph &theEleGraph);
    virtual int buildNodeGraph(Graph &theNodeGraph);    
    
  public:
    ShadowSubdomain(int tag, MachineBroker &theMachineBroker, FEM_ObjectBroker &theObjectBroker,DataOutputHandler::map_output_handlers *oh,EntCmd *owr);
    ShadowSubdomain(int tag, Channel &, FEM_ObjectBroker &,DataOutputHandler::map_output_handlers *,EntCmd *);
    virtual  ~ShadowSubdomain(void);    

    // method added for parallel domain generation
    virtual int buildSubdomain(int numSubdomains, 
			       PartitionedModelBuilder &theBuilder);
    virtual int getRemoteData(void);

    // Methods inherited from Domain, Subdomain and Element
    // which must be rewritten

    virtual  bool addElement(Element *);
    virtual  bool addNode(Node *);
    virtual  bool addExternalNode(Node *);
    virtual  bool addSP_Constraint(SP_Constraint *);
    virtual  bool addMP_Constraint(MP_Constraint *);    
    virtual  bool addLoadPattern(LoadPattern *);            
    virtual  bool addNodalLoad(NodalLoad *, int loadPattern);
    virtual  bool addElementalLoad(ElementalLoad *, int loadPattern);
    virtual  bool addSP_Constraint(SP_Constraint *, int loadPattern);    


    virtual bool hasNode(int tag);
    virtual bool hasElement(int tag);

    virtual void clearAll(void);	
    virtual bool removeElement(int tag);
    virtual bool removeNode(int tag);    
    virtual bool removeSP_Constraint(int tag);
    virtual bool removeMP_Constraint(int tag);
    virtual bool removeLoadPattern(int tag);
    virtual bool removeNodalLoad(int tag, int loadPattern);
    virtual bool removeElementalLoad(int tag, int loadPattern);
    virtual bool removeSP_Constraint(int tag, int loadPattern);
    
    virtual  ElementIter       &getElements();
    virtual  NodeIter          &getNodes();
    virtual  NodeIter	       &getInternalNodeIter(void);
    virtual  NodeIter	       &getExternalNodeIter(void);    
    
    virtual  Element       *getElementPtr(int tag);
    virtual  Node          *getNodePtr(int tag);

    virtual int getNumElements(void) const;
    virtual int getNumNodes(void) const;
    virtual int getNumSPs(void) const;
    virtual int getNumMPs(void) const;
    virtual int getNumLoadPatterns(void) const;
    
    virtual  Graph &getElementGraph(void);
    virtual  Graph &getNodeGraph(void);

    // methods to update the domain
    virtual  void setCommitTag(int newTag);    	
    virtual  void setCurrentTime(double newTime);    
    virtual  void setCommittedTime(double newTime);        
    virtual  void applyLoad(double pseudoTime);
    virtual  void setLoadConstant(void);    

    virtual  int update(void);    
    virtual  int update(double newTime, double dT);    
    virtual  int commit(void);
    virtual  int revertToLastCommit(void);    
    virtual  int revertToStart(void);    
    virtual  int barrierCheckIN(void);    
    virtual  int barrierCheckOUT(int);    

    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF);

    virtual int  addRecorder(Recorder &theRecorder);    	
    virtual int  removeRecorders(void);

    virtual void wipeAnalysis(void);
    virtual void setDomainDecompAnalysis(DomainDecompositionAnalysis &theAnalysis);
    virtual int setAnalysisAlgorithm(EquiSolnAlgo &theAlgorithm);
    virtual int setAnalysisIntegrator(IncrementalIntegrator &theIntegrator);
    virtual int setAnalysisLinearSOE(LinearSOE &theSOE);
    virtual int setAnalysisConvergenceTest(ConvergenceTest &theTest);
    virtual void clearAnalysis(void);
    virtual void domainChange(void);
    
    virtual int getNumExternalNodes(void) const;    
    virtual const ID &getExternalNodes(void) const;
    virtual int getNumDOF(void) const;

    virtual const Matrix &getTang(void);    
    virtual const Vector &getResistingForce(void) const;    

    virtual int  computeTang(void);
    virtual int  computeResidual(void);

    const Vector &getLastExternalSysResponse(void);
    virtual int computeNodalResponse(void);    
    virtual int newStep(double deltaT);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);    

    virtual double getCost(void);
    
    virtual  void Print(std::ostream &s, int flag =0);

    // nodal methods required in domain interface for parallel interprter
    virtual double getNodeDisp(int nodeTag, int dof, int &errorFlag);
    virtual int setMass(const Matrix &mass, int nodeTag);
  };
} // end of XC namespace

#endif
