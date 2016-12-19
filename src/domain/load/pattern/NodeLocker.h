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
//NodeLocker.h
                                                
#ifndef NodeLocker_h
#define NodeLocker_h

#include "domain/component/ForceReprComponent.h"

namespace XC {
class SFreedom_Constraint;
class SingleDomSFreedom_Iter;
class SFreedom_ConstraintIter;
class TaggedObjectStorage;

//! @ingroup LPatterns
//
//! @brief Condiciones de contorno mononodales
//! que forman parte de un caso de carga.
class NodeLocker: public ForceReprComponent
  {
    void alloc_contenedores(void);
    void alloc_iteradores(void);
    NodeLocker(const NodeLocker &otro);
    NodeLocker &operator=(const NodeLocker &otro);
  protected:
    int nextTag; //!< Valor del tag para la siguiente condición.
    int currentGeoTag;
    int lastGeoSendTag;

    // storage objects for the constraints
    TaggedObjectStorage  *theSPs; //!< Almacenamiento de condiciones de contorno mononodales.	
    // iterator objects for the objects added to the storage objects
    SingleDomSFreedom_Iter *theSpIter; //!< Iterador sobre condiciones de contorno mononodales.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);


    friend class NodeLockers;
    friend class FEM_ObjectBroker;
    NodeLocker(void);
    NodeLocker(int tag);
    NodeLocker(int tag, int classTag);
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *theSp);
  public:
    virtual ~NodeLocker(void);

    // method to set the associated Domain
    virtual void setDomain(Domain *theDomain);

    // methods to add loads
    virtual SFreedom_Constraint *addSFreedom_Constraint(const int &tagNodo,const int &id_gdl,const double &valor);

    virtual SFreedom_ConstraintIter &getSPs(void);
    int getNumSPs(void) const;

    // methods to remove loads
    virtual void clearAll(void);
    SFreedom_Constraint *newSPConstraint(const int &,const int &,const double &);
    virtual bool removeSFreedom_Constraint(int tag);

    // methods to apply loads
    virtual void applyLoad(const double &pseudoTime= 0.0,const double &factor= 1.0);

    // methods for o/p
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag =0);



    std::deque<int> getTagsSPsNodo(int theNode, int theDOF) const;
    std::deque<int> getTagsSPsNodo(int theNode) const;
    bool nodoAfectadoSPs(int nodeTag) const;
  };


} // end of XC namespace

#endif







