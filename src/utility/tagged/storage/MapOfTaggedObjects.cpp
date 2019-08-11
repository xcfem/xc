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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/MapOfTaggedObjects.cpp,v $
                                                                        
                                                                        
// File: ~/utility/tagged/storage/MapOfTaggedObjects.C
//
// Written: fmk 
// Created: 02/00
// Revision: A
//
// Purpose: This file contains the implementation of the XC::MapOfTaggedObjects
// class.
//
// What: "@(#) MapOfTaggedObjects.C, revA"

#include "MapOfTaggedObjects.h"
#include <utility/tagged/TaggedObject.h>

#include "boost/any.hpp"

// some typedefs that will be useful

//! @brief Creates the map object and an iter for iterating through the objects
//! that are added to the map.
//!
//! @param owr: object owner (this object is somewhat contained by).
XC::MapOfTaggedObjects::MapOfTaggedObjects(CommandEntity *owr,const std::string &containerName)
  : TaggedObjectStorage(owr,containerName), myIter(*this) {}

//! @brief Copy constructor.
XC::MapOfTaggedObjects::MapOfTaggedObjects(const MapOfTaggedObjects &other)
  : TaggedObjectStorage(other), myIter(*this)
  {
    copy(other);
  }

//! @brief Assignment operator.
XC::MapOfTaggedObjects &XC::MapOfTaggedObjects::operator=(const MapOfTaggedObjects &other)
  {
    TaggedObjectStorage::operator=(other);
    copy(other);
    return *this;
  }

XC::MapOfTaggedObjects::~MapOfTaggedObjects(void)
  { clearComponents(); }

//! @brief Set the container size.
//!
//! Checks to see that max size for the map (which is a built in value
//! defined for the template class) is larger than \p newSize. Returns
//! \f$0\f$ if successful. If not successful, a warning is raised
//! and \f$-1\f$ is returned.
int XC::MapOfTaggedObjects::setSize(int newSize)
  {
    // no setSize for map template .. can only check enough space available
    int maxSize = theMap.max_size();
    if(newSize > maxSize)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed as map stl has a max size of " << maxSize << "\n";
        return -1;
      } 
   
    return 0;
  }

//! @brief Adds a component to the container.
//!
//! To add the object \p newComponent to the container. First checks to
//! see if an element with a similar tag already exists in the map. If
//! not, the pointer to \p newElement is added to the map using the
//! insert() method. A check is then made to ensure that the object
//! has been added. (This is done as insert() returns no error flag).
//! Returns \p true if successful. If not successful, a warning is raised
//! and false is returned. Note that the map template does not allow items with
//! duplicate keys to be added. 
bool XC::MapOfTaggedObjects::addComponent(TaggedObject *newComponent)
  {
    int tag = newComponent->getTag();

    // check if the ele already in map, if not we add
    iterator theEle = theMap.find(tag);
    if(theEle==end())
      {
        newComponent->set_owner(this);
	theMap.insert(value_type(tag,newComponent));
        transmitIDs= true; //Component added.
		      
	// check if successfully added 
	theEle = theMap.find(tag);
	if(theEle == theMap.end())
         {
	   std::cerr << getClassName() << "::" << __FUNCTION__
		     << "; map STL failed to add object with tag : " << 
	     newComponent->getTag() << "\n";
	   return false;
	 }
      }
    // if ele already there map cannot add even if allowMultiple is true
    // as the map template does not allow multiple entries with the same tag
    else
      {	
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; not adding as one with similar tag exists, tag: " <<
	  newComponent->getTag() << "\n";
        return false;
      }
    return true;  // o.k.
  }

//! @brief Adds a component to the container.
//!
//! To remove the component whose tag is given by \p tag from the
//! container and return a pointer to the object. Invokes {\em find(tag)}
//! on the map to first see if the element is there. If it is {\em
//! erase(tag)} is invoked on the map to remove the item. \f$0\f$ is returned
//! if the component is not in the map, otherwise a pointer to the component
//! is returned.
bool XC::MapOfTaggedObjects::removeComponent(int tag)
  {
    bool retval= false;
    TaggedObject *tmp= nullptr;
    // return 0 if component does not exist, otherwise remove it
    iterator theEle = theMap.find(tag);
    if(theEle!=end()) // the object has not been removed
      { // the object exists so we remove it
	tmp= (*theEle).second;
	int ok= theMap.erase(tag);
        delete tmp;
        retval= true;
        transmitIDs= true; //Component removed.
	if(ok != 1)
          { // ensure the map did remove the object
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; map STL failed to remove object with tag "
		      << tag << "\n";
	  }
      }
    return retval;
  }

//! @brief Returns the number of components currently stored in the
//! container. This is found by invoking size() on the map object.
int XC::MapOfTaggedObjects::getNumComponents(void) const
  { return theMap.size(); }


//! @brief To return a pointer to the TaggedObject whose identifier is given by
//! \p tag.
//! 
//! To return a pointer to the TaggedObject whose identifier is given by
//! \p tag. Invokes {\em find(tag)} on the map to determine if the
//! component is in the container. If it is a pointer to the component is
//! returned. If it is not in the map \f$0\f$ is returned.
XC::TaggedObject *XC::MapOfTaggedObjects::getComponentPtr(int tag)
  {
    const MapOfTaggedObjects *cthis= static_cast<const MapOfTaggedObjects *>(this);
    return const_cast<TaggedObject *>(cthis->getComponentPtr(tag));
  }

//! @brief To return a pointer to the TaggedObject whose identifier is given by
//! \p tag. Const version of the method.
const XC::TaggedObject *XC::MapOfTaggedObjects::getComponentPtr(int tag) const
  {
    TaggedObject *retval= nullptr;
    // return nullptr if component does not exist.
    const_iterator theEle= theMap.find(tag);
    if(theEle == end()) 
      return nullptr;
    else 
      retval = (*theEle).second;
    return retval;
  }

XC::TaggedObjectIter &XC::MapOfTaggedObjects::getComponents(void)
  {
    myIter.reset();
    return myIter;
  }


//! @brief To return an iter for iterating through the objects that have been
//! added to the container. Each MapOfTaggedObjects object has its own iter. This
//! iter() is first reset and a reference to this iter is then returned. 
XC::MapOfTaggedObjectsIter XC::MapOfTaggedObjects::getIter(void)
  { return XC::MapOfTaggedObjectsIter(*this); }

//! @brief Get an empty copy of the method.
//!
//! Returns a pointer to a new MapOfTaggedObjects which was created using
//! new(). The new container that is returned is an empty container.
//! If not enough memory is available to create this object a warning
//! is raised and \f$0\f$ is returned. Note that it is the responsibility of
//! the caller to invoke the destructor on the object that is returned.
XC::TaggedObjectStorage *XC::MapOfTaggedObjects::getEmptyCopy(void)
  {
    MapOfTaggedObjects *theCopy = new MapOfTaggedObjects(Owner(),containerName);  
    if(!theCopy)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; out of memory\n";
    return theCopy;
  }

//! @brief Free memory reserved for components.
void XC::MapOfTaggedObjects::clearComponents(void)
  {
    iterator p= begin();
    while(p!= end())
      {
	delete (*p).second;
        (*p).second= nullptr;
	p++;
      }
  }

//! @brief Remove all objects from the container.
//!
//! To remove all objects from the container and {\bf to invoke the
//! destructor on these objects}. Goes through the container, invoking the
//! destructor on any object in the map. It then invokes clear() on
//! the map object to clear it.
void XC::MapOfTaggedObjects::clearAll(bool invokeDestructor)
  {
    // invoke the destructor on all the tagged objects stored
    if(invokeDestructor)
      clearComponents();
    // now clear the map of all entries
    theMap.clear();
    transmitIDs= true; //All component removed.
  }

//! @brief Print stuff.
void XC::MapOfTaggedObjects::Print(std::ostream &s, int flag)
  {
    // go through the array invoking Print on non-zero entries
    const_iterator p = begin();
    while(p!=end())
      {
	((*p).second)->Print(s, flag);
	p++;
      }
  }



