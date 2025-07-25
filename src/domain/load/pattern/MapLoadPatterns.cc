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

#include "utility/utils/misc_utils/colormod.h"



//! @brief Default constructor.
XC::MapLoadPatterns::MapLoadPatterns(LoadHandler *owr)
  : LoadHandlerMember(owr), time_series_name("nil"), lpcode("nil"), tag_el(0), tag_nl(0), tag_spc(0) {}

//! @brief Returns a pointer to the TS cuyo dbTag being passed as parameter.
//! se usa en LoadPattern::recvData.
XC::TimeSeries *XC::MapLoadPatterns::findTS(const int &dbTag)
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

//! @brief Returns a pointer to the TS which name is being passed as parameter.
const XC::TimeSeries *XC::MapLoadPatterns::findTS(const int &dbTag) const
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

//! @brief Returns a pointer to the TimeSeries which name being passed as parameter.
XC::TimeSeries *XC::MapLoadPatterns::findTS(const std::string &ts_code)
  {
    TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Returns a pointer to the TimeSeries which name being passed as parameter.
const XC::TimeSeries *XC::MapLoadPatterns::findTS(const std::string &ts_code) const
  {
    const TimeSeries *retval= nullptr;
    map_timeseries::const_iterator its= tseries.find(ts_code);
    if(its!=tseries.end())
      retval= its->second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern which name being passed as parameter.
XC::LoadPattern *XC::MapLoadPatterns::findLoadPattern(const std::string &lp_code)
  {
    LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=end())
      retval= ilp->second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern which name being passed as parameter.
const XC::LoadPattern *XC::MapLoadPatterns::findLoadPattern(const std::string &lp_code) const
  {
    const LoadPattern *retval= nullptr;
    const_iterator ilp= loadpatterns.find(lp_code);
    if(ilp!=loadpatterns.end())
      retval= ilp->second;
    return retval;
  }

//! @brief Returns a pointer to the load pattern identified by the
//! tag being passed as parameter.
XC::LoadPattern *XC::MapLoadPatterns::findLoadPattern(const int &tag)
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
const XC::LoadPattern *XC::MapLoadPatterns::findLoadPattern(const int &tag) const
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
    LoadPattern *lp= findLoadPattern(lp_code);
    if(lp)
      {
        bool result= getDomain()->addLoadPattern(lp);
        if((!result) && (verbosity>3))
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; can't add the load pattern: '"
                    << lp_code
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" 
                << lp_code << "' not found."
		<< Color::red << std::endl;
  }

//! @brief Remove the load pattern del domain.
void XC::MapLoadPatterns::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= findLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" 
                << lp_code << "' not found."
		<< Color::def << std::endl;
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
XC::TimeSeries *XC::MapLoadPatterns::newTimeSeries(const std::string &type, const std::string &cod_ts)
  {
    TimeSeries *ts= this->findTS(cod_ts);
    if(ts)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; WARNING time series: '" << cod_ts
                << "' already_exists. This command redefines it."
		<< Color::red << std::endl;
    if(type == "constant_ts")
      ts= create_time_series<ConstantSeries>(cod_ts);
    else if(type == "linear_ts")
      ts= create_time_series<LinearSeries>(cod_ts);
    else if(type == "path_ts")
      ts= create_time_series<PathSeries>(cod_ts);
    else if(type == "path_time_ts")
      ts= create_time_series<PathTimeSeries>(cod_ts);
    else if(type == "pulse_ts")
      ts= create_time_series<PulseSeries>(cod_ts);
    else if(type == "rectangular_ts")
      ts= create_time_series<RectangularSeries>(cod_ts);
    else if(type == "triangular_ts")
      ts= create_time_series<TriangleSeries>(cod_ts);
    else if(type == "trig_ts")
      ts= create_time_series<TrigSeries>(cod_ts);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; time series type: '" << type
                << "' unknown."
		<< Color::def << std::endl;
    if(!ts)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; error in time series definition of type: '" 
                << type << "'."
		<< Color::def << std::endl;
    return ts;
  }

//! @brief Returns the name of the time series pointed by the parameter.
const std::string &XC::MapLoadPatterns::getTimeSeriesName(const TimeSeries *ptr) const
  {
    static std::string retval;
    retval= "";
    for(time_series_const_iterator i= tseries.begin();i!=tseries.end();i++)
      if((*i).second == ptr)
        {
          retval= (*i).first;
          break;
        }
    return retval;
  }

//! @brief Define a LoadPattern object with the type and the
//! name being passed as parameters.
XC::LoadPattern *XC::MapLoadPatterns::newLoadPattern(const std::string &type,const std::string &cod_lp)
  {
    LoadPattern *retval= findLoadPattern(cod_lp);
    if(!retval)
      {
	if(type == "load_pattern" || type == "default" )
	  retval=  create_load_pattern<LoadPattern>(cod_lp);
	else if(type == "uniform_excitation")
	  retval= create_load_pattern<UniformExcitation>(cod_lp);
	else if(type == "multi_support_pattern")
	  retval= create_load_pattern<MultiSupportPattern>(cod_lp);
	else if(type == "pbowl_loading")
	  retval= create_load_pattern<PBowlLoading>(cod_lp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" << cod_lp
	        << "' already defined. Command ignored."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Remove the load pattern with the given name.
bool XC::MapLoadPatterns::removeLoadPattern(const std::string &cod_lp)
  {
    bool retval= false;
    const_iterator ilp= loadpatterns.find(cod_lp);
    if(ilp!=end())
      {
        LoadPattern *tmp= ilp->second;
	if(tmp)
	  {
	    delete tmp;
	    tmp= nullptr;
	  }
	loadpatterns.erase(ilp);
	retval= true;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern: '" << cod_lp
	        << "' not found. Command ignored."
	        << Color::def << std::endl;
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
  { return findLoadPattern(lpcode); }

const XC::LoadPattern *XC::MapLoadPatterns::getCurrentLoadPatternPtr(void) const
  { return findLoadPattern(lpcode); }

//! @brief Return the load patterns that act on the given node.
std::list<XC::LoadPattern *> XC::MapLoadPatterns::getLoadPatternsActingOn(const Node *n)
  {
    std::list<XC::LoadPattern *> retval;
    for(iterator i= begin();i!=end();i++)
      {
	LoadPattern *lp= (*i).second;
	if(lp->actsOn(n))
	  retval.push_back(lp);
      }
    return retval;
  }

//! @brief Return the load patterns that act on the given node.
boost::python::list XC::MapLoadPatterns::getLoadPatternsActingOnPy(const Node *n)
  {
    std::list<LoadPattern *> tmp= this->getLoadPatternsActingOn(n);
    boost::python::list retval;
    for(std::list<XC::LoadPattern *>::const_iterator i= tmp.begin(); i!=tmp.end(); i++)
      {
	LoadPattern *lp= *i;
	boost::python::object pyObj(boost::ref(*lp));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Removes the given node from all the load patterns.
void XC::MapLoadPatterns::removeLoadsOn(const Node *n)
  {
    std::list<XC::LoadPattern *> retval;
    for(iterator i= begin();i!=end();i++)
      {
	LoadPattern *lp= (*i).second;
	lp->removeLoadsOn(n);
      }
  }

//! @brief Copy the loads from the first node to the second one.
//! @param fromNode: node to copy the loads from.
//! @param toNode: node to copy the loads to.
void XC::MapLoadPatterns::copyLoads(const Node *fromNode, const Node *toNode)
  {
    std::list<XC::LoadPattern *> affectedLoadPatterns= this->getLoadPatternsActingOn(fromNode);
    for(std::list<XC::LoadPattern *>::iterator i= affectedLoadPatterns.begin();i!=affectedLoadPatterns.end();i++)
      {
	LoadPattern *lp= *i;
	lp->copyLoads(fromNode, toNode);
      }
  }

//! @brief Return the load patterns that act on the given element.
std::list<XC::LoadPattern *> XC::MapLoadPatterns::getLoadPatternsActingOn(const Element *e)
  {
    std::list<XC::LoadPattern *> retval;
    for(iterator i= begin();i!=end();i++)
      {
	LoadPattern *lp= (*i).second;
	if(lp->actsOn(e))
	  retval.push_back(lp);
      }
    return retval;
  }

//! @brief Return the load patterns that act on the given element.
boost::python::list XC::MapLoadPatterns::getLoadPatternsActingOnPy(const Element *e)
  {
    std::list<XC::LoadPattern *> tmp= this->getLoadPatternsActingOn(e);
    boost::python::list retval;
    for(std::list<XC::LoadPattern *>::const_iterator i= tmp.begin(); i!=tmp.end(); i++)
      {
	LoadPattern *lp= *i;
	boost::python::object pyObj(boost::ref(*lp));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Removes the given element from all the load patterns.
void XC::MapLoadPatterns::removeLoadsOn(const Element *e)
  {
    std::list<XC::LoadPattern *> retval;
    for(iterator i= begin();i!=end();i++)
      {
	LoadPattern *lp= (*i).second;
	lp->removeLoadsOn(e);
      }
  }

//! @brief Copy the loads from the first element to the second one.
//! @param fromElement: element to copy the loads from.
//! @param toElement: element to copy the loads to.
void XC::MapLoadPatterns::copyLoads(const Element *fromElement, const Element *toElement)
  {
    std::list<XC::LoadPattern *> affectedLoadPatterns= this->getLoadPatternsActingOn(fromElement);
    for(std::list<XC::LoadPattern *>::iterator i= affectedLoadPatterns.begin();i!=affectedLoadPatterns.end();i++)
      {
	LoadPattern *lp= *i;
	lp->copyLoads(fromElement, toElement);
      }
  }

//! @brief Return a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MapLoadPatterns::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::MapLoadPatterns::sendData(Communicator &comm)
  {
    int res= sendMap(tseries,comm,getDbTagData(),CommMetaData(0));
    res+= comm.sendString(time_series_name,getDbTagData(),CommMetaData(1));
    res+= sendMap(loadpatterns,comm,getDbTagData(),CommMetaData(2));
    res+= comm.sendString(lpcode,getDbTagData(),CommMetaData(3));
    res+= comm.sendInts(tag_el,tag_nl,tag_spc,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Send members through the communicator argument.
int XC::MapLoadPatterns::recvData(const Communicator &comm)
  {
    clear();
    int res= receiveMap(tseries,comm,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewTimeSeries);
    res+= comm.receiveString(time_series_name,getDbTagData(),CommMetaData(1));
    res+= receiveMap(loadpatterns,comm,getDbTagData(),CommMetaData(2),&FEM_ObjectBroker::getNewLoadPattern);
    for(iterator i= begin();i!= end();i++)
      (*i).second->set_owner(this);
    res+= comm.receiveString(lpcode,getDbTagData(),CommMetaData(3));
    res+= comm.receiveInts(tag_el,tag_nl,tag_spc,getDbTagData(),CommMetaData(4));
    return res;
  }
//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::MapLoadPatterns::getPyDict(void) const
  {
    boost::python::dict retval= LoadHandlerMember::getPyDict();
    boost::python::dict tSeriesDict;
    for(map_timeseries::const_iterator i= tseries.begin(); i!= tseries.end(); i++)
      {
	const std::string key= (*i).first;
	const TimeSeries *value= (*i).second;
	boost::python::dict tmp= value->getPyDict();
	tmp["classTag"]= value->getClassTag();
	tSeriesDict[key]= tmp;
      }
    retval["tseries"]= tSeriesDict;
    retval["time_series_name"]= time_series_name;
    boost::python::dict lPatternsDict;
    for(map_loadpatterns::const_iterator i= loadpatterns.begin(); i!= loadpatterns.end(); i++)
      {
	const std::string key= (*i).first;
	const LoadPattern *value= (*i).second;
	boost::python::dict tmp= value->getPyDict();
	tmp["classTag"]= value->getClassTag();
	lPatternsDict[key]= tmp;
      }
    retval["loadpatterns"]= lPatternsDict;
    retval["lpcode"]= lpcode;
    retval["tag_el"]= tag_el;
    retval["tag_nl"]= tag_nl;
    retval["tag_spc"]= tag_spc;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::MapLoadPatterns::setPyDict(const boost::python::dict &d)
  {
    LoadHandlerMember::setPyDict(d);
    boost::python::dict tSeriesDict= boost::python::extract<boost::python::dict>(d["tseries"]);
    boost::python::list ts_items= tSeriesDict.items();
    const size_t ts_sz= len(ts_items);
    if(ts_sz>0)
      {
	for(size_t i=0; i<ts_sz; i++)
	  {
	    const std::string key= boost::python::extract<std::string>(ts_items[i][0]);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(ts_items[i][1]);
	    const int classTag= boost::python::extract<int>(itemDict["classTag"]);
	    const std::string className= boost::python::extract<std::string>(itemDict["className"]);
	    TimeSeries *newObject= dynamic_cast<TimeSeries *>(get_new_movable_object(className, classTag));
	    if(newObject)
	      {
		newObject->setPyDict(itemDict);
		this->tseries[key]= newObject;
	      }
	    else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive object. Class mame: "
			<< className << " class tag: " << classTag
			<< Color::def << std::endl;
	  }	
      }
    this->time_series_name= boost::python::extract<std::string>(d["time_series_name"]);
    boost::python::dict lPatternsDict= boost::python::extract<boost::python::dict>(d["loadpatterns"]);
    boost::python::list lp_items= lPatternsDict.items();
    const size_t lp_sz= len(lp_items);
    if(lp_sz>0)
      {
	for(size_t i=0; i<lp_sz; i++)
	  {
	    const std::string key= boost::python::extract<std::string>(lp_items[i][0]);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(lp_items[i][1]);
	    const int classTag= boost::python::extract<int>(itemDict["classTag"]);
	    const std::string className= boost::python::extract<std::string>(itemDict["className"]);
	    LoadPattern *newObject= dynamic_cast<LoadPattern *>(get_new_tagged_object(className, classTag));
	    if(newObject)
	      {
		newObject->setPyDict(itemDict);
		this->loadpatterns[key]= newObject;
	      }
	    else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive object. Class mame: "
			<< className << " class tag: " << classTag
			<< Color::def << std::endl;
	  }	
      }
    this->lpcode= boost::python::extract<std::string>(d["lpcode"]);
    this->tag_el= boost::python::extract<int>(d["tag_el"]);
    this->tag_nl= boost::python::extract<int>(d["tag_nl"]);
    this->tag_spc= boost::python::extract<int>(d["tag_spc"]);    
  }

//! @brief Sends object through the communicator argument.
int XC::MapLoadPatterns::sendSelf(Communicator &comm)
  {
    inicComm(5);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
	        << Color::def << std::endl;    
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::MapLoadPatterns::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    int res= 0;
    //If not empty we suppose that current definitions are OK.
    if(empty())
      {
        const int dataTag= getDbTag();
        res= comm.receiveIdData(getDbTagData(),dataTag);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; data could not be received."
		    << Color::def << std::endl;
        else
          res+= recvData(comm);
      }
    return res;
  }

//! @brief Return the names of the load patterns.
std::deque<std::string> XC::MapLoadPatterns::getNamesList(void) const
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
