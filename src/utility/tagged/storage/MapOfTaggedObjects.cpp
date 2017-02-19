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

XC::MapOfTaggedObjects::MapOfTaggedObjects(EntCmd *owr,const std::string &containerName)
  : TaggedObjectStorage(owr,containerName), myIter(*this) {}

//! @brief Copy constructor.
XC::MapOfTaggedObjects::MapOfTaggedObjects(const MapOfTaggedObjects &otro)
  : TaggedObjectStorage(otro), myIter(*this)
  {
    copia(otro);
  }

//! @brief Assignment operator.
XC::MapOfTaggedObjects &XC::MapOfTaggedObjects::operator=(const MapOfTaggedObjects &otro)
  {
    TaggedObjectStorage::operator=(otro);
    copia(otro);
    return *this;
  }

XC::MapOfTaggedObjects::~MapOfTaggedObjects(void)
  { clearComponents(); }


int XC::MapOfTaggedObjects::setSize(int newSize)
  {
    // no setSize for map template .. can only check enough space available
    int maxSize = theMap.max_size();
    if(newSize > maxSize)
      {
        std::cerr << "XC::MapOfTaggedObjects::setSize - failed as map stl has a max size of " << maxSize << "\n";
        return -1;
      } 
   
    return 0;
  }

//! @brief Agrega un componente al contenedor.
bool XC::MapOfTaggedObjects::addComponent(TaggedObject *newComponent)
  {
    int tag = newComponent->getTag();

    // check if the ele already in map, if not we add
    iterator theEle = theMap.find(tag);
    if(theEle==end())
      {
        newComponent->set_owner(this);
	theMap.insert(value_type(tag,newComponent));
        transmitIDs= true; //Se agrega un componente.
		      
	// check if sucessfully added 
	theEle = theMap.find(tag);
	if(theEle == theMap.end())
         {
	   std::cerr << "MapOfTaggedObjects::addComponent - map STL failed to add object with tag : " << 
	     newComponent->getTag() << "\n";
	   return false;
	 }
      }
    // if ele already there map cannot add even if allowMultiple is true
    // as the map template does not allow multiple entries wih the same tag
    else
      {	
        std::cerr << "MapOfTaggedObjects::addComponent - not adding as one with similar tag exists, tag: " <<
	  newComponent->getTag() << "\n";
        return false;
      }
    return true;  // o.k.
  }

//! @brief Agrega un componente al contenedor.
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
        transmitIDs= true; //Se elimina un componente.
	if(ok != 1)
          { // ensure the map did remove the object
	    std::cerr << "MapOfTaggedObjects::removeComponent - map STL failed to remove object with tag " << 
	      tag << "\n";
	  }
      }
    return retval;
  }


int XC::MapOfTaggedObjects::getNumComponents(void) const
  { return theMap.size(); }


XC::TaggedObject *XC::MapOfTaggedObjects::getComponentPtr(int tag)
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


XC::MapOfTaggedObjectsIter XC::MapOfTaggedObjects::getIter(void)
  { return XC::MapOfTaggedObjectsIter(*this); }


XC::TaggedObjectStorage *XC::MapOfTaggedObjects::getEmptyCopy(void)
  {
    MapOfTaggedObjects *theCopy = new MapOfTaggedObjects(Owner(),containerName);  
    if(!theCopy)
      std::cerr << "XC::MapOfTaggedObjects::getEmptyCopy-out of memory\n";
    return theCopy;
  }

//! @brief Libera la memoria ocupada por los componentes.
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

void XC::MapOfTaggedObjects::clearAll(bool invokeDestructor)
  {
    // invoke the destructor on all the tagged objects stored
    if(invokeDestructor)
      clearComponents();
    // now clear the map of all entries
    theMap.clear();
    transmitIDs= true; //Se eliminan todos los componentes.
  }

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



