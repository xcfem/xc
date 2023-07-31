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

#include "UniaxialMaterialWrapper.h"
#include <material/uniaxial/UniaxialMaterial.h>

//! @brief Free memory allocated for the uniaxial material.
void XC::UniaxialMaterialWrapper::free_mem(void)
  {
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
  }

//! brief Copy object members.
void XC::UniaxialMaterialWrapper::copy(const UniaxialMaterial *other)
  {
    free_mem();
    if(other)
      theMaterial= other->getCopy();
  }

//! @brief Sets the wrapped material.
void XC::UniaxialMaterialWrapper::setMaterial(const UniaxialMaterial &material)
  {
    copy(&material);
    if(!theMaterial)
      {
        std::cerr << "UniaxialMaterialWrapper::" << __FUNCTION__
		  <<  "; failed to get copy of material\n";
        exit(-1);
      }    
  }

//! @brief Default constructor.
XC::UniaxialMaterialWrapper::UniaxialMaterialWrapper(void)
  : MovableObject(MAT_TAG_UniaxialMaterialWrapper), theMaterial(nullptr) {}

//! @brief Constructor.
XC::UniaxialMaterialWrapper::UniaxialMaterialWrapper(const UniaxialMaterial &material)
  :MovableObject(MAT_TAG_UniaxialMaterialWrapper), theMaterial(nullptr)
  {
    setMaterial(material);
  }


XC::UniaxialMaterialWrapper::~UniaxialMaterialWrapper(void)
  { free_mem(); }

//! @brief Copy constructor.
XC::UniaxialMaterialWrapper::UniaxialMaterialWrapper(const UniaxialMaterialWrapper &other)
  :MovableObject(other), theMaterial(nullptr)
  { copy(other.theMaterial); }

//! @brief Assignment operator.
XC::UniaxialMaterialWrapper &XC::UniaxialMaterialWrapper::operator=(const UniaxialMaterialWrapper &other)
  {
    MovableObject::operator=(other);
    copy(other.theMaterial);
    return *this;
  }

double XC::UniaxialMaterialWrapper::getStrain(void) const
  { return theMaterial->getStrain(); }

double XC::UniaxialMaterialWrapper::getStrainRate(void) const
  { return theMaterial->getStrainRate(); }

int XC::UniaxialMaterialWrapper::sendData(Communicator &comm)
  {
    //int res= MovableObject::recvData(comm);
    int res= 0;
    res+= sendMaterialPtr(theMaterial,getDbTagData(),comm,BrokedPtrCommMetaData(1,2,3));
    return res;
  }

int XC::UniaxialMaterialWrapper::recvData(const Communicator &comm)
  {
    //int res= MovableObject::recvData(comm);
    int res= 0;
    theMaterial= dynamic_cast<UniaxialMaterial *>(receiveMaterialPtr(theMaterial,getDbTagData(),comm,BrokedPtrCommMetaData(1,2,3)));
    if(!theMaterial)
      res= -1;
    
    return res;
  }

int XC::UniaxialMaterialWrapper::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING UniaxialMaterialWrapper::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

int XC::UniaxialMaterialWrapper::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING UniaxialMaterialWrapper::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(comm);
    return res;
  }
