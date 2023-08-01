// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#ifndef MaterialWrapper_h
#define MaterialWrapper_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
  
//! @ingroup Mat
//
//! @brief Encapsulates a copy to a material.
template <typename MatType, int cTag>
class MaterialWrapper: public MovableObject
  {
  public:
    typedef MatType MaterialType;
  protected:
    MatType *theMaterial;

    void free_mem(void);
    void copy(const MatType *);
  public:
    MaterialWrapper(void);
    MaterialWrapper( const MatType &material); 
    MaterialWrapper(const MaterialWrapper &);
    MaterialWrapper &operator=(const MaterialWrapper &);
    ~MaterialWrapper(void);

    //! @brief Return a pointer to the wrapped material.
    inline const MatType *getMaterial(void) const
      { return theMaterial; }
    //! @brief Return a pointer to the wrapped material.
    inline MatType *getMaterial(void)
      { return theMaterial; }
    virtual void setMaterial(const MatType &);
    
    int sendData(Communicator &);  
    int recvData(const Communicator &);
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
  
//! @brief Free memory allocated for the uniaxial material.
template <typename MatType, int cTag>
void MaterialWrapper<MatType, cTag>::free_mem(void)
  {
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
  }

//! brief Copy object members.
template <typename MatType, int cTag>
void MaterialWrapper<MatType, cTag>::copy(const MatType *other)
  {
    free_mem();
    if(other)
      theMaterial= other->getCopy();
  }

//! @brief Sets the wrapped material.
template <typename MatType, int cTag>
void MaterialWrapper<MatType, cTag>::setMaterial(const MatType &material)
  {
    copy(&material);
    if(!theMaterial)
      {
        std::cerr << "MaterialWrapper::" << __FUNCTION__
		  <<  "; failed to get copy of material\n";
        exit(-1);
      }    
  }
//! @brief Default constructor.
template <typename MatType, int cTag>
MaterialWrapper<MatType, cTag>::MaterialWrapper(void)
  : MovableObject(cTag), theMaterial(nullptr) {}

//! @brief Constructor.
template <typename MatType, int cTag>
MaterialWrapper<MatType, cTag>::MaterialWrapper(const MatType &material)
  :MovableObject(cTag), theMaterial(nullptr)
  {
    setMaterial(material);
  }

template <typename MatType, int cTag>
MaterialWrapper<MatType, cTag>::~MaterialWrapper(void)
  { free_mem(); }

//! @brief Copy constructor.
template <typename MatType, int cTag>
MaterialWrapper<MatType, cTag>::MaterialWrapper(const MaterialWrapper &other)
  :MovableObject(other), theMaterial(nullptr)
  { copy(other.theMaterial); }

//! @brief Assignment operator.
template <typename MatType, int cTag>
MaterialWrapper<MatType, cTag> &MaterialWrapper<MatType, cTag>::operator=(const MaterialWrapper &other)
  {
    MovableObject::operator=(other);
    copy(other.theMaterial);
    return *this;
  }

template <typename MatType, int cTag>
int MaterialWrapper<MatType, cTag>::sendData(Communicator &comm)
  {
    //int res= MovableObject::recvData(comm);
    int res= 0;
    res+= sendMaterialPtr(theMaterial,getDbTagData(),comm,BrokedPtrCommMetaData(1,2,3));
    return res;
  }

template <typename MatType, int cTag>
int MaterialWrapper<MatType, cTag>::recvData(const Communicator &comm)
  {
    //int res= MovableObject::recvData(comm);
    int res= 0;
    theMaterial= dynamic_cast<MatType *>(receiveMaterialPtr(theMaterial,getDbTagData(),comm,BrokedPtrCommMetaData(1,2,3)));
    if(!theMaterial)
      res= -1;
    
    return res;
  }

template <typename MatType, int cTag>
int MaterialWrapper<MatType, cTag>::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING MaterialWrapper::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

template <typename MatType, int cTag>
int MaterialWrapper<MatType, cTag>::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING MaterialWrapper::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(comm);
    return res;
  }
} // end of XC namespace


#endif

