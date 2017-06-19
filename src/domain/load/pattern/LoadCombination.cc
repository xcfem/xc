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
#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_basic/src/texto/StringFormatter.h"
#include "preprocessor/loaders/LoadLoader.h"
#include "boost/lexical_cast.hpp"


#include "domain/load/pattern/MapLoadPatterns.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableString.h"
#include "utility/database/FE_Datastore.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//used only to receive data.
std::map<int,std::string> XC::LoadCombination::map_str_descomp;

//! @brief constructor.
XC::LoadCombination::sumando::sumando(const float &f,LoadPattern *lp)
  : factor(f), lpattern(lp) {}

//! @brief Returns the factor que multiplica al sumando.
const float &XC::LoadCombination::sumando::Factor(void) const
  { return factor; }

//! @brief Returns the LoadPattern al que se refiere el sumando.
const XC::LoadPattern *XC::LoadCombination::sumando::Caso(void) const
  { return lpattern; }

//! @brief Returns the LoadPattern al que se refiere el sumando.
XC::LoadPattern *XC::LoadCombination::sumando::Caso(void)
  { return lpattern; }

//! @brief Returns the nombre del caso al que se refiere el sumando.
const std::string &XC::LoadCombination::sumando::getNombreCaso(const MapLoadPatterns &casos) const
  { return casos.getNombreLoadPattern(lpattern); }

//! @brief Cambia de signo el sumando.
void XC::LoadCombination::sumando::neg(void)
  { factor*=-1.0; }

//! @brief Returns the sumando cambiado de signo.
XC::LoadCombination::sumando XC::LoadCombination::sumando::getNeg(void) const
  {
    sumando tmp(*this);
    tmp.neg();
    return tmp;
  }

//! @brief Suma a éste sumando el being passed as parameter.
const XC::LoadCombination::sumando &XC::LoadCombination::sumando::suma(const sumando &otro)
  {
    if(lpattern==otro.lpattern)
      factor+= otro.factor;
    else
      std::cerr << "Sumandos incompatibles en suma." << std::endl;
    return *this;
  }

//! @brief Resta a éste sumando el being passed as parameter.
const XC::LoadCombination::sumando &XC::LoadCombination::sumando::resta(const sumando &otro)
  {
    if(lpattern==otro.lpattern)
      factor-= otro.factor;
    else
      std::cerr << "Sumandos incompatibles en resta." << std::endl;
    return *this;
  }

//! @brief Multiplica el sumando por el valor being passed as parameter.
const XC::LoadCombination::sumando &XC::LoadCombination::sumando::multiplica(const float &f)
  {
    factor*= f;
    return *this;
  }

//! @brief Divide el sumando por el valor being passed as parameter.
const XC::LoadCombination::sumando &XC::LoadCombination::sumando::divide(const float &f)
  {
    factor/= f;
    return *this;
  }

//! @brief Returns a string representation of the combination i.e. "1.35*G1".
//! @arg \c casos: Load pattern container.
//! @arg \c fmt: Format for the factor.
std::string XC::LoadCombination::sumando::getString(const MapLoadPatterns &casos,const std::string &fmt) const
  {
    std::string retval= "";
    if(fmt.empty())
      retval= boost::lexical_cast<std::string>(factor);
    else
      retval= format(fmt,factor);
    retval+= '*' + getNombreCaso(casos);
    return retval;
  }

//! @brief Imprime.
void XC::LoadCombination::sumando::Print(std::ostream &os) const
  {
    os << factor << '*';
    if(lpattern)
      os << 'C' << lpattern->getTag();
    else
      os << "nil";
 }


//! @brief Constructor
XC::LoadCombination::LoadCombination(LoadCombinationGroup *owr,const std::string &nmb,int tag,LoadLoader *ll)
  :ForceReprComponent(tag,LOAD_TAG_LoadCombination), loader(ll), nombre(nmb) 
  { set_owner(owr); }

//! @brief Destructor
XC::LoadCombination::~LoadCombination(void)
  { loader= nullptr; }

//! @brief Assigns the weightings for each load case of the combination.
void XC::LoadCombination::LoadCombination::set_gamma_f(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          lp->GammaF()= i->Factor();
        else
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Assigns the domain to each domain.
void XC::LoadCombination::set_domain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          lp->setDomain(dom);
        else
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Adds to the domain being passed as parameter each of the load cases of the combination.
bool XC::LoadCombination::addToDomain(void)
  {
    Domain *dom= getDomain();    
    assert(dom);
    bool retval= true;
    set_gamma_f();
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          {
            bool result= dom->addLoadPattern(lp);
            if((!result) && (verbosity>3))
              {
                const MapLoadPatterns &casos= loader->getLoadPatterns();
	        std::cerr << "Can't add load case: '"
                          << i->getNombreCaso(casos)
                          << "' when activating combination: '"
                          << getNombre() << "'\n";
              }
            retval= (retval && result);
          }
        else
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
    return retval;
  }

//! @brief Removes from the domain being passed as parameter the load cases of the combination.
void XC::LoadCombination::removeFromDomain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          dom->removeLoadPattern(lp);
        else
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; null pointer found in expression." << std::endl;
      }
  }

//! @brief Adds a component to the combination.
void XC::LoadCombination::add_component(const sumando &sum)
  {
    const LoadPattern *lp= sum.Caso();
    if((sum.Factor()!= 0.0) && lp)
      {
        iterator j= buscaCaso(lp);
        if(j!=end())
          (*j).suma(sum);
        else
          descomp.push_back(sum);
      }
  }

//! @brief Computes the combination from the string being passed as parameter.
void XC::LoadCombination::interpreta_descomp(const std::string &str_descomp)
  {
    clear();
    typedef std::deque<std::string> dq_string;
    dq_string str_sumandos= separa_cadena(str_descomp,"+-");
    for(dq_string::iterator i= str_sumandos.begin();i!=str_sumandos.end();i++)
      {
        const std::string &str_sum_i= *i;
        dq_string str_prod= separa_cadena(str_sum_i,"*");
        const size_t sz= str_prod.size();
        if(sz!=2)
	  std::cerr << "Term: " << str_sum_i << " is incorrect." << std::endl;
        else
          {
            const float factor= boost::lexical_cast<float>(q_blancos(str_prod[0]));
            const std::string nmb_hipot= q_blancos(str_prod[1]);
            if(loader)
              {
                LoadPattern *lp= loader->getLoadPatterns().buscaLoadPattern(nmb_hipot);
                if(lp)
                  add_component(sumando(factor,lp));
                else
	          std::cerr << nombre_clase() << "::" << __FUNCTION__
		            << " load case identified by: '" 
                            << nmb_hipot << "' not found.\n";
              }
            else
	      std::cerr << "LoadCombination::interpreta_descomp; se necesita a pointer to LoadLoader." << std::endl;
          } 
      }
  }


//! @brief Returns a const iterator pointing to the load pattern being passed as parameter.
XC::LoadCombination::const_iterator XC::LoadCombination::buscaCaso(const LoadPattern *lp) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if(i->Caso()==lp)
        {
          retval= i;
          break;;
        }
    return retval;
  }

//! @brief Returns an iterator pointing to the load pattern being passed as parameter.
XC::LoadCombination::iterator XC::LoadCombination::buscaCaso(const LoadPattern *lp)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if(i->Caso()==lp)
        {
          retval= i;
          break;;
        }
    return retval;
  }

//! @brief Deletes the components of the load combination.
void XC::LoadCombination::clear(void)
  { descomp.clear(); }

//! @brief Deletes the null weighted load combinations.
void XC::LoadCombination::limpia_ceros(void)
  {
    TDescomp nueva;
    for(iterator i= begin();i!=end();i++)
      if(i->Factor()!=0.0)
        nueva.push_back(*i);
    descomp= nueva;
  }

//! @brief Returns the group to wich the combination belongs.
const XC::LoadCombinationGroup *XC::LoadCombination::getGrupo(void) const
  { return dynamic_cast<const LoadCombinationGroup *>(Owner()); }
    

//! @brief Returns the group to wich the combination belongs.
XC::LoadCombinationGroup *XC::LoadCombination::getGrupo(void)
  { return dynamic_cast<LoadCombinationGroup *>(Owner()); }

//! @brief Returns, if possible, a pointer to the "previous" combination.
const XC::LoadCombination *XC::LoadCombination::getPtrCombPrevia(void) const
  {
    const LoadCombination *retval= nullptr;
    const LoadCombinationGroup *g= getGrupo();
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
      retval= c->getNombre();
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
        dif.resta(*c);
        retval= dif.getString();
      }
    return retval;
  }

//! @brief Assigns the domain to the combination load patterns.
void XC::LoadCombination::LoadCombination::setDomain(Domain *theDomain)
  {
    ForceReprComponent::setDomain(theDomain);
    set_domain();
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::LoadCombination::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadCombination::sendData(CommParameters &cp)
  {
    int res= ForceReprComponent::sendData(cp);
    res+= cp.sendString(nombre,getDbTagData(),CommMetaData(2));
    res+= cp.sendString(getString(),getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::LoadCombination::recvData(const CommParameters &cp)
  {
    int res= ForceReprComponent::recvData(cp);
    res+= cp.receiveString(nombre,getDbTagData(),CommMetaData(2));
    //Teporary storage for decomposition.
    std::string tmp;
    res+= cp.receiveString(tmp,getDbTagData(),CommMetaData(3));
    map_str_descomp[getDbTag()]= tmp;
    //Decomposition is established later (in LoadCombinationGroup::recvData),
    //after setting up the object's owner and the pointer to LoadLoader.
    return res;
  }

//! @brief Returns the combination decomposition
//! (it must be called only after setting un the object's owner
//! and the pointer to the load handler -LoadLoader-).
int XC::LoadCombination::recvDescomp(void)
  {
    assert(Owner());
    assert(loader);
    const int dataTag= getDbTag();
    const std::string &desc= map_str_descomp[dataTag];
    interpreta_descomp(desc);
    map_str_descomp.erase(dataTag);
    return 0;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::LoadCombination::sendSelf(CommParameters &cp)
  {
    inicComm(4);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadCombination::sendSelf() - failed to send data.\n";    
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::LoadCombination::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadCombination::recvSelf() - data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Sums the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::suma(const LoadCombination &otro)
  {
    for(const_iterator i= otro.begin();i!=otro.end();i++)
      add_component(*i);
    limpia_ceros();
    return *this;
  }

//! @brief Suma the combination identified by the name being passed as parameter.
XC::LoadCombination &XC::LoadCombination::suma(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const LoadCombination *cmb= loader->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          suma(*cmb);
        else
          std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; load combination identified by: '" 
                    << nmbComb << "' not found.\n";
      }
    return *this;
  }


//! @brief Substracts the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::resta(const LoadCombination &otro)
  {
    for(const_iterator i= otro.begin();i!=otro.end();i++)
      add_component((*i).getNeg());
    limpia_ceros();
    return *this;
  }

//! @brief Substracts the combination being passed as parameter.
XC::LoadCombination &XC::LoadCombination::resta(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const LoadCombination *cmb= loader->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          resta(*cmb);
        else
          std::cerr << nombre_clase() << "::" << __FUNCTION__
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
        const LoadCombination *cmb= loader->getLoadCombinations().buscaLoadCombination(nmbComb);
        if(cmb)
          (*this)= *cmb;
        else
          std::cerr << nombre_clase() << "::" << __FUNCTION__
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

//! @brief Substraction operator.
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

//! @brief Returns the weighting factor for the load case
//! being passed as parameter.
float XC::LoadCombination::getCoefCaso(const LoadPattern *lp) const
  {
    float retval= 0.0;
    const_iterator i= buscaCaso(lp);
    if(i!=end())
      retval= (*i).Factor();
    return retval;
  }

bool XC::LoadCombination::operator!=(const LoadCombination &otra) const
  {
    bool retval= false;
    for(const_iterator i= begin();i!=end();i++)
      {
        const LoadPattern *caso= (*i).Caso();
        const float f1= (*i).Factor();
        const float f2= otra.getCoefCaso(caso);
        if(f1!=f2)
          {
            retval= true;
            break;
          }
      }
    return retval;
  }

bool XC::LoadCombination::operator==(const LoadCombination &otra) const
  {
    bool retval= true;
    for(const_iterator i= begin();i!=end();i++)
      {
        const LoadPattern *caso= (*i).Caso();
        const float f1= (*i).Factor();
        const float f2= otra.getCoefCaso(caso);
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
bool XC::LoadCombination::dominaA(const LoadCombination &otra) const
  {
    bool retval= true;
    if(this == &otra)
      retval= false;
    else if(otra.size()>size())
      retval= false;
    else
      {
        for(const_iterator i= begin();i!=end();i++)
          {
            const LoadPattern *caso= (*i).Caso();
            const float f1= (*i).Factor();
            const float f2= otra.getCoefCaso(caso);
            if(f1<f2)
              {
                retval= false;
                break;
              }
          }
        for(const_iterator i= otra.begin();i!=otra.end();i++)
          {
            const LoadPattern *caso= (*i).Caso();
            const float f1= getCoefCaso(caso);
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

//! @brief Returns a string that represents the load combination
//! «1.35*G1+0.90*G1».
//! @arg \c fmt: Format for the factor that multiplies the load case.
std::string XC::LoadCombination::getString(const std::string &fmt) const
  {
    std::string retval= "";
    const MapLoadPatterns &casos= loader->getLoadPatterns();
    if(!empty())
      {
        const_iterator i= begin();
        retval= (*i).getString(casos,fmt);
        i++;
        for(;i!=end();i++)
          retval+= '+' + (*i).getString(casos,fmt);
      }
    return retval;
  }

//! @brief Imprime.
void XC::LoadCombination::Print(std::ostream &s, int flag) const
  { s << getString(); }

std::ostream &XC::operator<<(std::ostream &os,const LoadCombination &c)
  {
    c.Print(os);
    return os;
  }
