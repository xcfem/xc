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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.13 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LoadPattern.cpp,v $

// Written: fmk 07/99
// Revised:
//
// Purpose: This file contains the method definitions for class LoadPattern.


#include "LoadPattern.h"
#include <cstdlib>
#include <utility/matrix/ID.h>
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include <domain/constraints/SFreedom_Constraint.h>
#include <domain/load/pattern/TimeSeries.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>

#include "domain/load/NodalLoad.h"
#include "domain/load/ElementalLoad.h"
#include "domain/load/elem_load.h"


#include "utility/actor/actor/ArrayCommMetaData.h"

#include "preprocessor/prep_handlers/LoadHandler.h"
#include "utility/utils/misc_utils/colormod.h"

void XC::LoadPattern::free(void)
  {
    setTimeSeries(nullptr);

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads.resize(0);
    // AddingSensitivity:END ///////////////////////////////
  }

//! @brief Constructor
//! 
//! @param tag: load pattern identifier.
//! @param classTag: class identifier.
XC::LoadPattern::LoadPattern(int tag, int classTag)
  : NodeLocker(tag,classTag), loadFactor(0.0), gamma_f(1.0),
    theSeries(nullptr), theLoads(this), randomLoads(), isConstant(false)
  {}


//! @brief Constructor.
XC::LoadPattern::LoadPattern(int tag)
  : NodeLocker(tag,PATTERN_TAG_LoadPattern),loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theLoads(this), randomLoads(), isConstant(false)
  {}

//! @brief Virtual constructor.
XC::LoadPattern *XC::LoadPattern::getCopy(void)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented."
	      << Color::def << std::endl;
    return nullptr;
  }

//! @brief Destructor.
XC::LoadPattern::~LoadPattern(void)
  { free(); }

//! @brief Returns a const pointer to the container of the load pattern.
const XC::MapLoadPatterns *XC::LoadPattern::getMapLoadPatterns(void) const
  {
    const MapLoadPatterns *retval= dynamic_cast<const MapLoadPatterns *>(Owner());
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern container not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns a pointer to the container of the load pattern.
XC::MapLoadPatterns *XC::LoadPattern::getMapLoadPatterns(void)
  {
    MapLoadPatterns *retval= dynamic_cast<MapLoadPatterns *>(Owner());
    if(!retval)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; load pattern container not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Returns the name of this load pattern.
const std::string &XC::LoadPattern::getName(void) const
  {
    const MapLoadPatterns *mhandler= getMapLoadPatterns();
    return mhandler->getLoadPatternName(this);
  }

//! @brief Returns the name of this load pattern multiplied
//! by its partial safety factor.
std::string XC::LoadPattern::getFactoredName(void) const
  {
    std::string retval= getName();
    if(gamma_f!=1.0)
      {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << gamma_f;
        retval= ss.str()+'*'+retval;
      }
    return retval;
  }

//! @brief Return the time series pointer (const version).
const XC::TimeSeries *XC::LoadPattern::getTimeSeries(void) const
  { return theSeries; }

//! @brief Return the time series pointer.
XC::TimeSeries *XC::LoadPattern::getTimeSeries(void)
  { return theSeries; }

//! @brief Set the time series for the pattern.
void XC::LoadPattern::setTimeSeries(TimeSeries *theTimeSeries)
  {
    // invoke the destructor on the old TimeSeries
    if(theSeries)
      { 
        delete theSeries;
        theSeries= nullptr;
      }
    // set the pointer to the new series object
    if(theTimeSeries)
      theSeries= theTimeSeries->getCopy();
    else
      theSeries= nullptr;
  }


//! @brief Set the domain for the pattern loads.
void XC::LoadPattern::setDomain(Domain *theDomain)
  {
    NodeLocker::setDomain(theDomain);
    theLoads.setDomain(theDomain);
  }

//! @brief Apply load.
bool XC::LoadPattern::addToDomain(void)
  {
    bool retval= false;
    Domain *theDomain= this->getDomain();
    if(theDomain)
      retval= theDomain->addLoadPattern(this);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; WARNING: null pointer to the domain."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Removes load.
void XC::LoadPattern::removeFromDomain(void)
  {
    Domain *theDomain = this->getDomain();
    if(theDomain)
      theDomain->removeLoadPattern(this);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; WARNING: null pointer to the domain."
	        << Color::def << std::endl;
  }

//! @brief Return true if the load pattern is already added
//! to the domain.
bool XC::LoadPattern::isActive(void) const
  {
    bool retval= false;
    const Domain *theDomain = this->getDomain();
    if(theDomain)
      retval= theDomain->isLoadPatternActive(this);
    return retval;
  }

//! @brief Returns true if there is no loads nor constraints.
bool XC::LoadPattern::empty(void) const
  {
    bool retval= NodeLocker::empty();
    if(retval)
      retval= theLoads.empty();
    return retval;
  }

//! @brief Adds the nodal load being passed as parameter.
bool XC::LoadPattern::addNodalLoad(NodalLoad *load)
  {
    Domain *theDomain = this->getDomain();

    bool result = theLoads.addNodalLoad(load);
    if(result == true)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; WARNING: load could not be added."
	        << Color::def << std::endl;
    return result;
  }

//! @brief Creates a nodal load.
//! 
//! @param nodeTag: node identifier.
//! @param f: load vector.
XC::NodalLoad *XC::LoadPattern::newNodalLoad(const int &nodeTag,const Vector &f)
  {
    if(isActive())
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << "; this load pattern is already active,"
	        << " loads must be defined previously."
	        << " Results are undefined."
		<< Color::def << std::endl;
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentNodeLoadTag();
    NodalLoad *retval= new NodalLoad(nextTag,nodeTag,f);
    if(addNodalLoad(retval))
      map->setCurrentNodeLoadTag(++nextTag);
    return retval;
  }

//! @brief Creates a new load over elements.
//! 
//! @param loadType: load type
XC::ElementalLoad *XC::LoadPattern::newElementalLoad(const std::string &loadType)
  {
    if(isActive())
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << "; this load pattern is already active,"
	        << " loads must be defined previously."
	        << " Results are undefined."
		<< Color::def << std::endl;
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentElementLoadTag();
    ElementalLoad *retval= process_element_load(this,nextTag,loadType);
    if(retval)
      map->setCurrentElementLoadTag(++nextTag);
    return retval;
  }

//! @brief Adds the element load being passed as parameter.
bool XC::LoadPattern::addElementalLoad(ElementalLoad *load)
  {
    Domain *theDomain= getDomain();

    const bool result= theLoads.addElementalLoad(load);
    if(result)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; WARNING - load could not be added."
	        << Color::def << std::endl;
    return result;
  }

//! @brief Appends the elemental load being passed as parameter.
bool XC::LoadPattern::newElementalLoad(ElementalLoad *load)
  {
    if(isActive())
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << "; this load pattern is already active,"
	        << " loads must be defined previously."
	        << " results are undefined."
		<< Color::def << std::endl;
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    int nextTag= map->getCurrentElementLoadTag();
    bool result= false;
    if(load)
      {
        result= addElementalLoad(load);
        if(result)
          map->setCurrentElementLoadTag(++nextTag); //increments by one the corresponding tag
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; the pointer to the load es nulo."
		<< Color::def << std::endl;
    return result;
  }

// //! @brief Crea the load over element being passed as parameter.
// XC::NodalLoad *XC::MapLoadPatterns::newElementLoad(const int &tag_elem,const Vector &f)
//   {
//     MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(owner());
//     int nextTag= map->getCurrentNodeLoadTag();
//     NodalLoad *retval= new NodalLoad(nextTag,nodeTag,f);
//     bool retval= addNodalLoad(theLoad);
//     if(addNodalLoad(theLoad))
//       map->setCurrentNodeLoadTag(++nextTag);
//     return retval;
//   }

//! @brief Adds the constraint being passed as parameter.
bool XC::LoadPattern::addSFreedom_Constraint(SFreedom_Constraint *theSp)
  { return NodeLocker::addSFreedom_Constraint(theSp); }

//! @brief Deletes all loads.
void XC::LoadPattern::clearLoads(void)
  {
    theLoads.clearAll();
  }

//! @brief Deletes all loads, constraints AND pointer to time series.
void XC::LoadPattern::clearAll(void)
  {
    clearLoads();
    NodeLocker::clearAll();
    setTimeSeries(nullptr);
  }

//! @brief Remove the nodal load which tag is being passed as parameter.
bool XC::LoadPattern::removeNodalLoad(int tag)
  {
    bool result= theLoads.removeNodalLoad(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Remove the load over element which tag is being passed as parameter.
bool XC::LoadPattern::removeElementalLoad(int tag)
  {
    bool result= theLoads.removeElementalLoad(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Return true if the load pattern acts on the given node.
bool XC::LoadPattern::actsOn(const Node *n) const
  { return theLoads.actsOn(n); }

//! @brief Removes the given node from all the load patterns.
void XC::LoadPattern::removeLoadsOn(const Node *n)
  { theLoads.removeLoadsOn(n); }

//! @brief Copy the loads from the first node to the second one.
//! @param fromNode: node to copy the loads from.
//! @param toNode: node to copy the loads to.
void XC::LoadPattern::copyLoads(const Node *fromNode, const Node *toNode)
  {
    std::list<NodalLoad *> loads= theLoads.getLoadsOn(fromNode);
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    for(std::list<NodalLoad *>::const_iterator i= loads.begin(); i!=loads.end();i++)
      {
	NodalLoad *orgLoad= *i;
	int nextTag= map->getCurrentNodeLoadTag();
	NodalLoad *destLoad= orgLoad->getCopy(nextTag);
	destLoad->setNodeTag(toNode->getTag());
	if(addNodalLoad(destLoad))
	  map->setCurrentNodeLoadTag(++nextTag);
      }
  }

//! @brief Return true if the load pattern acts on the given element.
bool XC::LoadPattern::actsOn(const Element *e) const
  { return theLoads.actsOn(e); }

//! @brief Removes the given element from all the load patterns.
void XC::LoadPattern::removeLoadsOn(const Element *e)
  { theLoads.removeLoadsOn(e); }

//! @brief Copy the loads from the first element to the second one.
//! @param fromElement: element to copy the loads from.
//! @param toElement: element to copy the loads to.
void XC::LoadPattern::copyLoads(const Element *fromElement, const Element *toElement)
  { theLoads.copyLoads(fromElement, toElement); }

//! @brief Apply the load for pseudo-time being passed as parameter.
void XC::LoadPattern::applyLoad(double pseudoTime)
  {
    if(theSeries)  // first determine the load factor
      {
        if(!isConstant)
          loadFactor= theSeries->getFactor(pseudoTime);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: time series not defined. Using load factor: "
	        << loadFactor
		<< Color::def << std::endl;
    const double factor= loadFactor*gamma_f; //Weighting of the case.

    theLoads.applyLoad(factor);

    NodeLocker::applyLoad(pseudoTime,factor);
  }

//! @brief Marks the LoadPattern as being constant. Subsequent calls to {\em
//! applyLoad()} will use the current load factor.
void XC::LoadPattern::setLoadConstant(void)
  { isConstant= true; }

//! @brief Returns the weighting factor obtained from the TimeSeries object.
const double &XC::LoadPattern::getLoadFactor(void) const
  { return loadFactor; }

//! @brief Returns the partial safety factor according to the load combination.
const double &XC::LoadPattern::GammaF(void) const
  { return gamma_f; }

//! @brief Returns the partial safety factor according to the load combination.
double &XC::LoadPattern::GammaF(void)
  { return gamma_f; }

//! @brief Sets the partial safety factor according to the load combination.
void XC::LoadPattern::setGammaF(const double &g)
  { gamma_f= g; }

//! @brief Multiplies the partial safety factor.
XC::LoadPattern &XC::LoadPattern::operator*=(const double &fact)
  {
    gamma_f*= fact;
    return *this;
  }

//! @brief Divides the partial safety factor.
XC::LoadPattern &XC::LoadPattern::operator/=(const double &fact)
  {
    gamma_f/= fact;
    return *this;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::LoadPattern::getDbTagData(void) const
  {
    static DbTagData retval(15);
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::LoadPattern::getPyDict(void) const
  {
    boost::python::dict retval= NodeLocker::getPyDict();
    retval["description"]= this->description;
    retval["load_factor"]= this->loadFactor;
    retval["gamma_f"]= this->gamma_f;
    retval["ts_class_tag"]= this->theSeries->getClassTag();
    retval["ts_class_name"]= this->theSeries->getClassName();
    retval["loads"]= this->theLoads.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::LoadPattern::setPyDict(const boost::python::dict &d)
  {
    NodeLocker::setPyDict(d);
    this->description= boost::python::extract<std::string>(d["description"]);
    this->loadFactor= boost::python::extract<double>(d["load_factor"]);
    this->gamma_f= boost::python::extract<double>(d["gamma_f"]);
    const int ts_class_tag= boost::python::extract<int>(d["ts_class_tag"]);
    const std::string ts_class_name= boost::python::extract<std::string>(d["ts_class_name"]);
    TimeSeries *newTimeSeries= dynamic_cast<TimeSeries *>(get_new_movable_object(ts_class_name, ts_class_tag));
    if(newTimeSeries)
      this->setTimeSeries(newTimeSeries);
    else
      {
	this->setTimeSeries(nullptr);
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; failed to read time series."
		  << Color::def << std::endl;
      }      
    this->theLoads.setPyDict(boost::python::extract<boost::python::dict>(d["loads"]));
  }

//! @brief Send members through the communicator argument.
int XC::LoadPattern::sendData(Communicator &comm)
  {
    int res= NodeLocker::sendData(comm);
    res+= comm.sendDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    res+= sendTimeSeriesPtr(theSeries,8,9,getDbTagData(),comm);
    res+= comm.sendMovable(theLoads,getDbTagData(),CommMetaData(10));

    res+= comm.sendVector(randomLoads,getDbTagData(),CommMetaData(11));
    res+= comm.sendBools(RVisRandomProcessDiscretizer,isConstant,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::LoadPattern::recvData(const Communicator &comm)
  {
    int res= NodeLocker::recvData(comm);
    res+= comm.receiveDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    theSeries= receiveTimeSeriesPtr(theSeries,8,9,getDbTagData(),comm);
    res+= comm.receiveMovable(theLoads,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(randomLoads,getDbTagData(),CommMetaData(11));
    res+= comm.receiveBools(RVisRandomProcessDiscretizer,isConstant,getDbTagData(),CommMetaData(12));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::LoadPattern::sendSelf(Communicator &comm)
  {
    inicComm(15);
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
int XC::LoadPattern::recvSelf(const Communicator &comm)
  {
    inicComm(15);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received."
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Prints load pattern information.
void XC::LoadPattern::Print(std::ostream &s, int flag) const
  {
    s << "Load Pattern: " << this->getTag() << "\n";
    s << "  load factor: " << loadFactor << "\n";
    s << "  partial safety factor gamma_f: " << gamma_f << "\n";
    if(theSeries)
      theSeries->Print(s,flag);
    s << "  Loads: \n";
    theLoads.Print(s,flag);
    NodeLocker::Print(s,flag);
  }

int XC::LoadPattern::addMotion(GroundMotion &theMotion, int tag)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; cannot add GroundMotion"
              << "- use MultiSupport Pattern instead."
              << Color::def << std::endl;
    return -1;
  }

XC::GroundMotion *XC::LoadPattern::getMotion(int tag)
  { return nullptr; }

// AddingSensitivity:BEGIN ////////////////////////////////////
void XC::LoadPattern::applyLoadSensitivity(double pseudoTime)
  {
    if(theSeries && !isConstant)
      loadFactor= theSeries->getFactorSensitivity(pseudoTime);

    const double factor= loadFactor*gamma_f;
    theLoads.applyLoadSensitivity(factor);

    // Don't include sp constraints for now
    /*
      SFreedom_Constraint *sp;
      SFreedom_ConstraintIter &theIter = this->getSPs();
      while((sp = theIter()) != 0)
      sp->applyConstraint(factor);
    */
  }

int XC::LoadPattern::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(!theSeries)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; set/update/activate parameter is illegaly called."
		  << Color::def << std::endl;
      }

    //const int argc= argv.size();
    if(strstr(argv[0].c_str(),"randomProcessDiscretizer") != 0)
      {
        if(argv.size() < 2)
          return -1;
        RVisRandomProcessDiscretizer = true;
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+1);
        return theSeries->setParameter(argv2, param);
      }
    else if(strstr(argv[0].c_str(),"loadAtNode") != 0) // Nodal load
      {
        RVisRandomProcessDiscretizer = false;
        return theLoads.setParameter(argv,param);
      }
    else if(strstr(argv[0].c_str(),"elementPointLoad") != 0)
      {
        RVisRandomProcessDiscretizer = false;
        return theLoads.setParameter(argv,param);
      }
    else // Unknown parameter
        return -1;
  }

int XC::LoadPattern::updateParameter(int parameterID, Information &info)
  {
    if(!theSeries)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "set/update/activate parameter is illegaly called."
		  << Color::def << std::endl;
      }
    if(RVisRandomProcessDiscretizer)
      { return theSeries->updateParameter(parameterID,info); }
    else
      { return theLoads.updateParameter(parameterID,info); }
  }


int XC::LoadPattern::activateParameter(int parameterID)
  {
    if(!theSeries)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "set/update/activate parameter is illegaly called."
		  << Color::def << std::endl;
      }

    if(RVisRandomProcessDiscretizer)
      { return theSeries->activateParameter(parameterID); }
    else
      { return theLoads.activateParameter(parameterID); }
  }

const XC::Vector &XC::LoadPattern::getExternalForceSensitivity(int gradNumber)
  {
    // THIS METHOD IS CURRENTLY ONLY USED FOR THE STATIC CASE
    // IT SHOULD BE DELETED AND REPLACED BY THE DYNAMIC CASE

    // Start with a fresh return vector
    randomLoads= theLoads.getExternalForceSensitivity(gradNumber);
    return randomLoads;
  }

// AddingSensitivity:END //////////////////////////////////////
