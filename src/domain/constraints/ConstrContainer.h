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
//ConstrContainer.h

#ifndef ConstrContainer_h
#define ConstrContainer_h

#include "domain/mesh/MeshComponentContainer.h"
#include "MapCasosActivos.h"
#include <deque>

namespace XC {
class SP_Constraint;
class MP_Constraint;
class MRMP_Constraint;
class NodalLoad;
class ElementalLoad;
class LoadPattern;
 class NodeLocker;
class SP_ConstraintIter;
class MP_ConstraintIter;
class MRMP_ConstraintIter;
class LoadPatternIter;
class NodeLockerIter;

class SingleDomSP_Iter;
class SingleDomMP_Iter;
class SingleDomMRMP_Iter;
class SingleDomAllSP_Iter;
class FEM_ObjectBroker;

class TaggedObjectStorage;

//! @ingroup CCont
//
//! @brief Constraint container.
class ConstrContainer: public MeshComponentContainer
  {
  private:
    TaggedObjectStorage *theSPs;//!< Single point constraints.
    TaggedObjectStorage *theMPs;//!< Multi-point constraints.
    TaggedObjectStorage *theMRMPs;//!< Multi-row multi-point constraints.
    MapCasosActivos<NodeLocker> activeNodeLockers; //!< Bloqueadores de nodo activos.
    MapCasosActivos<LoadPattern> activeLoadPatterns; //!< Casos de carga activos.

    SingleDomSP_Iter *theSP_Iter;
    SingleDomMP_Iter *theMP_Iter;
    SingleDomMRMP_Iter *theMRMP_Iter;
    SingleDomAllSP_Iter *allSP_Iter;

    void alloc_contenedores(int numSPs= 256, int numMPs= 256,int numNodeLockers= 16,int numLoadPatterns= 32);
    void alloc_iters(void);
    bool check_contenedores(void) const;
  protected:
    friend class Domain;
    // methods to populate containers
    virtual bool addSP_Constraint(SP_Constraint *);
    virtual bool addMP_Constraint(MP_Constraint *);
    virtual bool addMRMP_Constraint(MRMP_Constraint *);
    virtual bool addLoadPattern(LoadPattern *);
    virtual bool addNodeLocker(NodeLocker *);

    // methods to add components to a LoadPattern object
    virtual bool addSP_Constraint(SP_Constraint *, int loadPatternTag);
    virtual bool addNodalLoad(NodalLoad *, int loadPatternTag);
    virtual bool addElementalLoad(ElementalLoad *, int loadPatternTag);

    // methods to remove the components
    virtual void clearAll(void);
    virtual bool removeSP_Constraint(int theNode, int theDOF, int loadPatternTag);
    virtual bool removeSP_Constraint(int tag);
    virtual bool removeMP_Constraint(int tag);
    virtual bool removeMRMP_Constraint(int tag);
    virtual bool removeLoadPattern(int loadTag,int &numSPs);
    virtual bool removeNodeLocker(int spcpTag,int &numSPs);
    int removeLPs(void);
    int removeNLs(void);

    virtual bool removeNodalLoad(int tag, int loadPattern);
    virtual bool removeElementalLoad(int tag, int loadPattern);
    virtual bool removeSP_Constraint(int tag, int loadPattern);

    void libera(void);
    DbTagData &getDbTagData(void) const;
    int sendLPatternsTags(const int &,const int &,CommParameters &);
    int recvLPatternsTags(const int &,const int &,const CommParameters &);
    int sendNLockersTags(const int &,const int &,CommParameters &cp);
    int recvNLockersTags(const int &,const int &,const CommParameters &cp);
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    ConstrContainer(Domain *owr);
    ConstrContainer(Domain *owr,int numSPs, int numMPs,int numNodeLockers,int numLoadPatterns);

    virtual ~ConstrContainer(void);

     // methods to access the components
    virtual SP_ConstraintIter &getSPs();
    virtual MP_ConstraintIter &getMPs();
    virtual MRMP_ConstraintIter &getMRMPs();
    const std::map<int,LoadPattern *> &getLoadPatterns(void) const;
    const std::map<int,NodeLocker *> &getNodeLockers(void) const;
    std::map<int,LoadPattern *> &getLoadPatterns(void);
    std::map<int,NodeLocker *> &getNodeLockers(void);
    virtual SP_ConstraintIter &getDomainAndLoadPatternSPs();

    virtual SP_Constraint *getSP_Constraint(int tag);
    virtual MP_Constraint *getMP_Constraint(int tag);
    virtual MRMP_Constraint *getMRMP_Constraint(int tag);
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
    virtual int getNumNodeLockers(void) const;

    std::deque<int> getTagsSPsNodo(int theNode, int theDOF) const;
    std::deque<int> getTagsSPsNodo(int theNode) const;
    std::deque<int> getTagsMPsNodo(int theNode, int theDOF) const;
    std::deque<int> getTagsMPsNodo(int theNode) const;
    std::deque<int> getTagsLPs(void) const;
    std::deque<int> getTagsNLs(void) const;
    bool nodoAfectadoSPs(int nodeTag) const;
    bool nodoAfectadoMPs(int nodeTag) const;
    bool nodoAfectadoMRMPs(int tagNodo) const;
    bool nodoAfectadoSPsOMPs(int nodeTag) const;
    std::string getNombresLPs(void) const;

    virtual int calculateNodalReactions(bool inclInertia, const double &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, ConstrContainer &cc);
  };
} // fin namespace XC

#endif


