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
#include "DamageModel.h"

//! @brief Default constructor.
XC::DamageModelVector::DamageModelVector(const size_t &nDamageModels,const DamageModel *dmgModel)
  : std::vector<DamageModel *>(nDamageModels,nullptr), MovableObject(0)
  {
    if(dmgModel)
      {
        for(iterator i= dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
          {
            (*i)= dmgModel->getCopy();
            if(!(*i))
              std::cerr<<" DamageModelVector -- failed allocate damage model model pointer\n";
          }
      }
  }

//! @brief Copia los dmg_models.
void XC::DamageModelVector::alloc(const std::vector<DamageModel *> &dmgs)
  {
    clearAll();
    const size_t nDamageModels= dmgs.size();
    this->resize(nDamageModels);
    for(size_t i= 0;i<nDamageModels;i++)
      {
        if(dmgs[i])
          {
            (*this)[i]= dmgs[i]->getCopy();
            if(!(*this)[i])
              std::cerr<<" DamageModelVector -- failed allocate damage model pointer\n";
          }
      }
  }

//! @brief Copy constructor.
XC::DamageModelVector::DamageModelVector(const DamageModelVector &otro)
  : std::vector<DamageModel *>(otro.size(),nullptr), MovableObject(0)
  { alloc(otro); }

//! @brief Assignment operator.
XC::DamageModelVector &XC::DamageModelVector::operator=(const DamageModelVector &otro)
  { 
    alloc(otro);
    return *this;
  }

void XC::DamageModelVector::setDamageModel(const DamageModel *new_dmg)
  {
    borra_dmg_models();
    if(new_dmg)
      {
        for(iterator i= dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
          {
            (*i)= new_dmg->getCopy();
            if(!(*i))
              std::cerr<<" DamageModelVector -- failed allocate damages model pointer\n";
          }
      }
  }

void XC::DamageModelVector::setDamageModel(size_t i,DamageModel *dmg)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= dmg;
  }

void XC::DamageModelVector::borra_dmg_models(void)
  {
    for(iterator i= dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
      {
        if(*i)
          {
            delete (*i);
            (*i)= nullptr;
          }
      }
  }

//! @brief Returns true ifno se ha asignado damage model.
bool XC::DamageModelVector::empty(void) const
  {
    if(dmg_model_vector::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

void XC::DamageModelVector::clearAll(void)
  {
    borra_dmg_models();
    std::vector<DamageModel *>::clear();
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

//! @brief Returns the damage models to its last commited state.
int XC::DamageModelVector::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
      {
        DamageModel *dmg= *i;
        if(dmg)
         retVal+= dmg->revertToLastCommit();
        //if(retval!=0) break;
      }
    return retVal;
  }


//! @brief Returns the estado de los dmg_models al inicial.
int XC::DamageModelVector::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
      {
        DamageModel *dmg= *i;
        if(dmg)
         retVal+= dmg->revertToStart();
        //if(retval!=0) break;
      }
    return retVal;
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
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
