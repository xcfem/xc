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
//MapSet.cc

#include "MapSet.h"
#include "Set.h"
#include "SetEstruct.h"
#include "preprocessor/cad/entidades/EntMdlr.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/actor/MovableID.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

XC::ID XC::MapSet::setsDbTags;
std::deque<std::string> XC::MapSet::setsClassNames;

//! @brief Devuelve verdadero si el conjunto existe.
bool XC::MapSet::existe(const std::string &nmb) const
  { return (sets.find(nmb)!=sets.end()); }

//! @brief Devuelve un puntero al conjunto cuyo nombre se pasa como parámetro.
XC::SetBase *XC::MapSet::busca_set(const std::string &nmb)
  {
    if(existe(nmb))
      return sets[nmb];
    else if(entidades.find(nmb)!=entidades.end())
      return entidades[nmb];
    else
      return nullptr;
  }

//! @brief Devuelve un puntero al conjunto cuyo nombre se pasa como parámetro.
const XC::SetBase *XC::MapSet::busca_set(const std::string &nmb) const
  {
    const_iterator i= sets.find(nmb);
    if(i!=sets.end())
      return (*i).second;
    else
      {
    	map_ent_mdlr::const_iterator j= entidades.find(nmb);
        if(j != entidades.end())
          return (*j).second;
        else
          return nullptr;
      }
  }

//! @brief Devuelve el conjunto cuyo nombre se pasa como parámetro.
XC::SetBase &XC::MapSet::getSet(const std::string &nmb)
  {
    SetBase *retval= busca_set(nmb);
    if(!retval)
      {
	std::cerr << "MapSet::getSet; set named '"
                  << nmb << "' not found. Total set returned." << std::endl;
        retval= total;
      }
    return *retval;
  }


XC::MapSet::const_iterator XC::MapSet::begin(void) const
  { return sets.begin(); }

XC::MapSet::const_iterator XC::MapSet::end(void) const
  { return sets.end(); }

XC::MapSet::iterator XC::MapSet::begin(void)
  { return sets.begin(); }

XC::MapSet::iterator XC::MapSet::end(void)
  { return sets.end(); }

//! @brief Crea un nuevo conjunto con el nombre que se le pasa como parámetro.
XC::Set *XC::MapSet::crea_set(const std::string &nmb)
  {
    Set *tmp =nullptr;
    if(!existe(nmb)) //El conjunto es nuevo.
      {
        tmp= new Set(nmb,preprocessor);
        sets[nmb]= tmp;
      }
    else //El conjunto existe
      tmp= dynamic_cast<Set *>(busca_set(nmb));
    return tmp;
  }

void XC::MapSet::abre_set(const std::string &nmb)
  {
    SetBase *tmp =nullptr;
    if(!existe(nmb)) //El conjunto no existe.
      std::cerr << "MapSet::abre_set; el conjunto: '"
                << nmb << "' no existe.\n";
    else //El conjunto existe
      {
        tmp= busca_set(nmb);
        assert(tmp);
        abiertos[nmb]= tmp;
      }
  }
void XC::MapSet::cierra_set(const std::string &nmb)
  {
    if(!existe(nmb)) //El conjunto no existe.
      std::cerr << "MapSet::cierra_set; el conjunto: '"
                << nmb << "' no existe.\n";
    else //El conjunto existe
      {
	iterator i= abiertos.find(nmb);
        if(i!= abiertos.end())
          abiertos.erase(i);
        else
          if(verborrea>1)
	    std::cerr << "MapSet::cierra_set; el conjunto: '"
                      << nmb << "' no está abierto.\n";
      }
  }


//! @bref Crea un conjunto estructurado copia del que se le pasa como parámetro.
XC::SetEstruct *XC::MapSet::crea_set_estruct(const SetEstruct &set_estruct)
  {
    const std::string nmb= set_estruct.GetNombre();
    if(existe(nmb)) //El conjunto existe
      {
	std::cerr << "El conjunto: " << nmb << " ya existe. No se inserta el nuevo." << std::endl;
        return nullptr;
      }
    else //El conjunto es nuevo.
      {
	SetEstruct *retval= set_estruct.getCopy();
        if(retval) sets[nmb]= retval;
        return retval;
      }
  }

//! @bref Crea un conjunto estructurado copia del que se le pasa como parámetro.
XC::SetBase *XC::MapSet::broke_set(const std::string &nmb,const std::string &nmb_clase)
  {
    SetBase *retval= nullptr;
    if(existe(nmb)) //El conjunto existe
      retval= busca_set(nmb);
    else //El conjunto es nuevo.
      {
        if(nmb_clase == "XC::Set")
          retval= new Set(nmb,preprocessor);
//         else if(nmb_clase == "XC::SetEstruct")
//           retval= new SetEstruct(nmb,preprocessor);
        else
	  std::cerr << "MapSet::broke_set; el nombre de clase: '"
                    << nmb_clase << "' es desconocido." << std::endl;
      }
    return retval;
  }

//! @bref Inserta, como conjunto, una entidad del preprocesador.
XC::EntMdlr *XC::MapSet::inserta_ent_mdlr(EntMdlr *ent_mdlr)
  {
    assert(ent_mdlr);
    const std::string nmb= ent_mdlr->GetNombre();
    if(existe(nmb)) //El conjunto existe
      {
	std::cerr << "El conjunto: " << nmb << " ya existe. No se inserta el nuevo." << std::endl;
        return nullptr;
      }
    else //El conjunto es nuevo.
      {
        entidades[nmb]= ent_mdlr;
        return ent_mdlr;
      }
  }

//! @brief Devuelve un puntero al conjunto estructurado cuyo nombre se pasa como parámetro.
XC::SetEstruct *XC::MapSet::busca_set_estruct(const std::string &nmb)
  {
    SetBase *set= busca_set(nmb);
    SetEstruct *retval= nullptr;
    if(set)
      retval= dynamic_cast<SetEstruct *>(set);
    return retval;
  }

//! @brief Inicializa el conjunto total.
void XC::MapSet::setup_total(void)
  {
    const std::string str_tot= "total";
    total= crea_set(str_tot);
  }

//! @brief Constructor por defecto.
XC::MapSet::MapSet(Preprocessor *m)
  : EntCmd(m), MovableObject(0), preprocessor(m),total(nullptr)
  { setup_total(); }

//! @brief Constructor de copia (NO COPIA LOS CONJUNTOS).
XC::MapSet::MapSet(const MapSet &otro)
  : EntCmd(otro), MovableObject(otro), preprocessor(otro.preprocessor), total(nullptr)
  { setup_total(); }

//! @brief Operador asignación (NO COPIA LOS CONJUNTOS).
XC::MapSet &XC::MapSet::operator=(const MapSet &otro)
  {
    EntCmd::operator=(otro);
    MovableObject::operator=(otro);
    preprocessor= otro.preprocessor;
    setup_total();
    return *this;
  }

//! @brief Creates a new set with the name which is passed as a parameter.
XC::Set *XC::MapSet::defSet(const std::string &nmb)
  { return crea_set(nmb); }

//! @brief Borra todos los conjuntos definidos.
void XC::MapSet::clearSets(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    total= nullptr;
    sets.clear();
  }

//! @brief Borra todo.
void XC::MapSet::clearAll(void)
  {
    total= nullptr;
    entidades.clear();
    clearSets();
  }

//! @brief Borra todo.
void XC::MapSet::reset(void)
  {
    clearAll();
    setup_total();
  }

//! @brief Destructor.
XC::MapSet::~MapSet(void)
  { clearAll(); }

//! @brief Devuelve los DBTags de los conjuntos.
const XC::ID &XC::MapSet::getSetsDBTags(CommParameters &cp)
  {
    static ID retval;
    const int size= sets.size();
    if(size>0)
      {
        retval.resize(size);
        int loc =0;
        // loop over sets in map adding their dbTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            retval[loc]= (*i).second->getDbTag(cp);
            loc++;
          }        
      }
    return retval;
  }

//! @brief Devuelve los nombres de clase de los conjuntos.
const std::deque<std::string> &XC::MapSet::getSetsClassNames(void)
  {
    const int size= sets.size();
    if(size>0)
      {
        setsClassNames.resize(size);
        int loc =0;
        // loop over sets in map adding their classTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            setsClassNames[loc]= (*i).second->nombre_clase();
            loc++;
          }
      }
    return setsClassNames;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::MapSet::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Envía los dbTags de los conjuntos través del canal que se pasa como parámetro.
int XC::MapSet::sendSetsDbTags(int posDbTag,CommParameters &cp)
  {
    const int size= sets.size();
    int res= 0;
    if(size>0)
      {
        setsDbTags= getSetsDBTags(cp);
        res+= cp.sendID(setsDbTags,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "MapSet::sendDbTags - ch failed to send the IDs.\n";
    return res;
  }

//! @brief Envía los nombres de clase de los conjuntos través del canal que se pasa como parámetro.
int XC::MapSet::sendSetsClassNames(int posDbTag,CommParameters &cp)
  {
    const int size= sets.size();
    int res= 0;
    if(size>0)
      {
        setsClassNames= getSetsClassNames();
        res+= cp.sendStrings(setsClassNames,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "MapSet::sendDbTags - ch failed to send the IDs.\n";
    return res;
  }

//! @brief Recibe los dbTags de los conjuntos a través del canal que se pasa como parámetro.
int XC::MapSet::receiveSetsDbTags(int posDbTag,int size,const CommParameters &cp)
  {
    setsDbTags.resize(size);
    int res= 0;
    if(size>0)
      res= cp.receiveID(setsDbTags,getDbTagData(),CommMetaData(posDbTag));
    if(res<0)
      std::cerr << "MapSet::receiveDbTags - ch failed to receive the IDs.\n";
    return res;
  }

//! @brief Recibe los nombres de clase de los conjuntos a través del canal que se pasa como parámetro.
int XC::MapSet::receiveSetsClassNames(int posDbTag,int size,const CommParameters &cp)
  {
    setsClassNames.resize(size);
    int res= 0;
    if(size>0)
      res= cp.receiveStrings(setsClassNames,getDbTagData(),CommMetaData(posDbTag));
    if(res<0)
      std::cerr << "MapSet::receiveSetsClassNames - ch failed to receive the IDs.\n";
    return res;
  }

//! @brief Envía los conjuntos definidos por el canal que se pasa como parámetro.
int XC::MapSet::sendSets(int posDbTag1, int posDbTag2, int posDbTag3,CommParameters &cp)
  {
    std::deque<std::string> nombres;
    for(const_iterator i= begin();i!=end();i++)
      nombres.push_back((*i).first);
    DbTagData &dt= getDbTagData();
    int res= cp.sendStrings(nombres,dt,CommMetaData(posDbTag1));
    res+= sendSetsDbTags(posDbTag2,cp);
    res+= sendSetsClassNames(posDbTag3,cp);
    int loc= 0;
    DbTagData tags(setsDbTags);
    for(iterator i= begin();i!=end();i++,loc++)
      res+= cp.sendMovable(*(*i).second,tags,CommMetaData(loc));
    return res;
  }

//! @brief Recibe los conjuntos definidos por el canal que se pasa como parámetro.
int XC::MapSet::receiveSets(int posDbTag1, int posDbTag2, int posDbTag3,const int &sz,const CommParameters &cp)
  {
    std::deque<std::string> nombres;
    int res= cp.receiveStrings(nombres,getDbTagData(),CommMetaData(posDbTag1));
    res+= receiveSetsDbTags(posDbTag2,sz,cp);
    res+= receiveSetsClassNames(posDbTag3,sz,cp);
    int loc= 0;
    DbTagData tags(setsDbTags);
    for(std::deque<std::string>::iterator i= nombres.begin();i!=nombres.end();i++,loc++)
      {
        SetBase *tmp= broke_set(*i,setsClassNames[loc]);
        if(tmp)
          cp.receiveMovable(*tmp,tags,CommMetaData(loc));
        else
	  std::cerr << "Error al recibir el conjunto: '" << *i << "'.\n";
      }
    return res;
  }
//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::MapSet::sendAbiertos(int posDbTag1, int posDbTag2,CommParameters &cp)
  {
    const size_t sz= abiertos.size();
    int res= cp.sendInt(sz,getDbTagData(),CommMetaData(posDbTag1));
    if(!abiertos.empty())
      {
        std::deque<std::string> nmb_abiertos;
        for(const_iterator i= abiertos.begin();i!=abiertos.end();i++)
          nmb_abiertos.push_back((*i).first);
        res+= cp.sendStrings(nmb_abiertos,getDbTagData(),CommMetaData(posDbTag2));
      }
    return res;
  }
//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::MapSet::receiveAbiertos(int posDbTag1, int posDbTag2,const CommParameters &cp)
  {
     int sz_abiertos= 0;
     int res= cp.receiveInt(sz_abiertos,getDbTagData(),CommMetaData(posDbTag1));
     if(sz_abiertos>0)
       {
         std::deque<std::string> nmb_abiertos;
         res+= cp.receiveStrings(nmb_abiertos,getDbTagData(),CommMetaData(posDbTag2));
         for(std::deque<std::string>::const_iterator i= nmb_abiertos.begin();i!=nmb_abiertos.end();i++)
           abre_set(*i);
       }
    return res;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::MapSet::sendData(CommParameters &cp)
  {
    const size_t sz= sets.size();
    int res= cp.sendInt(sz,getDbTagData(),CommMetaData(0)); //Número de conjuntos.
    if(sz>0)
      {
        res+= sendSets(1,2,3,cp);
        res+= sendAbiertos(4,5,cp);
      }
    //XX Falta enviar las entidades (puntos, lineas, superficies,...).
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::MapSet::recvData(const CommParameters &cp)
  {
    int sz= 0;
    int res= cp.receiveInt(sz,getDbTagData(),CommMetaData(0)); //Número de conjuntos.
    if(sz>0)
      {
        res+= receiveSets(1,2,3,sz,cp);
        res+= receiveAbiertos(4,5,cp);
      }
    //XX Falta enviar las entidades (puntos, lineas, superficies,...).
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::MapSet::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr <<  nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::MapSet::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr <<  nombre_clase() << "recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr <<  nombre_clase() << "recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a nodo
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Node *n)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(n)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a elemento
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Element *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a punto
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Pnt *p)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(p)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a «edge»
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Edge *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a superficie
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Face *f)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(f)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a cuerpo
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Body *b)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(b)) retval.insert((*i).second);
    return retval;
  }

//! @brief Devuelve los conjuntos que contienen el puntero a «uniform grid»
//! que se pasa como parámetro.
std::set<XC::SetBase *> XC::MapSet::get_sets(const UniformGrid *ug)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(ug)) retval.insert((*i).second);
    return retval;
  }
