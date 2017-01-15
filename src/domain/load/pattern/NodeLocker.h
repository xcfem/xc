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







