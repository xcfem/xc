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
//DamageModelVector.cc

#include "DamageModelVector.h"
#include "DamageModel.h"

//! @brief Constructor por defecto.
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

//! @brief Constructor de copia.
XC::DamageModelVector::DamageModelVector(const DamageModelVector &otro)
  : std::vector<DamageModel *>(otro.size(),nullptr), MovableObject(0)
  { alloc(otro); }

//! @brief Operador asignación.
XC::DamageModelVector &XC::DamageModelVector::operator=(const DamageModelVector &otro)
  { 
    alloc(otro);
    return *this;
  }

void XC::DamageModelVector::setDamageModel(const DamageModel *nuevo_dmg)
  {
    borra_dmg_models();
    if(nuevo_dmg)
      {
        for(iterator i= dmg_model_vector::begin();i!=dmg_model_vector::end();i++)
          {
            (*i)= nuevo_dmg->getCopy();
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

//! @brief Devuelve verdadero si no se ha asignado damage model.
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

//! @brief Devuelve el estado de los dmg_models al del último commit.
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


//! @brief Devuelve el estado de los dmg_models al inicial.
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

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::DamageModelVector::getDbTagData(void) const
  {
    static DbTagData retval(2);
    std::cerr << "XC::DamageModelVector::getDbTagData not implemented." << std::endl;
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DamageModelVector::sendData(CommParameters &cp)
  {
    int res= 0;
    std::cerr << "XC::DamageModelVector::sendData not implemented." << std::endl;
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::DamageModelVector::recvData(const CommParameters &cp)
  {
    int res= 0;
    //const int flag = getDbTagDataPos(0);
    std::cerr << "XC::DamageModelVector::recvData not implemented." << std::endl;
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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
