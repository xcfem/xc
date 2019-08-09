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

#include "LoadPatternCombination.h"
#include "LoadPattern.h"
#include "domain/domain/Domain.h"
#include "xc_utils/src/utils/text/text_string.h"
#include "xc_utils/src/utils/text/StringFormatter.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "boost/lexical_cast.hpp"


#include "domain/load/pattern/MapLoadPatterns.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableString.h"
#include "utility/database/FE_Datastore.h"
#include "preprocessor/Preprocessor.h"

//used only to receive data.
std::map<int,std::string> XC::LoadPatternCombination::map_str_descomp;

//! @brief constructor.
XC::LoadPatternCombination::summand::summand(const float &f,LoadPattern *lp)
  : factor(f), lpattern(lp) {}

//! @brief Returns the factor that multiplies the summand.
const float &XC::LoadPatternCombination::summand::Factor(void) const
  { return factor; }

//! @brief Returns the LoadPattern corresponding to the summand.
const XC::LoadPattern *XC::LoadPatternCombination::summand::getLoadPattern(void) const
  { return lpattern; }

//! @brief Returns the LoadPattern corresponding to the summand.
XC::LoadPattern *XC::LoadPatternCombination::summand::getLoadPattern(void)
  { return lpattern; }

//! @brief Returns the name of the load case corresponding to the summand.
const std::string &XC::LoadPatternCombination::summand::getLoadPatternName(const MapLoadPatterns &lPatterns) const
  { return lPatterns.getLoadPatternName(lpattern); }

//! @brief Changes the sign of the summand.
void XC::LoadPatternCombination::summand::neg(void)
  { factor*=-1.0; }

//! @brief Returns the summand with sign changed.
XC::LoadPatternCombination::summand XC::LoadPatternCombination::summand::getNeg(void) const
  {
    summand tmp(*this);
    tmp.neg();
    return tmp;
  }

//! @brief Add to this summand the argument.
const XC::LoadPatternCombination::summand &XC::LoadPatternCombination::summand::add(const summand &other)
  {
    if(lpattern==other.lpattern)
      factor+= other.factor;
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible summands." << std::endl;
    return *this;
  }

//! @brief Substract from this summand the argument.
const XC::LoadPatternCombination::summand &XC::LoadPatternCombination::summand::substract(const summand &other)
  {
    if(lpattern==other.lpattern)
      factor-= other.factor;
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; incompatible summands." << std::endl;
    return *this;
  }

//! @brief Multiplies the summand by the value being passed as parameter.
const XC::LoadPatternCombination::summand &XC::LoadPatternCombination::summand::multiplica(const float &f)
  {
    factor*= f;
    return *this;
  }

//! @brief Divides the summand by the value being passed as parameter.
const XC::LoadPatternCombination::summand &XC::LoadPatternCombination::summand::divide(const float &f)
  {
    factor/= f;
    return *this;
  }

//! @brief Returns a string representation of the combination i.e. "1.35*G1".
//! @arg \c lPatterns: Load pattern container.
//! @arg \c fmt: Format for the factor.
std::string XC::LoadPatternCombination::summand::getString(const MapLoadPatterns &lPatterns,const std::string &fmt) const
  {
    std::string retval= "";
    if(fmt.empty())
      retval= boost::lexical_cast<std::string>(factor);
    else
      retval= format(fmt,factor);
    retval+= '*' + getLoadPatternName(lPatterns);
    return retval;
  }

//! @brief Imprime.
void XC::LoadPatternCombination::summand::Print(std::ostream &os) const
  {
    os << factor << '*';
    if(lpattern)
      os << 'C' << lpattern->getTag();
    else
      os << "nil";
 }


//! @brief Constructor
XC::LoadPatternCombination::LoadPatternCombination(int tag, int classTag, const std::string &nm, LoadHandler *ll)
  :ForceReprComponent(tag,classTag), name(nm), handler(ll)
  {}

//! @brief Destructor
XC::LoadPatternCombination::~LoadPatternCombination(void)
  { handler= nullptr; }

//! @brief Deletes the components of the load combination.
void XC::LoadPatternCombination::clear(void)
  { descomp.clear(); }

//! @brief Adds a component to the combination.
void XC::LoadPatternCombination::add_component(const summand &sum)
  {
    const LoadPattern *lp= sum.getLoadPattern();
    if((sum.Factor()!= 0.0) && lp)
      {
        iterator j= findLoadPattern(lp);
        if(j!=end())
          (*j).add(sum);
        else
          descomp.push_back(sum);
      }
  }

//! @brief Computes the combination from the string being passed as parameter.
void XC::LoadPatternCombination::interpreta_descomp(const std::string &str_descomp)
  {
    clear();
    typedef std::deque<std::string> dq_string;
    dq_string str_summands= separa_cadena(str_descomp,"+-");
    for(dq_string::iterator i= str_summands.begin();i!=str_summands.end();i++)
      {
        const std::string &str_sum_i= *i;
        dq_string str_prod= separa_cadena(str_sum_i,"*");
        const size_t sz= str_prod.size();
        if(sz!=2)
	  std::cerr << "Term: " << str_sum_i << " is incorrect." << std::endl;
        else
          {
            const float factor= boost::lexical_cast<float>(q_blancos(str_prod[0]));
            const std::string hypothesis_name= q_blancos(str_prod[1]);
            if(handler)
              {
                LoadPattern *lp= handler->getLoadPatterns().buscaLoadPattern(hypothesis_name);
                if(lp)
                  add_component(summand(factor,lp));
                else
	          std::cerr << getClassName() << "::" << __FUNCTION__
		            << " load case identified by: '" 
                            << hypothesis_name << "' not found.\n";
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; pointer to LoadHandler not set." << std::endl;
          } 
      }
  }


//! @brief Returns a const iterator pointing to the load pattern being passed as parameter.
XC::LoadPatternCombination::const_iterator XC::LoadPatternCombination::findLoadPattern(const LoadPattern *lp) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if(i->getLoadPattern()==lp)
        {
          retval= i;
          break;;
        }
    return retval;
  }

//! @brief Returns an iterator pointing to the load pattern being passed as parameter.
XC::LoadPatternCombination::iterator XC::LoadPatternCombination::findLoadPattern(const LoadPattern *lp)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if(i->getLoadPattern()==lp)
        {
          retval= i;
          break;;
        }
    return retval;
  }

//! @brief Deletes the null weighted load combinations.
void XC::LoadPatternCombination::limpia_ceros(void)
  {
    TDescomp nueva;
    for(iterator i= begin();i!=end();i++)
      if(i->Factor()!=0.0)
        nueva.push_back(*i);
    descomp= nueva;
  }

//! @brief Assigns the weightings for each load case of the combination.
void XC::LoadPatternCombination::set_gamma_f(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->getLoadPattern();
        if(lp)
          lp->GammaF()= i->Factor();
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Assigns the domain to each domain.
void XC::LoadPatternCombination::set_domain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->getLoadPattern();
        if(lp)
          lp->setDomain(dom);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Assigns the domain to the combination load patterns.
void XC::LoadPatternCombination::setDomain(Domain *theDomain)
  {
    ForceReprComponent::setDomain(theDomain);
    set_domain();
  }

//! @brief Adds to the domain being passed as parameter each of the load cases of the combination.
bool XC::LoadPatternCombination::addToDomain(void)
  {
    Domain *dom= getDomain();    
    assert(dom);
    bool retval= true;
    set_gamma_f();
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->getLoadPattern();
        if(lp)
          {
            bool result= dom->addLoadPattern(lp);
            if((!result) && (verbosity>3))
              {
                const MapLoadPatterns &lPatterns= handler->getLoadPatterns();
	        std::cerr << "Can't add load case: '"
                          << i->getLoadPatternName(lPatterns)
                          << "' when activating combination: '"
                          << getTag() << "'\n";
              }
            retval= (retval && result);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
    return retval;
  }

//! @brief Removes from the domain being passed as parameter the load cases of the combination.
void XC::LoadPatternCombination::removeFromDomain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->getLoadPattern();
        if(lp)
          dom->removeLoadPattern(lp);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Returns a string that represents the load combination
//! «1.35*G1+0.90*G1».
//! @arg \c fmt: Format for the factor that multiplies the load case.
std::string XC::LoadPatternCombination::getString(const std::string &fmt) const
  {
    std::string retval= "";
    const MapLoadPatterns &lPatterns= handler->getLoadPatterns();
    if(!empty())
      {
        const_iterator i= begin();
        retval= (*i).getString(lPatterns,fmt);
        i++;
        for(;i!=end();i++)
          retval+= '+' + (*i).getString(lPatterns,fmt);
      }
    return retval;
  }

//! @brief Returns the weighting factor for the load case
//! being passed as parameter.
float XC::LoadPatternCombination::getLoadPatternFactor(const LoadPattern *lp) const
  {
    float retval= 0.0;
    const_iterator i= findLoadPattern(lp);
    if(i!=end())
      retval= (*i).Factor();
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::LoadPatternCombination::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadPatternCombination::sendData(CommParameters &cp)
  {
    int res= ForceReprComponent::sendData(cp);
    res+= cp.sendString(name,getDbTagData(),CommMetaData(2));
    res+= cp.sendString(getString(),getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::LoadPatternCombination::recvData(const CommParameters &cp)
  {
    int res= ForceReprComponent::recvData(cp);
    res+= cp.receiveString(name,getDbTagData(),CommMetaData(2));
    //Teporary storage for decomposition.
    std::string tmp;
    res+= cp.receiveString(tmp,getDbTagData(),CommMetaData(3));
    map_str_descomp[getDbTag()]= tmp;
    //Decomposition is established later (in LoadCombinationGroup::recvData),
    //after setting up the object's owner and the pointer to LoadHandler.
    return res;
  }

//! @brief Imprime.
void XC::LoadPatternCombination::Print(std::ostream &s, int flag) const
  { s << getString(); }

std::ostream &XC::operator<<(std::ostream &os,const LoadPatternCombination &c)
  {
    c.Print(os);
    return os;
  }
