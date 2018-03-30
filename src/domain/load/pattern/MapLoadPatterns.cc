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
//MapLoadPatterns.cc

#include "MapLoadPatterns.h"
#include "preprocessor/prep_handlers/LoadHandler.h"

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
#include "domain/constraints/SFreedom_Constraint.h"

#include "domain/domain/Domain.h"



#include "utility/actor/actor/MovableMap.h"



//! @brief Default constructor.
XC::MapLoadPatterns::MapLoadPatterns(LoadHandler *owr)
  : LoadHandlerMember(owr), nmb_ts("nil"), lpcode("nil"), tag_el(0), tag_nl(0), tag_spc(0) {}

//! @brief Returns a pointer to the TS cuyo dbTag being passed as parameter.
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

//! @brief Returns a pointer to the TS cuyo nombre being passed as parameter.
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

//! @brief Returns a pointer to the TimeSeries cuyo nombre being passed as parameter.
XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const std::string &ts_code)
  {
    TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Returns a pointer to the TimeSeries cuyo nombre being passed as parameter.
const XC::TimeSeries *XC::MapLoadPatterns::buscaTS(const std::string &ts_code) const
  {
    const TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern cuyo nombre being passed as parameter.
XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const std::string &lp_code)
  {
    LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=end())
      retval= ilp->second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern cuyo nombre being passed as parameter.
const XC::LoadPattern *XC::MapLoadPatterns::buscaLoadPattern(const std::string &lp_code) const
  {
    const LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=loadpatterns.end())
      retval= ilp->second;
    return retval;
  }

//! @brief Returns a pointer to the caso cuyo tag being passed as parameter.
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

//! @brief Returns a pointer to the load pattern with the tag
//! being passed as parameter.
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

//! @brief Returns the name of the load pattern pointed by the parameter.
const std::string &XC::MapLoadPatterns::getLoadPatternName(const LoadPattern *ptr) const
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

//! @brief Adds the load pattern to the domain.
void XC::MapLoadPatterns::addToDomain(const std::string &lp_code)
  {
    LoadPattern *lp= buscaLoadPattern(lp_code);
    if(lp)
      {
        bool result= getDomain()->addLoadPattern(lp);
        if((!result) && (verbosity>3))
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; can't add the load pattern: '"
                    << lp_code << "'\n";
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" 
                << lp_code << "' not found." << std::endl;
  }

//! @brief Remove the load pattern del domain.
void XC::MapLoadPatterns::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= buscaLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" 
                << lp_code << "' not found." << std::endl;
  }

//! @brief Remove the loadpatterns del domain.
void XC::MapLoadPatterns::removeAllFromDomain(void)
  {
    Domain *dom= getDomain();
    for(iterator i= begin();i!=end();i++)
      dom->removeLoadPattern((*i).second);
  }

//! @brief Define a TimeSeries object with the type and name passed as parameters.
//! It Interprets the following types:
//!
//! - constant_ts: Defines a constant time series (ConstantSeries).
//! - linear_ts: Defines a linear time series (LinearSeries).
//! - path_ts: Defines a path time series (PathSeries).
//! - pulse_ts: Defines a pulse time series (PulseSeries).
//! - rectangular_ts: Defines a rectangular time series (RectangularSeries).
//! - triangular_ts: Defines a triangular time series (TriangleSeries).
//! - trig_ts: Defines a trigonometric time series (TrigSeries).
XC::TimeSeries *XC::MapLoadPatterns::newTimeSeries(const std::string &tipo, const std::string &cod_ts)
  {
    TimeSeries *ts= nullptr;
    if(tipo == "constant_ts")
      ts= create_time_series<ConstantSeries>(cod_ts);
    else if(tipo == "linear_ts")
      ts= create_time_series<LinearSeries>(cod_ts);
    else if(tipo == "path_ts")
      ts= create_time_series<PathSeries>(cod_ts);
    else if(tipo == "path_time_ts")
      ts= create_time_series<PathTimeSeries>(cod_ts);
    else if(tipo == "pulse_ts")
      ts= create_time_series<PulseSeries>(cod_ts);
    else if(tipo == "rectangular_ts")
      ts= create_time_series<RectangularSeries>(cod_ts);
    else if(tipo == "triangular_ts")
      ts= create_time_series<TriangleSeries>(cod_ts);
    else if(tipo == "trig_ts")
      ts= create_time_series<TrigSeries>(cod_ts);
    else
      std::cerr << "Time series type: '" << tipo
                << "' unknown." << std::endl;
    if(!ts)
      std::cerr << "Error in time series definition of type: '" 
                << tipo << "'." << std::endl;
    return ts;
  }

//! @brief Define a LoadPattern object withe the type and the
//! name being passed as parameters.
XC::LoadPattern *XC::MapLoadPatterns::newLoadPattern(const std::string &tipo,const std::string &cod_lp)
  {
    LoadPattern *retval= nullptr;
    if(tipo == "load_pattern" || tipo == "default" )
      retval=  create_load_pattern<LoadPattern>(cod_lp);
    else if(tipo == "uniform_excitation")
      retval= create_load_pattern<UniformExcitation>(cod_lp);
    else if(tipo == "multi_support_pattern")
      retval= create_load_pattern<MultiSupportPattern>(cod_lp);
    else if(tipo == "pbowl_loading")
      retval= create_load_pattern<PBowlLoading>(cod_lp);
    return retval;
  }

//! @brief Clears all the load patterns.
void XC::MapLoadPatterns::clear_time_series(void)
  {
    //Clears time series.
    for(map_timeseries::iterator i= tseries.begin();i!=tseries.end();i++)
      delete (*i).second;
    tseries.erase(tseries.begin(),tseries.end());
  }

//! @brief Clears all the load patterns.
void XC::MapLoadPatterns::clear(void)
  {
    removeAllFromDomain();
    //Clear loads patterns.
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

//! @brief Return a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MapLoadPatterns::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send members through the channel passed as parameter.
int XC::MapLoadPatterns::sendData(CommParameters &cp)
  {
    int res= sendMap(tseries,cp,getDbTagData(),CommMetaData(0));
    res+= cp.sendString(nmb_ts,getDbTagData(),CommMetaData(1));
    res+= sendMap(loadpatterns,cp,getDbTagData(),CommMetaData(2));
    res+= cp.sendString(lpcode,getDbTagData(),CommMetaData(3));
    res+= cp.sendInts(tag_el,tag_nl,tag_spc,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Send members through the channel passed as parameter.
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

//! @brief Sends object through the channel passed as parameter.
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

//! @brief Receives object through the channel passed as parameter.
int XC::MapLoadPatterns::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
    int res= 0;
    //If not empty we suppose that current definitions are OK.
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

//! @brief Return the names of the load patterns.
std::deque<std::string> XC::MapLoadPatterns::getListaNombres(void) const
  {
    std::deque<std::string> retval;
    for(const_iterator i= begin();i!= end();i++)
      retval.push_back((*i).first);
    return retval;
  }

//! @brief Return load case names.
boost::python::list XC::MapLoadPatterns::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;
  }
