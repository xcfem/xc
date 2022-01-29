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
//LoadCombinationGroup.cc


#include "LoadCombinationGroup.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "domain/load/pattern/LoadCombination.h"
#include "domain/domain/Domain.h"
#include "utility/actor/actor/MovableMap.h"

//! @brief Default constructor.
XC::LoadCombinationGroup::LoadCombinationGroup(LoadHandler *owr)
  : LoadHandlerMember(owr) {}


//! @brief Returns a pointer to the load combination identified by the
//! name being passed as parameter.
XC::LoadCombination *XC::LoadCombinationGroup::find_combination(const std::string &comb_code)
  {
    LoadCombination *retval= nullptr;
    const_iterator icomb= find(comb_code);
    if(icomb!=end())
      retval= icomb->second;
    return retval;
  }

//! @brief Returns a pointer to the load combination identified by the
//! name being passed as parameter.
const XC::LoadCombination *XC::LoadCombinationGroup::buscaLoadCombination(const std::string &comb_code) const
  {
    LoadCombination *retval= nullptr;
    const_iterator icomb= find(comb_code);
    if(icomb!=end())
      retval= icomb->second;
    return retval;
  }

//! @brief Returns the name of the load combination pointed by the parameter.
const std::string &XC::LoadCombinationGroup::getNombreLoadCombination(const LoadCombination *ptr) const
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

//! @brief Removes the combination from the domain.
void XC::LoadCombinationGroup::remove(const std::string &comb_code)
  {
    LoadCombination *comb= nullptr;
    iterator icomb= find(comb_code);
    if(icomb!=end())
      comb= icomb->second;
    if(comb)
      {
        getDomain()->removeLoadCombination(comb);
        delete comb;
        (*icomb).second= nullptr;
        erase(icomb);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; load combination: '" 
                << comb_code << "' not found." << std::endl;
  }

//! @brief Adds the combination to the domain.
void XC::LoadCombinationGroup::addToDomain(const std::string &comb_code)
  {
    LoadCombination *comb= find_combination(comb_code);
    if(comb)
      getDomain()->addLoadCombination(comb);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; load combination: '" 
                << comb_code << "' not found." << std::endl;
  }

//! @brief Removes the combination from the domain.
void XC::LoadCombinationGroup::removeFromDomain(const std::string &comb_code)
  {
    LoadCombination *comb= find_combination(comb_code);
    if(comb)
      getDomain()->removeLoadCombination(comb);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; load combination: '" 
                << comb_code << "' not found." << std::endl;
  }

//! @brief Remomves all the load combinations from the domain.
void XC::LoadCombinationGroup::removeAllFromDomain(void)
  {
    Domain *dom= getDomain();
    for(iterator i= begin();i!=end();i++)
      dom->removeLoadCombination((*i).second);
  }

XC::LoadCombination *XC::LoadCombinationGroup::newLoadCombination(const std::string &code, const std::string &descomp)
  {
    int tag_comb= getLoadHandler()->getTagLP();
    LoadCombination *comb= find_combination(code);
    if(comb) //Load combination already exists.
      {
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; load combination: " << code
                  << " already exists, will be redefined." << std::endl;
        bool ok= comb->setDescomp(descomp);
	if(!ok)
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; when redefining load combination: " << code
                      << " definition: '"
	              << descomp << "' was incorrect." << std::endl;
      }
    else //New combination
      {
        comb= new LoadCombination(this,code,tag_comb,getLoadHandler());
        getLoadHandler()->setTagLP(tag_comb+1);
        if(comb)
          {
            comb->setDomain(getDomain());
            bool ok= comb->setDescomp(descomp);
	    if(ok)
                (*this)[comb->getName()]= comb;
	    else
	      {
	        std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; could not create load combination: " << code
                          << " definition: '"
	                  << descomp << "' was incorrect." << std::endl;
		delete comb;
		comb= nullptr;
	      }
          }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; could not create load combination: " << code
                      << " defined as: '"
	              << descomp << "'." << std::endl;
	  }
      }
    return comb;
  }

//! @brief Returns container's keys.
boost::python::list XC::LoadCombinationGroup::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=begin();i!=end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Deletes all the combinations.
void XC::LoadCombinationGroup::clear(void)
  {
    removeAllFromDomain();
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    LoadCombinationMap::clear();
  }

//! @brief Destructor.
XC::LoadCombinationGroup::~LoadCombinationGroup(void)
  { clear(); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::LoadCombinationGroup::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::LoadCombinationGroup::sendData(Communicator &comm)
  { return sendMap(*this,comm,getDbTagData(),CommMetaData(0)); }

//! @brief Send members through the communicator argument.
int XC::LoadCombinationGroup::recvData(const Communicator &comm)
  {
    clear();
    int res= receiveMap(*this,comm,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewLoadCombination);
    for(iterator i= begin();i!= end();i++)
      {
        (*i).second->set_owner(this);
        (*i).second->setHandler(getLoadHandler());
        (*i).second->recvDescomp();
      }
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::LoadCombinationGroup::sendSelf(Communicator &comm)
  {
    inicComm(1);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadCombinationGroup::sendSelf() - failed to send data.\n";    
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::LoadCombinationGroup::recvSelf(const Communicator &comm)
  {
    int res= 0;
    //If it's not empty we suppose that current definitions are OK.
    if(empty())
      {
        inicComm(1);
        const int dataTag= getDbTag();
        res= comm.receiveIdData(getDbTagData(),dataTag);
        if(res<0)
          std::cerr << "LoadCombinationGroup::recvSelf() - data could not be received.\n" ;
        else
          res+= recvData(comm);
      }
    return res;
  }

std::deque<std::string> XC::LoadCombinationGroup::getNamesList(void) const
  {
    std::deque<std::string> retval;
    for(const_iterator i= begin();i!= end();i++)
      retval.push_back((*i).first);
    return retval;
  }

//! @brief Search for the nearest combination to the one
//! being passed as parameter from those that have their
//! factors equal or lesser than those of the parameter (c).
XC::LoadCombinationGroup::const_iterator XC::LoadCombinationGroup::buscaCombPrevia(const LoadCombination &c) const
  {
    const_iterator retval= end();
    LoadCombination dif, tmpDif;
    for(const_iterator i= begin();i!= end();i++)
      {
        const LoadCombination *tmp= (*i).second;
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
                if(tmpDif.size()<dif.size()) //It's closer.
                  {
                    retval= i;
                    dif= tmpDif; 
                  }
              }
	  }
      }
    return retval;
  }

const XC::LoadCombination *XC::LoadCombinationGroup::getPtrCombPrevia(const LoadCombination &c) const
  {
    const LoadCombination *retval= nullptr;
    const_iterator i= buscaCombPrevia(c);
    if(i!=end())
      retval= (*i).second;
    return retval;
  }

XC::LoadCombinationGroup::const_iterator XC::LoadCombinationGroup::buscaCombPrevia(const std::string &nmb) const
  {
    const_iterator retval= end();
    const_iterator i= find(nmb);
    if(i!=end())
      retval= buscaCombPrevia(*i->second);
    return retval;
  }

const std::string XC::LoadCombinationGroup::getNombreCombPrevia(const std::string &nmb) const
  {
    std::string retval= "";
    const_iterator i= buscaCombPrevia(nmb);
    if(i!=end())
      retval= i->first;
    return retval;
  }

int XC::LoadCombinationGroup::getTagCombPrevia(const std::string &nmb) const
  {
    int retval= -1;
    const_iterator i= buscaCombPrevia(nmb);
    if(i!=end())
      retval= i->second->getTag();
    return retval;
  }
