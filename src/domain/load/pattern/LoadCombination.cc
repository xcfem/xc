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
//LoadCombination.cc

#include "LoadCombination.h"
#include "LoadPattern.h"
#include "domain/domain/Domain.h"
#include "utility/utils/text/text_string.h"
#include "utility/utils/text/StringFormatter.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "boost/lexical_cast.hpp"


#include "domain/load/pattern/MapLoadPatterns.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableString.h"
#include "utility/database/FE_Datastore.h"
#include "preprocessor/Preprocessor.h"


//! @brief Constructor
XC::LoadCombination::LoadCombination(LoadCombinationGroup *owr,const std::string &name,int tag,LoadHandler *ll)
  :LoadPatternCombination(tag,LOAD_TAG_LoadCombination,name,ll)
  { set_owner(owr); }

//! @brief Returns the group to which the combination belongs.
const XC::LoadCombinationGroup *XC::LoadCombination::getGroup(void) const
  { return dynamic_cast<const LoadCombinationGroup *>(Owner()); }
    
//! @brief Returns the group to which the combination belongs.
XC::LoadCombinationGroup *XC::LoadCombination::getGroup(void)
  { return dynamic_cast<LoadCombinationGroup *>(Owner()); }

//! @brief Returns, if possible, a pointer to the "previous" combination.
const XC::LoadCombination *XC::LoadCombination::getPtrCombPrevia(void) const
  {
    const LoadCombination *retval= nullptr;
    const LoadCombinationGroup *g= getGroup();
    if(g)
      retval= g->getPtrCombPrevia(*this);
    return retval;
  }

//! @brief Returns, if possible, the name of the "previous" combination.
const std::string XC::LoadCombination::getNombreCombPrevia(void) const
  {
    std::string retval;
    const LoadCombination *c= getPtrCombPrevia();
    if(c)
      retval= c->getName();
    return retval;
  }

//! @brief Returns, if possible, the tag of the "previous" combination.
int XC::LoadCombination::getTagCombPrevia(void) const
  {
    int retval= -1;
    const LoadCombination *c= getPtrCombPrevia();
    if(c)
      retval= c->getTag();
    return retval;
  }

//! @brief Returns, if possible, the decomposition of the "previous" combination.
const std::string XC::LoadCombination::getDescompCombPrevia(void) const
  {
    std::string retval;
    const LoadCombination *c= getPtrCombPrevia();
    if(c)
      retval= c->getString();
    return retval;
  }

//! @brief Returns, si puede, la diferencia entre esta y la previa.
const std::string XC::LoadCombination::getDescompRestoSobrePrevia(void) const
  {
    std::string retval;
    const LoadCombination *c= getPtrCombPrevia();
    if(c)
      {
        LoadCombination dif(*this);
        dif.subtract(*c);
        retval= dif.getString();
      }
    return retval;
  }


//! @brief Returns the combination decomposition
//! (it must be called only after setting un the object's owner
//! and the pointer to the load handler -LoadHandler-).
int XC::LoadCombination::recvDescomp(void)
  {
    assert(Owner());
    assert(handler);
    const int dataTag= getDbTag();
    const std::string &desc= map_str_descomp[dataTag];
    interpreta_descomp(desc);
    map_str_descomp.erase(dataTag);
    return 0;
  }

//! @brief Sends object through the communicator argument.
int XC::LoadCombination::sendSelf(Communicator &comm)
  {
    inicComm(4);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";    
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::LoadCombination::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Sums the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::add(const LoadCombination &other)
  {
    for(const_iterator i= other.begin();i!=other.end();i++)
      add_component(*i);
    limpia_ceros();
    return *this;
  }

//! @brief Add the combination identified by the name being passed as parameter.
XC::LoadCombination &XC::LoadCombination::add(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const LoadCombination *cmb= handler->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          add(*cmb);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; load combination identified by: '" 
                    << nmbComb << "' not found.\n";
      }
    return *this;
  }


//! @brief Subtracts the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::subtract(const LoadCombination &other)
  {
    for(const_iterator i= other.begin();i!=other.end();i++)
      add_component((*i).getNeg());
    limpia_ceros();
    return *this;
  }

//! @brief Subtracts the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::subtract(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const LoadCombination *cmb= handler->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          subtract(*cmb);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; load combination identified by: '" 
                    << nmbComb << "' not found.\n";
      }
    return *this;
  }

//! @brief Assigns the combination identified by the name being passed as parameter.
XC::LoadCombination &XC::LoadCombination::asigna(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const LoadCombination *cmb= handler->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          (*this)= *cmb;
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; load combination identified by: '" 
                    << nmbComb << "' not found.\n";
      }
    return *this;
  }

//! @brief Multiplies the combination by the number being passed as parameter.
XC::LoadCombination &XC::LoadCombination::multiplica(const float &f)
  {
    if(f!=0.0)
      for(iterator i= begin();i!=end();i++)
        (*i).multiplica(f);
    else
      clear();
    return *this;
  }


//! @brief Divides the combination by the number being passed as parameter.
XC::LoadCombination &XC::LoadCombination::divide(const float &f)
  {
    for(iterator i= begin();i!=end();i++)
      (*i).divide(f);
    return *this;
  }

//! @brief Addition operator.
XC::LoadCombination XC::LoadCombination::operator+(const LoadCombination &c) const
  {
    LoadCombination retval(*this);
    retval+= c;
    return retval;
  }

//! @brief Subtraction operator.
XC::LoadCombination XC::LoadCombination::operator-(const LoadCombination &c) const
  {
    LoadCombination retval(*this);
    retval-= c;
    return retval;
  }
 
//! @brief Product by a number operator.
XC::LoadCombination XC::LoadCombination::operator*(const float &fact) const
  {
    LoadCombination retval(*this);
    retval*= fact;
    return retval;
  }

//! @brief Division by a number operator.
XC::LoadCombination XC::LoadCombination::operator/(const float &fact) const
  {
    LoadCombination retval(*this);
    retval/= fact;
    return retval;
  }

//! @brief Not equal operator.
bool XC::LoadCombination::operator!=(const LoadCombination &other) const
  {
    bool retval= false;
    for(const_iterator i= begin();i!=end();i++)
      {
        const LoadPattern *lPattern= (*i).getLoadPattern();
        const float f1= (*i).Factor();
        const float f2= other.getLoadPatternFactor(lPattern);
        if(f1!=f2)
          {
            retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Equal operator.
bool XC::LoadCombination::operator==(const LoadCombination &other) const
  {
    bool retval= true;
    for(const_iterator i= begin();i!=end();i++)
      {
        const LoadPattern *lPattern= (*i).getLoadPattern();
        const float f1= (*i).Factor();
        const float f2= other.getLoadPatternFactor(lPattern);
        if(f1!=f2)
          {
            retval= false;
            break;
          }
      }
    return retval;
  }

//! @brief Returns true if the factors that weight all the
//! load patterns of this load combination are greater that
//! those in the combination being passed as parameter.
bool XC::LoadCombination::dominaA(const LoadCombination &other) const
  {
    bool retval= true;
    if(this == &other)
      retval= false;
    else if(other.size()>size())
      retval= false;
    else
      {
        for(const_iterator i= begin();i!=end();i++)
          {
            const LoadPattern *lPattern= (*i).getLoadPattern();
            const float f1= (*i).Factor();
            const float f2= other.getLoadPatternFactor(lPattern);
            if(f1<f2)
              {
                retval= false;
                break;
              }
          }
        for(const_iterator i= other.begin();i!=other.end();i++)
          {
            const LoadPattern *lPattern= (*i).getLoadPattern();
            const float f1= getLoadPatternFactor(lPattern);
            const float f2= (*i).Factor();
            if(f1<f2)
              {
                retval= false;
                break;
              }
          }
      }
    return retval;
  }

//! @brief ostream insertion operator.
std::ostream &XC::operator<<(std::ostream &os,const LoadCombination &c)
  {
    c.Print(os);
    return os;
  }
