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
//GrupoCombinaciones.cc

#include "GrupoCombinaciones.h"
#include "preprocessor/loaders/LoadLoader.h"

#include "domain/load/pattern/Combinacion.h"
#include "domain/domain/Domain.h"



#include "utility/actor/actor/MovableMap.h"



//! @brief Default constructor.
XC::GrupoCombinaciones::GrupoCombinaciones(LoadLoader *owr)
  : LoadLoaderMember(owr) {}


//! @brief Returns a pointer to the la combinación cuyo nombre se pasa como parámetro.
XC::Combinacion *XC::GrupoCombinaciones::busca_combinacion(const std::string &comb_code)
  {
    Combinacion *retval= nullptr;
    const_iterator icomb= find(comb_code);
    if(icomb!=end())
      retval= icomb->second;
    return retval;
  }

//! @brief Returns a pointer to the la combinación cuyo nombre se pasa como parámetro.
const XC::Combinacion *XC::GrupoCombinaciones::buscaCombinacion(const std::string &comb_code) const
  {
    Combinacion *retval= nullptr;
    const_iterator icomb= find(comb_code);
    if(icomb!=end())
      retval= icomb->second;
    return retval;
  }

//! @brief Devuelve el nombre de la combinación a la que apunta el parámetro.
const std::string &XC::GrupoCombinaciones::getNombreCombinacion(const Combinacion *ptr) const
  {
    static std::string retval;
    retval= "";
    for(const_iterator i= begin();i!=end();i++)
      if((*i).second == ptr)
        {
          retval= (*i).first;
          break;
        }
    return retval;
  }

//! @brief Elimina la combinación del domain.
void XC::GrupoCombinaciones::remove(const std::string &comb_code)
  {
    Combinacion *comb= nullptr;
    iterator icomb= find(comb_code);
    if(icomb!=end())
      comb= icomb->second;
    if(comb)
      {
        getDomain()->removeCombinacion(comb);
        delete comb;
        (*icomb).second= nullptr;
        erase(icomb);
      }
    else
      std::cerr << "GrupoCombinaciones::remove: no se encontró la combinación: " 
                << comb_code << std::endl;
  }

//! @brief Agrega la combinación al domain.
void XC::GrupoCombinaciones::addToDomain(const std::string &comb_code)
  {
    Combinacion *comb= busca_combinacion(comb_code);
    if(comb)
      getDomain()->addCombinacion(comb);
    else
      std::cerr << "GrupoCombinaciones::addToDomain: no se encontró la combinación: " 
                << comb_code << std::endl;
  }

//! @brief Elimina la combinación del domain.
void XC::GrupoCombinaciones::removeFromDomain(const std::string &comb_code)
  {
    Combinacion *comb= busca_combinacion(comb_code);
    if(comb)
      getDomain()->removeCombinacion(comb);
    else
      std::cerr << "GrupoCombinaciones::removeFromDomain: no se encontró la combinación: " 
                << comb_code << std::endl;
  }

//! @brief Elimina las combinaciones del domain.
void XC::GrupoCombinaciones::removeAllFromDomain(void)
  {
    Domain *dom= getDomain();
    for(iterator i= begin();i!=end();i++)
      dom->removeCombinacion((*i).second);
  }

XC::Combinacion *XC::GrupoCombinaciones::newLoadCombination(const std::string &code,const std::string &descomp)
  {
    int tag_comb= getLoadLoader()->getTagLP();
    Combinacion *comb= busca_combinacion(code);
    if(comb) //La combinación ya existe.
      {
        std::cerr << "La combinación: " << code
                  << " ya existe, se redefine." << std::endl;
        comb->setDescomp(descomp);
      }
    else //La combinación es nueva.
      {
        comb= new Combinacion(this,code,tag_comb,getLoadLoader());
        getLoadLoader()->setTagLP(tag_comb+1);
        if(comb)
          {
            comb->setDomain(getDomain());
            comb->setDescomp(descomp);
            (*this)[comb->getNombre()]= comb;
          }
      }
    return comb;
  }

//! @brief Returns container's keys.
boost::python::list XC::GrupoCombinaciones::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=begin();i!=end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Borra todas las combinaciones.
void XC::GrupoCombinaciones::clear(void)
  {
    removeAllFromDomain();
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    map_combinaciones::clear();
  }

//! @brief Destructor.
XC::GrupoCombinaciones::~GrupoCombinaciones(void)
  { clear(); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::GrupoCombinaciones::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::GrupoCombinaciones::sendData(CommParameters &cp)
  { return sendMap(*this,cp,getDbTagData(),CommMetaData(0)); }

//! @brief Send members through the channel being passed as parameter.
int XC::GrupoCombinaciones::recvData(const CommParameters &cp)
  {
    clear();
    int res= receiveMap(*this,cp,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewCombinacion);
    for(iterator i= begin();i!= end();i++)
      {
        (*i).second->set_owner(this);
        (*i).second->setLoader(getLoadLoader());
        (*i).second->recvDescomp();
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::GrupoCombinaciones::sendSelf(CommParameters &cp)
  {
    inicComm(1);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "GrupoCombinaciones::sendSelf() - failed to send data.\n";    
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::GrupoCombinaciones::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    //Si no está vacío entendemos que valen las definiciones existentes.
    if(empty())
      {
        inicComm(1);
        const int dataTag= getDbTag();
        res= cp.receiveIdData(getDbTagData(),dataTag);
        if(res<0)
          std::cerr << "GrupoCombinaciones::recvSelf() - data could not be received.\n" ;
        else
          res+= recvData(cp);
      }
    return res;
  }

std::deque<std::string> XC::GrupoCombinaciones::getListaNombres(void) const
  {
    std::deque<std::string> retval;
    for(const_iterator i= begin();i!= end();i++)
      retval.push_back((*i).first);
    return retval;
  }

//! @brief Busca la combinación más próxima a la being passed as parameter
//! de entre las que tienen sus coeficientes de ponderación iguales o menores
//! que los de ésta.
XC::GrupoCombinaciones::const_iterator XC::GrupoCombinaciones::buscaCombPrevia(const Combinacion &c) const
  {
    const_iterator retval= end();
    Combinacion dif, tmpDif;
    for(const_iterator i= begin();i!= end();i++)
      {
        const Combinacion *tmp= (*i).second;
        if(c.dominaA(*tmp))
          {
            if(retval==end())
              {
                retval= i;
                dif= c-(*tmp);
              }
            else
              {
                tmpDif= c-(*tmp);
                if(tmpDif.size()<dif.size()) //Está más cerca.
                  {
                    retval= i;
                    dif= tmpDif; 
                  }
              }
	  }
      }
    return retval;
  }

const XC::Combinacion *XC::GrupoCombinaciones::getPtrCombPrevia(const Combinacion &c) const
  {
    const Combinacion *retval= nullptr;
    const_iterator i= buscaCombPrevia(c);
    if(i!=end())
      retval= (*i).second;
    return retval;
  }

XC::GrupoCombinaciones::const_iterator XC::GrupoCombinaciones::buscaCombPrevia(const std::string &nmb) const
  {
    const_iterator retval= end();
    const_iterator i= find(nmb);
    if(i!=end())
      retval= buscaCombPrevia(*i->second);
    return retval;
  }

const std::string XC::GrupoCombinaciones::getNombreCombPrevia(const std::string &nmb) const
  {
    std::string retval= "";
    const_iterator i= buscaCombPrevia(nmb);
    if(i!=end())
      retval= i->first;
    return retval;
  }

int XC::GrupoCombinaciones::getTagCombPrevia(const std::string &nmb) const
  {
    int retval= -1;
    const_iterator i= buscaCombPrevia(nmb);
    if(i!=end())
      retval= i->second->getTag();
    return retval;
  }
