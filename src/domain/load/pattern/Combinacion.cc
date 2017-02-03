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
//Combinacion.cc

#include "Combinacion.h"
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

//sólo se usa para recibir datos.
std::map<int,std::string> XC::Combinacion::map_str_descomp;

//! @brief constructor.
XC::Combinacion::sumando::sumando(const float &f,LoadPattern *lp)
  : factor(f), lpattern(lp) {}

//! @brief Devuelve el factor que multiplica al sumando.
const float &XC::Combinacion::sumando::Factor(void) const
  { return factor; }

//! @brief Devuelve el LoadPattern al que se refiere el sumando.
const XC::LoadPattern *XC::Combinacion::sumando::Caso(void) const
  { return lpattern; }

//! @brief Devuelve el LoadPattern al que se refiere el sumando.
XC::LoadPattern *XC::Combinacion::sumando::Caso(void)
  { return lpattern; }

//! @brief Devuelve el nombre del caso al que se refiere el sumando.
const std::string &XC::Combinacion::sumando::getNombreCaso(const MapLoadPatterns &casos) const
  { return casos.getNombreLoadPattern(lpattern); }

//! @brief Cambia de signo el sumando.
void XC::Combinacion::sumando::neg(void)
  { factor*=-1.0; }

//! @brief Devuelve el sumando cambiado de signo.
XC::Combinacion::sumando XC::Combinacion::sumando::getNeg(void) const
  {
    sumando tmp(*this);
    tmp.neg();
    return tmp;
  }

//! @brief Suma a éste sumando el being passed as parameter.
const XC::Combinacion::sumando &XC::Combinacion::sumando::suma(const sumando &otro)
  {
    if(lpattern==otro.lpattern)
      factor+= otro.factor;
    else
      std::cerr << "Sumandos incompatibles en suma." << std::endl;
    return *this;
  }

//! @brief Resta a éste sumando el being passed as parameter.
const XC::Combinacion::sumando &XC::Combinacion::sumando::resta(const sumando &otro)
  {
    if(lpattern==otro.lpattern)
      factor-= otro.factor;
    else
      std::cerr << "Sumandos incompatibles en resta." << std::endl;
    return *this;
  }

//! @brief Multiplica el sumando por el valor being passed as parameter.
const XC::Combinacion::sumando &XC::Combinacion::sumando::multiplica(const float &f)
  {
    factor*= f;
    return *this;
  }

//! @brief Divide el sumando por el valor being passed as parameter.
const XC::Combinacion::sumando &XC::Combinacion::sumando::divide(const float &f)
  {
    factor/= f;
    return *this;
  }

//! @brief Devuelve una cadena de caracteres que representa al caso como puede ser
//! «1.35*G1».
//! @arg \c casos: Contenedor de los casos de carga.
//! @arg \c fmt: Formato para el factor que multiplica a la hipótesis.
std::string XC::Combinacion::sumando::getString(const MapLoadPatterns &casos,const std::string &fmt) const
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
void XC::Combinacion::sumando::Print(std::ostream &os) const
  {
    os << factor << '*';
    if(lpattern)
      os << 'C' << lpattern->getTag();
    else
      os << "nil";
 }


//! @brief Constructor
XC::Combinacion::Combinacion(GrupoCombinaciones *owr,const std::string &nmb,int tag,LoadLoader *ll)
  :ForceReprComponent(tag,LOAD_TAG_Combinacion), loader(ll), nombre(nmb) 
  { set_owner(owr); }

//! @brief Destructor
XC::Combinacion::~Combinacion(void)
  { loader= nullptr; }

//! @brief Asigna los coeficientes de ponderación a cada caso de la combinación.
void XC::Combinacion::Combinacion::set_gamma_f(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          lp->GammaF()= i->Factor();
        else
	  std::cerr << "Combinacion::set_gamma_f; Se encontró un puntero nulo en la descomposición." << std::endl;
      }
  }

//! @brief Asigna el dominio a cada caso de la combinación.
void XC::Combinacion::set_domain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          lp->setDomain(dom);
        else
	  std::cerr << "Combinacion::set_domain; Se encontró un puntero nulo en la descomposición." << std::endl;
      }
  }

//! @brief Añade al dominio being passed as parameter las hipótesis de la combinacion.
bool XC::Combinacion::addToDomain(void)
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
            if((!result) && (verborrea>3))
              {
                const MapLoadPatterns &casos= loader->getLoadPatterns();
	        std::cerr << "No se pudo agregar la acción: '"
                          << i->getNombreCaso(casos)
                          << "' al activar la combinación: '"
                          << getNombre() << "'\n";
              }
            retval= (retval && result);
          }
        else
	  std::cerr << "Combinacion::addToDomain; Se encontró un puntero nulo en la descomposición." << std::endl;
      }
    return retval;
  }

//! @brief Elimina del dominio being passed as parameter las hipótesis de la combinacion.
void XC::Combinacion::removeFromDomain(void)
  {
    Domain *dom= getDomain();
    assert(dom);
    for(iterator i= begin();i!=end();i++)
      {
        LoadPattern *lp= i->Caso();
        if(lp)
          dom->removeLoadPattern(lp);
        else
	  std::cerr << "Combinacion::removeFromDomain; Se encontró un puntero nulo en la descomposición." << std::endl;
      }
  }

//! @brief Agrega un sumando a la descomposición.
void XC::Combinacion::agrega_sumando(const sumando &sum)
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

//! @brief Obtiene la descomposición interpretando el string being passed as parameter.
void XC::Combinacion::interpreta_descomp(const std::string &str_descomp)
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
	  std::cerr << "El sumando: " << str_sum_i << " está mal expresado." << std::endl;
        else
          {
            const float factor= boost::lexical_cast<float>(q_blancos(str_prod[0]));
            const std::string nmb_hipot= q_blancos(str_prod[1]);
            if(loader)
              {
                LoadPattern *lp= loader->getLoadPatterns().buscaLoadPattern(nmb_hipot);
                if(lp)
                  agrega_sumando(sumando(factor,lp));
                else
	          std::cerr << "No se encontró la hipótesis: '" 
                            << nmb_hipot << "'\n";
              }
            else
	      std::cerr << "Combinacion::interpreta_descomp; se necesita un apuntador a LoadLoader." << std::endl;
          } 
      }
  }


//! @brief Devuelve un iterador apuntando al caso being passed as parameter.
XC::Combinacion::const_iterator XC::Combinacion::buscaCaso(const LoadPattern *lp) const
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

//! @brief Devuelve un iterador apuntando al caso being passed as parameter.
XC::Combinacion::iterator XC::Combinacion::buscaCaso(const LoadPattern *lp)
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

//! @brief Elimina los sumandos de la combinacion.
void XC::Combinacion::clear(void)
  { descomp.clear(); }

//! @brief Elimina los sumandos con factor nulo.
void XC::Combinacion::limpia_ceros(void)
  {
    TDescomp nueva;
    for(iterator i= begin();i!=end();i++)
      if(i->Factor()!=0.0)
        nueva.push_back(*i);
    descomp= nueva;
  }

//! @brief Devuelve el grupo al que pertenece la combinación.
const XC::GrupoCombinaciones *XC::Combinacion::getGrupo(void) const
  { return dynamic_cast<const GrupoCombinaciones *>(Owner()); }
    

//! @brief Devuelve el grupo al que pertenece la combinación.
XC::GrupoCombinaciones *XC::Combinacion::getGrupo(void)
  { return dynamic_cast<GrupoCombinaciones *>(Owner()); }

//! @brief Devuelve, si puede, un puntero a la combinación previa.
const XC::Combinacion *XC::Combinacion::getPtrCombPrevia(void) const
  {
    const Combinacion *retval= nullptr;
    const GrupoCombinaciones *g= getGrupo();
    if(g)
      retval= g->getPtrCombPrevia(*this);
    return retval;
  }

//! @brief Devuelve, si puede, el nombre de la combinación previa.
const std::string XC::Combinacion::getNombreCombPrevia(void) const
  {
    std::string retval;
    const Combinacion *c= getPtrCombPrevia();
    if(c)
      retval= c->getNombre();
    return retval;
  }

//! @brief Devuelve, si puede, el tag de la combinación previa.
int XC::Combinacion::getTagCombPrevia(void) const
  {
    int retval= -1;
    const Combinacion *c= getPtrCombPrevia();
    if(c)
      retval= c->getTag();
    return retval;
  }

//! @brief Devuelve, si puede, la descomposición de la combinación previa.
const std::string XC::Combinacion::getDescompCombPrevia(void) const
  {
    std::string retval;
    const Combinacion *c= getPtrCombPrevia();
    if(c)
      retval= c->getString();
    return retval;
  }

//! @brief Devuelve, si puede, la diferencia entre esta y la previa.
const std::string XC::Combinacion::getDescompRestoSobrePrevia(void) const
  {
    std::string retval;
    const Combinacion *c= getPtrCombPrevia();
    if(c)
      {
        Combinacion dif(*this);
        dif.resta(*c);
        retval= dif.getString();
      }
    return retval;
  }

//! @brief Asigna el dominio a los casos de carga de la combinación.
void XC::Combinacion::Combinacion::setDomain(Domain *theDomain)
  {
    ForceReprComponent::setDomain(theDomain);
    set_domain();
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Combinacion::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Combinacion::sendData(CommParameters &cp)
  {
    int res= ForceReprComponent::sendData(cp);
    res+= cp.sendString(nombre,getDbTagData(),CommMetaData(2));
    res+= cp.sendString(getString(),getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Combinacion::recvData(const CommParameters &cp)
  {
    int res= ForceReprComponent::recvData(cp);
    res+= cp.receiveString(nombre,getDbTagData(),CommMetaData(2));
    //Teporary storage for decomposition.
    std::string tmp;
    res+= cp.receiveString(tmp,getDbTagData(),CommMetaData(3));
    map_str_descomp[getDbTag()]= tmp;
    //La descomposición se asigna más tarde (en GrupoCombinaciones::recvData),
    //una vez asignado el propietario y el apuntador a LoadLoader.
    return res;
  }

//! @brief Recibe la descomposición que corresponde a la combinación
//! debe llamarse una vez que se ha asignado el propietario y el
//! apuntador a LoadLoader.
int XC::Combinacion::recvDescomp(void)
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
int XC::Combinacion::sendSelf(CommParameters &cp)
  {
    inicComm(4);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Combinacion::sendSelf() - failed to send data.\n";    
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Combinacion::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Combinacion::recvSelf() - data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Suma a ésta la combinación being passed as parameter.
XC::Combinacion &XC::Combinacion::suma(const Combinacion &otro)
  {
    for(const_iterator i= otro.begin();i!=otro.end();i++)
      agrega_sumando(*i);
//     for(iterator i= begin();i!=end();i++)
//       {
//         sumando &tmp= *i;
//         const_iterator j= otro.buscaCaso(tmp.Caso());
//         if(j!=otro.end())
//           tmp.suma(*j);
//       }
    limpia_ceros();
    return *this;
  }

//! @brief Suma a ésta la combinación cuyo nombre se pasa como parámetro.
XC::Combinacion &XC::Combinacion::suma(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const Combinacion *cmb= loader->getCombinaciones().buscaCombinacion(nmbComb);
        if(cmb)
          suma(*cmb);
        else
          std::cerr << "Combinacion::suma; no se encontró la combinación: '" 
                    << nmbComb << "'\n";
      }
    return *this;
  }


//! @brief Resta a ésta la combinación being passed as parameter.
XC::Combinacion &XC::Combinacion::resta(const Combinacion &otro)
  {
    for(const_iterator i= otro.begin();i!=otro.end();i++)
      agrega_sumando((*i).getNeg());
    limpia_ceros();
    return *this;
  }

//! @brief Suma a ésta la combinación cuyo nombre se pasa como parámetro.
XC::Combinacion &XC::Combinacion::resta(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const Combinacion *cmb= loader->getCombinaciones().buscaCombinacion(nmbComb);
        if(cmb)
          resta(*cmb);
        else
          std::cerr << "Combinacion::resta; no se encontró la combinación: '" 
                    << nmbComb << "'\n";
      }
    return *this;
  }

//! @brief Asigna a ésta la combinación cuyo nombre se pasa como parámetro.
XC::Combinacion &XC::Combinacion::asigna(const std::string &nmbComb)
  {
    if(!nmbComb.empty())
      {
        const Combinacion *cmb= loader->getCombinaciones().buscaCombinacion(nmbComb);
        if(cmb)
          (*this)= *cmb;
        else
          std::cerr << "Combinacion::igual; no se encontró la combinación: '" 
                    << nmbComb << "'\n";
      }
    return *this;
  }

//! @brief Multiplica la combinación por el número being passed as parameter.
XC::Combinacion &XC::Combinacion::multiplica(const float &f)
  {
    if(f!=0.0)
      for(iterator i= begin();i!=end();i++)
        (*i).multiplica(f);
    else
      clear();
    return *this;
  }


//! @brief Divide la combinación por el número being passed as parameter.
XC::Combinacion &XC::Combinacion::divide(const float &f)
  {
    for(iterator i= begin();i!=end();i++)
      (*i).divide(f);
    return *this;
  }

//! @brief Devuelve el resultado de sumar ésta combinación con
//! la being passed as parameter.
XC::Combinacion XC::Combinacion::operator+(const Combinacion &c) const
  {
    Combinacion retval(*this);
    retval+= c;
    return retval;
  }

//! @brief Devuelve el resultado de sumar ésta combinación con
//! la being passed as parameter.
XC::Combinacion XC::Combinacion::operator-(const Combinacion &c) const
  {
    Combinacion retval(*this);
    retval-= c;
    return retval;
  }
 
//! @brief Devuelve el resultado de multiplicar ésta combinación por
//! el factor being passed as parameter.
XC::Combinacion XC::Combinacion::operator*(const float &fact) const
  {
    Combinacion retval(*this);
    retval*= fact;
    return retval;
  }

//! @brief Devuelve el resultado de dividir ésta combinación por
//! el factor being passed as parameter.
XC::Combinacion XC::Combinacion::operator/(const float &fact) const
  {
    Combinacion retval(*this);
    retval/= fact;
    return retval;
  }

//! @brief Devuelve verdadero el coeficiente que pondera al caso que
//! se pasa como parámetro.
float XC::Combinacion::getCoefCaso(const LoadPattern *lp) const
  {
    float retval= 0.0;
    const_iterator i= buscaCaso(lp);
    if(i!=end())
      retval= (*i).Factor();
    return retval;
  }

bool XC::Combinacion::operator!=(const Combinacion &otra) const
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

bool XC::Combinacion::operator==(const Combinacion &otra) const
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

//! @brief Devuelve verdadero si los coeficientes que ponderan a todos
//! los casos de carga de esta combinación son mayores que los correspondientes
//! en la being passed as parameter.
bool XC::Combinacion::dominaA(const Combinacion &otra) const
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

//! @brief Devuelve una cadena de caracteres que representa la combinación
//! «1.35*G1+0.90*G1».
//! @arg \c fmt: Formato para el factor que multiplica a la hipótesis.
std::string XC::Combinacion::getString(const std::string &fmt) const
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
void XC::Combinacion::Print(std::ostream &s, int flag) const
  { s << getString(); }

std::ostream &XC::operator<<(std::ostream &os,const Combinacion &c)
  {
    c.Print(os);
    return os;
  }
