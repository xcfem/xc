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
// $Date: 2006/01/10 00:33:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/partitioned/PartitionedDomain.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Wed Sep 25 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for PartitionedDomain.
// PartitionedDomain is an abstract class. The class is responsible for holding
// and providing access to the Elements, Nodes, SP_Constraints 
// and MP_Constraints just like a normal domain. In addition the domain provides
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

//! @brief Dominio particionado.
class PartitionedDomain: public Domain
  {
  private:
    TaggedObjectStorage  *elements;    
    ArrayOfTaggedObjects *theSubdomains;
    DomainPartitioner    *theDomainPartitioner;

    SingleDomEleIter	       *mainEleIter;  // for ele that belong to elements
    PartitionedDomainSubIter   *theSubdomainIter;
    PartitionedDomainEleIter   *theEleIter;
    
    Graph mySubdomainGraph; //! Grafo de conectividad de subdominios.
    void alloc(void);
    void libera(void);
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
    virtual  bool addSP_Constraint(SP_Constraint *); 
    virtual  bool addSP_Constraint(SP_Constraint *, int loadPatternTag); 
    virtual  bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual  bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components     
    virtual void clearAll(void);
    virtual bool removeElement(int tag);
    virtual bool removeNode(int tag);        
    virtual bool removeSP_Constraint(int tag);
    virtual bool removeMP_Constraint(int tag);
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
} // fin namespace XC

#endif


