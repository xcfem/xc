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
void XC::Joint2DPhysicalProperties::set_springs(const UniaxialMaterial &spring1, const UniaxialMaterial &spring2, const UniaxialMaterial &spring3, const UniaxialMaterial &spring4, const UniaxialMaterial &springC)
  {
    fixedEnd[0] = 0; theMaterial[0]= spring1.getCopy();
    fixedEnd[1] = 0; theMaterial[1]= spring2.getCopy();
    fixedEnd[2] = 0; theMaterial[2]= spring3.getCopy();
    fixedEnd[3] = 0; theMaterial[3]= spring4.getCopy();
    fixedEnd[4] = 0; theMaterial[4]= springC.getCopy();

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
void XC::Joint2DPhysicalProperties::set_damage_models(const DamageModel &dmg1, const DamageModel &dmg2, const DamageModel &dmg3, const DamageModel &dmg4, const DamageModel &dmgC)
  {
    theDamages[0] = dmg1.getCopy();
    theDamages[1] = dmg2.getCopy();
    theDamages[2] = dmg3.getCopy();
    theDamages[3] = dmg4.getCopy();
    theDamages[4] = dmgC.getCopy();
  }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : UniaxialMatPhysicalProperties(nMat,ptr_mat), theDamages(5,nullptr), fixedEnd(5)
  {
    for(int i=0;i<5;i++)
      fixedEnd[i] = 1;
  }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const UniaxialMaterial &spring1, const UniaxialMaterial &spring2, const UniaxialMaterial &spring3, const UniaxialMaterial &spring4, const UniaxialMaterial &springC)
  : UniaxialMatPhysicalProperties(5,nullptr), theDamages(5,nullptr), fixedEnd(5)
  {
    set_springs(spring1,spring2,spring3,spring4,springC);
  }

//! @brief Constructor
XC::Joint2DPhysicalProperties::Joint2DPhysicalProperties(const UniaxialMaterial &spring1, const UniaxialMaterial &spring2, const UniaxialMaterial &spring3, const UniaxialMaterial &spring4, const UniaxialMaterial &springC, const DamageModel &dmg1, const DamageModel &dmg2, const DamageModel &dmg3, const DamageModel &dmg4, const DamageModel &dmgC)
  : UniaxialMatPhysicalProperties(5,nullptr), theDamages(5,nullptr), fixedEnd(5)
  {
    // make copy of the uniaxial materials for the element

    set_springs(spring1,spring2,spring3,spring4,springC);
    set_damage_models(dmg1,dmg2,dmg3,dmg4,dmgC);
    theDamages.revertToStart(); //??
  }

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

//! @brief Send members through the channel being passed as parameter.
int XC::Joint2DPhysicalProperties::sendData(CommParameters &cp)
  {
    int res= UniaxialMatPhysicalProperties::sendData(cp);
    res+= cp.sendMovable(theDamages,getDbTagData(),CommMetaData(2));
    res+= cp.sendID(fixedEnd,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Joint2DPhysicalProperties::recvData(const CommParameters &cp)
  {
    int res= UniaxialMatPhysicalProperties::recvData(cp);
    res+= cp.receiveMovable(theDamages,getDbTagData(),CommMetaData(2));
    res+= cp.receiveID(fixedEnd,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object.
int XC::Joint2DPhysicalProperties::sendSelf(CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Joint2DPhysicalProperties::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::Joint2DPhysicalProperties::recvSelf(const CommParameters &cp)
  {
    inicComm(DBTAGS_SIZE);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Joint2DPhysicalProperties::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

