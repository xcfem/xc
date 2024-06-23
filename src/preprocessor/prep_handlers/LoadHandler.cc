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
//LoadHandler.cc

#include "LoadHandler.h"
#include "domain/domain/Domain.h"

//Ground motions.
#include "domain/load/groundMotion/GroundMotionRecord.h"
#include "domain/load/groundMotion/InterpolatedGroundMotion.h"
#include "domain/load/pattern/LoadCombination.h"
#include "utility/actor/actor/MovableMap.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::LoadHandler::LoadHandler(Preprocessor *owr)
  : PrepHandler(owr), lpatterns(this), tag_lp(0), combinations(this) {}

//! @brief Adds the load pattern to the domain.
void XC::LoadHandler::addToDomain(const std::string &lp_code)
  {
    LoadPattern *lp= lpatterns.findLoadPattern(lp_code);
    if(lp)
      {
        bool result= getDomain()->addLoadPattern(lp);
        if((!result) && (verbosity>3))
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; can't add the load pattern: '"
                    << lp_code << "'"
	            << Color::def << std::endl;
      }
    else
      combinations.addToDomain(lp_code);
  }

//! @brief Remove load pattern from domain.
void XC::LoadHandler::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= lpatterns.findLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      combinations.removeFromDomain(lp_code);
  }

void XC::LoadHandler::removeAllFromDomain(void)
  {
    combinations.removeAllFromDomain();
    lpatterns.removeAllFromDomain();
  }

//! @brief Clears all the objects.
void XC::LoadHandler::clearAll(void)
  {
    combinations.clear();
    lpatterns.clear();
  }

//! @brief Destructor.
XC::LoadHandler::~LoadHandler(void)
  { clearAll(); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::LoadHandler::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::LoadHandler::sendData(Communicator &comm)
  {
    int res= sendMap(ground_motions,comm,getDbTagData(),CommMetaData(0));
    res+= comm.sendMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= comm.sendInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= comm.sendMovable(combinations,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Send members through the communicator argument.
int XC::LoadHandler::recvData(const Communicator &comm)
  {
    int res= receiveMap(ground_motions,comm,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewGroundMotion);
    res+= comm.receiveMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= comm.receiveInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= comm.receiveMovable(combinations,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::LoadHandler::getPyDict(void) const
  {
    boost::python::dict retval= PrepHandler::getPyDict();
    boost::python::dict ground_motions_dict;
    for(map_ground_motions::const_iterator i= ground_motions.begin(); i!= ground_motions.end(); i++)
      {
	const std::string key= (*i).first;
	const GroundMotion *value= (*i).second;
	if(value)
	  {
	    boost::python::dict tmp= value->getPyDict();
	    tmp["classTag"]= value->getClassTag();
	    ground_motions_dict[key]= tmp;
	  }
	else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; null pointer for ground motion: " << key
		    << Color::def << std::endl;
      }
    retval["ground_motions"]= ground_motions_dict;
    retval["lpatterns"]= this->lpatterns.getPyDict();
    retval["tag_lp"]= this->tag_lp;
    retval["combinations"]= this->combinations.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::LoadHandler::setPyDict(const boost::python::dict &d)
  {
    PrepHandler::setPyDict(d);
    boost::python::dict ground_motions_dict= boost::python::extract<boost::python::dict>(d["ground_motions"]);
    boost::python::list items= ground_motions_dict.items();
    const size_t sz= len(items);
    if(sz>0)
      {
	for(size_t i=0; i<sz; i++)
	  {
	    const std::string key= boost::python::extract<std::string>(items[i][0]);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(items[i][1]);
	    const int classTag= boost::python::extract<int>(itemDict["classTag"]);
	    const std::string className= boost::python::extract<std::string>(itemDict["className"]);
	    GroundMotion *newObject= dynamic_cast<GroundMotion *>(get_new_movable_object(className, classTag));
	    if(newObject)
	      {
		newObject->setPyDict(itemDict);
		this->ground_motions[key]= newObject;
	      }
	    else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive object. Class mame: "
			<< className << " class tag: " << classTag
			<< Color::def << std::endl;
	  }
	
      }
    this->lpatterns.setPyDict(boost::python::extract<boost::python::dict>(d["lpatterns"]));
    this->tag_lp= boost::python::extract<int>(d["tag_lp"]);
    this->combinations.setPyDict(boost::python::extract<boost::python::dict>(d["combinations"]));
  }

//! @brief Sends object through the communicator argument.
int XC::LoadHandler::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    DbTagData &dbTagData= getDbTagData();
    inicComm(dbTagData.Size());
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::LoadHandler::recvSelf(const Communicator &comm)
  {
    DbTagData &dbTagData= getDbTagData();
    inicComm(dbTagData.Size());
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
		<< Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
	            << Color::def << std::endl;
      }
    return res;
  }


