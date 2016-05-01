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
//MapLoadPatterns.cc

#include "MapLoadPatterns.h"
#include "preprocessor/loaders/LoadLoader.h"

//Time series.
#include "domain/load/pattern/TimeSeries.h"
#include "domain/load/pattern/time_series/ConstantSeries.h"
#include "domain/load/pattern/time_series/LinearSeries.h"
#include "domain/load/pattern/time_series/DiscretizedRandomProcessSeries.h"
#include "domain/load/pattern/time_series/PathSeries.h"
#include "domain/load/pattern/time_series/PathTimeSeries.h"
#include "domain/load/pattern/time_series/PulseSeries.h"
#include "domain/load/pattern/time_series/RectangularSeries.h"
#include "domain/load/pattern/time_series/SimulatedRandomProcessSeries.h"
#include "domain/load/pattern/time_series/TriangleSeries.h"
#include "domain/load/pattern/time_series/TrigSeries.h"

//Load patterns.
#include "domain/load/pattern/load_patterns/EarthquakePattern.h"
#include "domain/load/pattern/load_patterns/MultiSupportPattern.h"
#include "domain/load/pattern/load_patterns/PBowlLoading.h"
#include "domain/load/pattern/load_patterns/UniformExcitation.h"

//Loads.
#include "domain/load/NodalLoad.h"
#include "domain/load/elem_load.h"
//Constraints.
#include "domain/constraints/SP_Constraint.h"

#include "domain/domain/Domain.h"



#include "utility/actor/actor/MovableMap.h"



//! @brief Constructor por defecto.
XC::MapLoadPatterns::MapLoadPatterns(LoadLoader *owr)
  : LoadLoaderMember(owr), nmb_ts("nil"), lpcode("nil"), tag_el(0), tag_nl(0), tag_spc(0) {}

//! @brief Devuelve un puntero al TS cuyo dbTag se pasa como parámetro.
//! se usa en LoadPattern::recvData.
XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const int &dbTag)
  {
    TimeSeries *retval= nullptr;
    for(map_timeseries::iterator i= tseries.begin();i!=tseries.end();i++)
      if(dbTag== (i->second->getDbTag()))
        {
          retval= i->second;
          break;
        }
    return retval;
  }

//! @brief Devuelve un puntero al TS cuyo nombre se pasa como parámetro.
const XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const int &dbTag) const
  {
    const TimeSeries *retval= nullptr;
    for(map_timeseries::const_iterator i= tseries.begin();i!=tseries.end();i++)
      if(dbTag== (i->second->getDbTag()))
        {
          retval= i->second;
          break;
        }
    return retval;
  }

//! @brief Devuelve un puntero al TimeSeries cuyo nombre se pasa como parámetro.
XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const std::string &ts_code)
  {
    TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Devuelve un puntero al TimeSeries cuyo nombre se pasa como parámetro.
const XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const std::string &ts_code) const
  {
    const TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Devuelve un puntero al caso de carga cuyo nombre se pasa como parámetro.
XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const std::string &lp_code)
  {
    LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=end())
      retval= ilp->second;
    return retval;
  }

//! @brief Devuelve un puntero al caso de carga cuyo nombre se pasa como parámetro.
const XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const std::string &lp_code) const
  {
    const LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=loadpatterns.end())
      retval= ilp->second;
    return retval;
  }

//! @brief Devuelve un puntero al caso cuyo tag se pasa como parámetro.
XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const int &tag)
  {
    LoadPattern *retval= nullptr;
    for(iterator ilp= begin();ilp!=end();ilp++)
      if(tag== (ilp->second->getTag()))
        {
          retval= ilp->second;
          break;
        }
    return retval;
  }

//! @brief Devuelve un puntero al caso cuyo tag se pasa como parámetro.
const XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const int &tag) const
  {
    const LoadPattern *retval= nullptr;
    for(const_iterator ilp= begin();ilp!=end();ilp++)
      if(tag== (ilp->second->getTag()))
        {
          retval= ilp->second;
          break;
        }
    return retval;
  }

//! @brief Devuelve el nombre del caso al que apunta el parámetro.
const std::string &XC::MapLoadPatterns::getNombreLoadPattern(const LoadPattern *ptr) const
  {
    static std::string retval;
    retval= "";
    //Borramos las loadpatterns.
    for(const_iterator i= begin();i!=end();i++)
      if((*i).second == ptr)
        {
          retval= (*i).first;
          break;
        }
    return retval;
  }

//! @brief Agrega el caso de carga al dominio.
void XC::MapLoadPatterns::addToDomain(const std::string &lp_code)
  {
    LoadPattern *lp= buscaLoadPattern(lp_code);
    if(lp)
      {
        bool result= getDomain()->addLoadPattern(lp);
        if((!result) && (verborrea>3))
          std::cerr << "MapLoadPatterns::addToDomain; no se pudo agregar la acción: '"
                    << lp_code << "'\n";
      }
    else
      std::cerr << "MapLoadPatterns::addToDomain: no se encontró el caso de carga: " 
                << lp_code << std::endl;
  }

//! @brief Elimina el caso de carga del dominio.
void XC::MapLoadPatterns::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= buscaLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      std::cerr << "MapLoadPatterns::removeFromDomain: no se encontró el caso de carga: " 
                << lp_code << std::endl;
  }

//! @brief Elimina las loadpatterns del dominio.
void XC::MapLoadPatterns::removeAllFromDomain(void)
  {
    Domain *dom= getDomain();
    for(iterator i= begin();i!=end();i++)
      dom->removeLoadPattern((*i).second);
  }

//! @brief Define un objeto TimeSeries con el tipo y el nombre que se pasan como parámetro.
//! Interpreta los siguientes tipos:
//!
//! - constant_ts: Define una modulación temporal de tipo ConstantSeries.
//! - linear_ts: Define una modulación temporal de tipo LinearSeries.
//! - path_ts: Define una modulación temporal de tipo PathSeries.
//! - pulse_ts: Define una modulación temporal de tipo PulseSeries.
//! - rectangular_ts: Define una modulación temporal de tipo RectangularSeries.
//! - triangular_ts: Define una modulación temporal de tipo TriangleSeries.
//! - trig_ts: Define una modulación temporal de tipo TrigSeries.
XC::TimeSeries *XC::MapLoadPatterns::newTimeSeries(const std::string &tipo, const std::string &cod_ts)
  {
    TimeSeries *ts= nullptr;
    if(tipo == "constant_ts")
      ts= crea_time_series<ConstantSeries>(cod_ts);
    else if(tipo == "linear_ts")
      ts= crea_time_series<LinearSeries>(cod_ts);
    else if(tipo == "path_ts")
      ts= crea_time_series<PathSeries>(cod_ts);
    else if(tipo == "path_time_ts")
      ts= crea_time_series<PathTimeSeries>(cod_ts);
    else if(tipo == "pulse_ts")
      ts= crea_time_series<PulseSeries>(cod_ts);
    else if(tipo == "rectangular_ts")
      ts= crea_time_series<RectangularSeries>(cod_ts);
    else if(tipo == "triangular_ts")
      ts= crea_time_series<TriangleSeries>(cod_ts);
    else if(tipo == "trig_ts")
      ts= crea_time_series<TrigSeries>(cod_ts);
    else
      std::cerr << "Time series type: '" << tipo
                << "' unknown." << std::endl;
    if(!ts)
      std::cerr << "Error in time series definition of type: '" 
                << tipo << "'." << std::endl;
    return ts;
  }

//! @brief Define un objeto LoasPattern con el tipo y el nombre que se pasan como parámetro.
XC::LoadPattern *XC::MapLoadPatterns::newLoadPattern(const std::string &tipo,const std::string &cod_lp)
  {
    LoadPattern *retval= nullptr;
    if(tipo == "load_pattern" || tipo == "default" )
      retval=  crea_load_pattern<LoadPattern>(cod_lp);
    else if(tipo == "uniform_excitation")
      retval= crea_load_pattern<UniformExcitation>(cod_lp);
    else if(tipo == "multi_support_pattern")
      retval= crea_load_pattern<MultiSupportPattern>(cod_lp);
    else if(tipo == "pbowl_loading")
      retval= crea_load_pattern<PBowlLoading>(cod_lp);
    return retval;
  }

//! @brief Borra todos los casos de carga.
void XC::MapLoadPatterns::clear_time_series(void)
  {
    //Borramos la lista de timeseries.
    for(map_timeseries::iterator i= tseries.begin();i!=tseries.end();i++)
      delete (*i).second;
    tseries.erase(tseries.begin(),tseries.end());
  }

//! @brief Borra todas las loadpatterns.
void XC::MapLoadPatterns::clear(void)
  {
    removeAllFromDomain();
    //Borramos las loadpatterns.
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    loadpatterns.clear();
    clear_time_series();
  }

//! @brief Destructor.
XC::MapLoadPatterns::~MapLoadPatterns(void)
  { clear(); }

const std::string XC::MapLoadPatterns::getCurrentLoadPatternId(void) const
  { return lpcode; }

XC::LoadPattern *XC::MapLoadPatterns::getCurrentLoadPatternPtr(void)
  { return buscaLoadPattern(lpcode); }

const XC::LoadPattern *XC::MapLoadPatterns::getCurrentLoadPatternPtr(void) const
  { return buscaLoadPattern(lpcode); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::MapLoadPatterns::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::MapLoadPatterns::sendData(CommParameters &cp)
  {
    int res= sendMap(tseries,cp,getDbTagData(),CommMetaData(0));
    res+= cp.sendString(nmb_ts,getDbTagData(),CommMetaData(1));
    res+= sendMap(loadpatterns,cp,getDbTagData(),CommMetaData(2));
    res+= cp.sendString(lpcode,getDbTagData(),CommMetaData(3));
    res+= cp.sendInts(tag_el,tag_nl,tag_spc,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::MapLoadPatterns::recvData(const CommParameters &cp)
  {
    clear();
    int res= receiveMap(tseries,cp,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewTimeSeries);
    res+= cp.receiveString(nmb_ts,getDbTagData(),CommMetaData(1));
    res+= receiveMap(loadpatterns,cp,getDbTagData(),CommMetaData(2),&FEM_ObjectBroker::getNewLoadPattern);
    for(iterator i= begin();i!= end();i++)
      (*i).second->set_owner(this);
    res+= cp.receiveString(lpcode,getDbTagData(),CommMetaData(3));
    res+= cp.receiveInts(tag_el,tag_nl,tag_spc,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::MapLoadPatterns::sendSelf(CommParameters &cp)
  {
    inicComm(5);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MapLoadPatterns::sendSelf() - failed to send data.\n";    
    return res;
  }

//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::MapLoadPatterns::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
    int res= 0;
    //Si no está vacío entendemos que valen las definiciones existentes.
    if(empty())
      {
        const int dataTag= getDbTag();
        res= cp.receiveIdData(getDbTagData(),dataTag);
        if(res<0)
          std::cerr << "MapLoadPatterns::recvSelf() - data could not be received.\n" ;
        else
          res+= recvData(cp);
      }
    return res;
  }

//! @brief Devuelve los nombres de los casos de carga.
std::deque<std::string> XC::MapLoadPatterns::getListaNombres(void) const
  {
    std::deque<std::string> retval;
    for(const_iterator i= begin();i!= end();i++)
      retval.push_back((*i).first);
    return retval;
  }

//! @brief Returns load case names.
boost::python::list XC::MapLoadPatterns::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;
  }
