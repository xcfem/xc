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
//Joint2DPhysicalProperties.cpp

#include "Joint2DPhysicalProperties.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include "material/damage/DamageModel.h"
#include "utility/actor/actor/CommMetaData.h"

//! @brief Make copy of the uniaxial materials for the element.
void XC::Joint2DPhysicalProperties::set_springs(const SpringModels &springModels)
  {
    theMaterial= springModels;
    fixedEnd[0] = 0;
    fixedEnd[1] = 0;
    fixedEnd[2] = 0;
    fixedEnd[3] = 0;
    fixedEnd[4] = 0;

    for(size_t i=0 ; i<5 ; i++ )
      {
        if( fixedEnd[i] == 0 && theMaterial[i] == nullptr )
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << " ERROR: can not make copy of uniaxial materials, out of memory ";
            exit(-1);
          }
      }
  }

//! @brief Set the damage models
void XC::Joint2DPhysicalProperties::set_damage_models(const DamageModelVector &dmgModels)
  { theDamages= dmgModels; }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : UniaxialMatPhysicalProperties(nMat,ptr_mat), theDamages(5,nullptr), fixedEnd(5)
  {
    for(int i=0;i<5;i++)
      fixedEnd[i] = 1;
  }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const SpringModels &springModels)
  : UniaxialMatPhysicalProperties(5,nullptr), theDamages(5,nullptr), fixedEnd(5)
  {
    set_springs(springModels);
  }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const SpringModels &springModels, const DamageModelVector &dmgModels)
  : UniaxialMatPhysicalProperties(5,nullptr), theDamages(5,nullptr), fixedEnd(5)
  {
    // make copy of the uniaxial materials for the element

    set_springs(springModels);
    set_damage_models(dmgModels);
    theDamages.revertToStart(); //??
  }

//! @brief Commit state of the springs and the damage models.
int XC::Joint2DPhysicalProperties::commitState(void)
  {
    int retval= UniaxialMatPhysicalProperties::commitState();
    retval+= theDamages.commitState(theMaterial);
    return retval;
  }

int XC::Joint2DPhysicalProperties::revertToLastCommit(void)
  {
    int retval= UniaxialMatPhysicalProperties::revertToLastCommit();
    retval+= theDamages.revertToLastCommit();
    return retval;
  }

int XC::Joint2DPhysicalProperties::revertToStart(void)
  { 
    int retval= UniaxialMatPhysicalProperties::revertToStart();
    retval+= theDamages.revertToStart();
    return retval;
  }

#define DBTAGS_SIZE 4
XC::DbTagData &XC::Joint2DPhysicalProperties::getDbTagData(void) const
  {
    static DbTagData retval(DBTAGS_SIZE); 
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::Joint2DPhysicalProperties::sendData(Communicator &comm)
  {
    int res= UniaxialMatPhysicalProperties::sendData(comm);
    res+= comm.sendMovable(theDamages,getDbTagData(),CommMetaData(2));
    res+= comm.sendID(fixedEnd,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::Joint2DPhysicalProperties::recvData(const Communicator &comm)
  {
    int res= UniaxialMatPhysicalProperties::recvData(comm);
    res+= comm.receiveMovable(theDamages,getDbTagData(),CommMetaData(2));
    res+= comm.receiveID(fixedEnd,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object.
int XC::Joint2DPhysicalProperties::sendSelf(Communicator &comm)
  {
    inicComm(DBTAGS_SIZE);

    int res= this->sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Joint2DPhysicalProperties::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::Joint2DPhysicalProperties::recvSelf(const Communicator &comm)
  {
    inicComm(DBTAGS_SIZE);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Joint2DPhysicalProperties::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(comm);
    return res;
  }

