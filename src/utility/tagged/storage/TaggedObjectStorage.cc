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
//TaggedObjectStorage.cc

#include "TaggedObjectStorage.h"
#include "utility/tagged/TaggedObject.h"
#include "TaggedObjectIter.h"
#include <boost/any.hpp>


#include "utility/actor/actor/MovableObject.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/utils/misc_utils/colormod.h"


XC::ID XC::TaggedObjectStorage::dbTags;
XC::ID XC::TaggedObjectStorage::classTags;
XC::ID XC::TaggedObjectStorage::objTags;
const int XC::TaggedObjectStorage::posDbTag1;
const int XC::TaggedObjectStorage::posDbTag2;
const int XC::TaggedObjectStorage::posDbTag3;

//! @brief Constructor.
//!
//! @param owr: object owner (this object is somewhat contained by).
//! @param tag: name for this container.
XC::TaggedObjectStorage::TaggedObjectStorage(CommandEntity *owr,const std::string &contrName)
  : CommandEntity(owr), MovableObject(0), containerName(contrName), transmitIDs(true) {}

//! @brief Copy the components from the container into this one.
void XC::TaggedObjectStorage::copy(const TaggedObjectStorage &other)
  {
    TaggedObject *ptr= nullptr;
    TaggedObjectStorage &other_no_const= const_cast<TaggedObjectStorage &>(other);
    TaggedObjectIter &theIter= other_no_const.getComponents();
    while((ptr= theIter()) != nullptr)
      addComponent(ptr->getCopy());
  }

//! @brief Returns true if the component identified
//! by the argument exists.
bool XC::TaggedObjectStorage::existComponent(int tag)
  { return (getComponentPtr(tag)!=nullptr); }

//! @brief Returns the DBTags of the objects.
const XC::ID &XC::TaggedObjectStorage::getDBTags(Communicator &comm)
  {
    static ID retval;
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectIter &theIter= getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            retval(loc)= tmp->getDbTag(comm);
            loc++;
          }        
      }
    return retval;
  }

//! @brief Returns the classTags of the objects.
const XC::ID &XC::TaggedObjectStorage::getClassTags(void) const
  {
    static ID retval;
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectStorage *this_no_const= const_cast<TaggedObjectStorage *>(this);
        TaggedObjectIter &theIter=  this_no_const->getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            retval(loc)= tmp->getClassTag();
            loc++;
          }        
      }
    return retval;
  }

//! @brief Returns the tags of the objects.
const XC::ID &XC::TaggedObjectStorage::getObjTags(void) const
  {
    static ID retval;
    retval.Zero();
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectStorage *this_no_const= const_cast<TaggedObjectStorage *>(this);
        TaggedObjectIter &theIter= this_no_const->getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            retval(loc)= ptr->getTag();
            loc++;
          }
      }
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::TaggedObjectStorage::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Returns the dbTags y classTags of the objects through the communicator argument.
int XC::TaggedObjectStorage::sendObjectTags(Communicator &comm)
  {
    const int size= getNumComponents();
    int res= 0;
    if(size>0)
      {
        dbTags= getDBTags(comm);
        res+= comm.sendID(dbTags,getDbTagData(),CommMetaData(posDbTag1));
        classTags= getClassTags();
        res+= comm.sendID(classTags,getDbTagData(),CommMetaData(posDbTag2));
        objTags= getObjTags();
        res+= comm.sendID(objTags,getDbTagData(),CommMetaData(posDbTag3));
      }
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; communicator failed to send the IDs."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives the dbTags or classTags of the objects through the communicator argument.
const XC::ID &XC::TaggedObjectStorage::receiveTags(int posDbTag,int sz,const Communicator &comm)
  {
    static ID retVal;
    int res= 0;
    if(sz>0)
      {
        retVal.resize(sz);
        res= comm.receiveID(retVal,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; communicator failed to receive the IDs."
		<< Color::def << std::endl;
    return retVal;
  }

//! @brief Receive object tags through the communicator argument.
int XC::TaggedObjectStorage::receiveObjectTags(const int &sz,const Communicator &comm)
  {
    int res= 0;
    dbTags= receiveTags(posDbTag1,sz,comm);
    classTags= receiveTags(posDbTag2,sz,comm);
    objTags= receiveTags(posDbTag3,sz,comm);
    return res;
  }

//! @brief Send objects through the communicator argument.
int XC::TaggedObjectStorage::sendObjects(Communicator &comm)
  {
    const int size= getNumComponents();
    int res= 0;
    if(size>0)
      {
        TaggedObject *ptr= nullptr;
        TaggedObjectIter &theIter= getComponents();
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            if(tmp)
              {
                tmp->setDbTag(comm);
                res= tmp->sendSelf(comm);
                if(res<0)
                  {
                    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		              << "; object with tag "
                              << ptr->getTag() << " failed in sendSelf."
		              << Color::def << std::endl;
                    break;
                  }
              }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; the object with tag: "
                        << ptr->getTag() << " is not movable."
			<< Color::def << std::endl;
          }
      }
    return res;
  }

//! @brief Receive the objects through the communicator being
//! passed as parameter.
int XC::TaggedObjectStorage::receiveObjects(const Communicator &comm)
  {
    TaggedObject *ptr= nullptr;
    TaggedObjectIter &theIter= getComponents();
    int res= 0;
    while((ptr= theIter()) != nullptr)
      {
        MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
        if(tmp)
          {
            res= tmp->recvSelf(comm);
            if(res<0)
              {
                std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; object with tag "
                          << ptr->getTag() << " failed."
			  << Color::def << std::endl;
                break;
              }
          }
        else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; the object with tag: " << ptr->getTag()
		    << " is not movable."
		    << Color::def << std::endl;
      }
    return res;
  }

boost::python::dict XC::TaggedObjectStorage::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    const int size= getNumComponents();
    boost::python::list values;
    if(size>0)
      {
        TaggedObject *ptr= nullptr;
	TaggedObjectStorage *this_no_const= const_cast<TaggedObjectStorage *>(this);
        TaggedObjectIter &theIter= this_no_const->getComponents();
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
	    const MovableObject *mv_tmp= dynamic_cast<MovableObject *>(ptr);
	    int classTag= -1;
	    if(mv_tmp) // is movable.
	      {
		classTag= mv_tmp->getClassTag();
	      }
	    else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; the object with tag: " << ptr->getTag()
			<< " is not movable."
			<< Color::def << std::endl;
	      
            const TaggedObject *tg_tmp= dynamic_cast<const TaggedObject *>(ptr);
            if(tg_tmp)
              {
		boost::python::dict objDict= tg_tmp->getPyDict();
		objDict["classTag"]= classTag;
		values.append(objDict);
	      }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                        << "; failed when reading object: "
			<< ptr->getTag()
			<< Color::def << std::endl;
          }
      }
    retval["values"]= values; 
    return retval;    
  }

void XC::TaggedObjectStorage::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    boost::python::list values= boost::python::extract<boost::python::list>(d["values"]);
    const size_t sz= len(values);
    for(size_t i= 0; i<sz; i++)
      {
	boost::python::dict objectDict= boost::python::extract<boost::python::dict>(values[i]);
	const int classTag= boost::python::extract<int>(objectDict["classTag"]);
	const std::string className= boost::python::extract<std::string>(objectDict["className"]);
	TaggedObject *newObject= get_new_tagged_object(className, classTag);
	if(newObject)
	  {
	    newObject->setPyDict(objectDict);
	    this->addComponent(newObject);
	  }
	else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive object. Class mame: " << className << " class tag: " << classTag
		    << Color::def << std::endl;
      }
  }


//! @brief Send members through the communicator argument.
int XC::TaggedObjectStorage::sendData(Communicator &comm)
  {
    const int dbTag= getDbTag(comm);
    const int sz= getNumComponents();
    setDbTagDataPos(0,dbTag);
    setDbTagDataPos(1,sz);

    int res= 0;
    if(sz>0)
      {
        res+= sendObjects(comm);
        if(transmitIDs)
          {
            res+= sendObjectTags(comm);
            transmitIDs= false; //Ya se han enviado.
          }
      }
    return res;
  }

int XC::TaggedObjectStorage::sendSelf(Communicator &comm)
  {
    inicComm(5);
    int res= sendData(comm);

    const int dbTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::TaggedObjectStorage::recvSelf(const Communicator &comm)
  { return receiveObjects(comm); }

