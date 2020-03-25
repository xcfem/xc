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
//DamageModelVector.cc

#include "DamageModelVector.h"
#include "material/damage/DamageModel.h"

//! @brief Default constructor.
XC::DamageModelVector::DamageModelVector(const size_t &nDamageModels,const DamageModel *dmgModel)
  : DamageModels(nDamageModels, dmgModel), MovableObject(0)
  {}

//! @brief Removes all members.
void XC::DamageModelVector::clearAll(void)
  {
    DamageModels::clear();
  }


//! @brief Consuma el estado de los dmg_models.
int XC::DamageModelVector::commitState(const material_vector &materials)
  {
    int result= 0;
    const size_t sz= size();
    if(materials.size()!=sz)
      std::cerr << "DamageModelVector: wrong size in materials vector." << std::endl;
    else
      {
        // setting the trial state for the damage models
        Vector InforForDamage(3);

        int result= 0;
        for(size_t i=0;i<sz;i++)
          {
            if(result!=0) break;
            if(materials[i] != nullptr && (*this)[i] != nullptr )
              {
                InforForDamage(0) = materials[i]->getStrain();
                InforForDamage(1) = materials[i]->getStress();
                InforForDamage(2) = materials[i]->getInitialTangent();

                (*this)[i]->setTrial(InforForDamage);
                result+= (*this)[i]->commitState();
                if(result!=0) break;
              }
          }
      }
    return result;
  }





//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::DamageModelVector::getDbTagData(void) const
  {
    static DbTagData retval(2);
    std::cerr << "XC::DamageModelVector::getDbTagData not implemented." << std::endl;
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::DamageModelVector::sendData(CommParameters &cp)
  {
    int res= 0;
    std::cerr << "XC::DamageModelVector::sendData not implemented." << std::endl;
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DamageModelVector::recvData(const CommParameters &cp)
  {
    int res= 0;
    //const int flag = getDbTagDataPos(0);
    std::cerr << "XC::DamageModelVector::recvData not implemented." << std::endl;
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DamageModelVector::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int res= sendData(cp);
    const int dataTag=getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING XC::DamageModelVector::sendSelf() - " 
                << dataTag << " failed to send ID";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DamageModelVector::recvSelf(const CommParameters &cp)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "WARNING XC::DamageModelVector::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }
