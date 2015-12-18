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

// $Revision: 1.16 $
// $Date: 2005/11/30 23:32:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/Domain.h,v $

// Written: fmk
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for Domain.
// Domain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, SP_Constraints
// MP_Constraints, and LoadPatterns.
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

class Combinacion;

class MeshRegion;
class DqMeshRegion;
class Recorder;
class Graph;
class NodeGraph;
class ElementGraph;
class FEM_ObjectBroker;
class RayleighDampingFactors;

//!  @defgroup Dom Dominio del problema de elementos finitos.

//! @ingroup Dom
//
//! @brief Dominio del problema de elementos finitos.
class Domain: public ObjWithRecorders, public DistributedBase
  {
  private:
    PseudoTimeTracker timeTracker;//!< pseudo time
    std::string CallbackCommit; //!< Instrucciones que se ejecutan en cada llamada a commit.

    int dbTag;
    int currentGeoTag; //!< an integer used to mark if domain has changed
    bool hasDomainChangedFlag; //!< a bool flag used to indicate if GeoTag needs to be ++
    int commitTag;
    Mesh mesh; //!< Nodes and elements.
    ConstrContainer constraints;//!< Constraint container.
    Vector theEigenvalues; //!< Autovalores.
    Vector modalParticipationFactors; //!< Factores de participación modal.
    DqMeshRegion *theRegions;
    std::string nmbCombActual;//!< Combinación de cálculo actual.

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
    virtual bool addSP_Constraint(SP_Constraint *);
    virtual bool addMP_Constraint(MP_Constraint *);
    virtual bool addMRMP_Constraint(MRMP_Constraint *);
    virtual bool addLoadPattern(LoadPattern *);
    virtual bool addNodeLocker(NodeLocker *);
    virtual bool addCombinacion(Combinacion *);

    void setNodeReactionException(const int &);
    void checkNodalReactions(const double &);

    // methods to add components to a LoadPattern object
    virtual bool addSP_Constraint(SP_Constraint *, int loadPatternTag);
    virtual bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components
    virtual void clearAll(void);
    virtual bool removeElement(int tag);
    virtual bool removeNode(int tag);
    virtual bool removeSP_Constraint(int theNode, int theDOF, int loadPatternTag);
    virtual bool removeSP_Constraint(int tag);
    virtual bool removeMP_Constraint(int tag);
    virtual bool removeMRMP_Constraint(int tag);
    virtual bool removeLoadPattern(int loadTag);
    virtual bool removeNodeLocker(int nlTag);
    bool removeLoadPattern(LoadPattern *lp);
    bool removeNodeLocker(NodeLocker *lp);
    void removeCombinacion(Combinacion *comb);
    void removeLPs(void);
    void removeNLs(void);

    virtual bool removeNodalLoad(int tag, int loadPattern);
    virtual bool removeElementalLoad(int tag, int loadPattern);
    virtual bool removeSP_Constraint(int tag, int loadPattern);

    virtual void clearDOF_GroupPtr(void);

     // methods to access the components of a domain
    virtual ElementIter &getElements(void);
    virtual NodeIter &getNodes(void);
    virtual Mesh &getMesh(void);
    virtual const Mesh &getMesh(void) const;
    virtual ConstrContainer &getConstraints(void);
    virtual const ConstrContainer &getConstraints(void) const;

    const std::string &getNombreCombActual(void) const;

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
    virtual const double &getEigenvalue(int mode) const;
    double getPulsacion(int mode) const;
    double getPeriodo(int mode) const;
    double getFrecuencia(int mode) const;
    virtual const Vector &getEigenvalues(void) const;
    Vector getPulsaciones(void) const;
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

    const Preprocessor *GetPreprocessor(void) const;
    Preprocessor *GetPreprocessor(void);

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


