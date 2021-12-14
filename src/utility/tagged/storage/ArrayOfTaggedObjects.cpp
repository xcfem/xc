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

// $Revision: 1.2 $
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/ArrayOfTaggedObjects.cpp,v $


// File: ~/utility/tagged/storage/ArrayOfTaggedObjects.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of the XC::ArrayOfTaggedObjects
// class.
//
// What: "@(#) ArrayOfTaggedObjects.C, revA"

#include <utility/tagged/TaggedObject.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>



//! @brief Free components memory.
void XC::ArrayOfTaggedObjects::clearComponents(void)
  {
    const size_t sz= theComponents.size();
    for(size_t i=0; i<sz; i++)
      {
        if(theComponents[i])
          {
            delete theComponents[i];
            theComponents[i]= nullptr;
          }
      }
    theComponents.clear();
    positionLastEntry= 0;
    sizeComponentArray= 0; //Error if not coherent with theComponents.size()
  }

//! @brief Constructor.
//!
//! Creates an array of size \p sizeInitialArray to hold pointers to
//! the TaggedObject objects that will be added to the container. This
//! array is created using new() to allow the array to grow as
//! needed. Zeros this array and sets a number of integer values: (1) the
//! size of the array; (2) the last position used in the array is \f$0\f$; (3)
//! the position in the array through which all previous cells are being
//! used for pointers is \f$0\f$; (4) the number of components added to the
//! container is \f$0\f$; and (5) all components have been added at the
//! position given by their tag. If not enough space is available, a
//! warning is raised and the integer outlining the array size is set
//! to \f$0\f$. 
//!
//! @param owr: object owner (this object is somewhat contained by).
XC::ArrayOfTaggedObjects::ArrayOfTaggedObjects(CommandEntity *owr,int size,const std::string &containerName)
  :TaggedObjectStorage(owr,containerName), numComponents(0),sizeComponentArray(0), positionLastEntry(0),
   positionLastNoFitEntry(0),fitFlag(true), myIter(*this)
  {
    // get the components array from the heap
    theComponents= tagged_vector(size,static_cast<TaggedObject *>(nullptr));
    sizeComponentArray= size;

  }

//! @brief Copy constructor.
XC::ArrayOfTaggedObjects::ArrayOfTaggedObjects(const ArrayOfTaggedObjects &other)
  : TaggedObjectStorage(other), numComponents(other.numComponents),sizeComponentArray(other.sizeComponentArray), positionLastEntry(other.positionLastEntry), positionLastNoFitEntry(other.positionLastNoFitEntry),fitFlag(other.fitFlag), theComponents(other.theComponents.size(),nullptr), myIter(*this)
  { copy(other); }

//! @brief Assignment operator.
XC::ArrayOfTaggedObjects &XC::ArrayOfTaggedObjects::operator=(const ArrayOfTaggedObjects &other)
  {
    TaggedObjectStorage::operator=(other);
    numComponents= other.numComponents;
    sizeComponentArray= other.sizeComponentArray;
    positionLastEntry= other.positionLastEntry;
    positionLastNoFitEntry= other.positionLastNoFitEntry;
    fitFlag= other.fitFlag;
    clearComponents();
    theComponents= tagged_vector(other.theComponents.size(),nullptr);
    copy(other);
    return *this;
  }

//! @brief Destructor. Release memory.
XC::ArrayOfTaggedObjects::~ArrayOfTaggedObjects(void)
  { clearComponents(); }


//! @brief Set the size of the container.
//!
//! If \p newSize is valid, i.e. \f$>= 1\f$ AND \p newSize \f$>\f$ current
//! size of the array, the object allocates memory for a new array of size
//! \p newSize. It zeros this array and copies the old components to
//! this array, trying to see if they can be located at the positions
//! given by their tags. If all placed at ideal location last time and new
//! size \f$>=\f$ position of last entry straight copy, otherwise we reset and
//! add each again using addComponent(). Finally invokes the
//! destructor on the old array. Returns \f$0\f$ if successful. If not
//! successful, a warning is raised and a negative value is returned. 
int XC::ArrayOfTaggedObjects::setSize(int newSize)
  {
    // check a valid size
    if(newSize < 0 && newSize > sizeComponentArray) //?? LCPT 10-2006.
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; invalid size " << newSize << std::endl;
        return -1;
      }

    if(newSize < 2) newSize= 2; // make 2 the min size;


    // store the needed info of the old array
    theComponents.resize(newSize,nullptr);
    sizeComponentArray= theComponents.size();
    assert(newSize==sizeComponentArray);

    return 0;
  }

//! @brief Adds a component to the container.
//!
//! To add the object \p newComponent to the container. First checks to
//! see that an object with a similar tag does not already exist in the
//! container, invokes a warning is raised and
//! returns \p false if one does. It then checks to ensure that array
//! is large enough to hold another pointer, if not setSize() is
//! invoked with twice the size of the current array. (If setSize()
//! fails the warning method is invoked and \p false is returned: NOTE
//! other objects previously added may now be missing!!. The object is
//! then placed into the array by choosing the first of the following that
//! is successful: \begin{enumerate} 
//! \item If the array is large enough, the location given by the objects
//! tag is first tested to see if being used. If not this location is
//! used.
//! \item If it won't go in nicely, we find the first location in the
//! array that is not being used and use this location. We keep a marker
//! to this location for subsequent adds so that don't start at \f$0\f$
//! location all the time.
//! \end{enumerate}
//! \noindent Finally the integer indicating the numbers of objects in the
//! array is incremented and \p true is returned.
bool XC::ArrayOfTaggedObjects::addComponent(TaggedObject *newComponent)
  {
    // check to see that no other component already exists
    TaggedObject *other= getComponentPtr(newComponent->getTag());
    if(other)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - componentwith tag already exists, "
		  << " not adding component with tag: "
		  << newComponent->getTag() << std::endl;
        return false;
      }

    // check to see if size of current array is big enough. if not resize.
    if(numComponents == sizeComponentArray)
      if(this->setSize(2*numComponents) < 0)
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to enlarge the array with size: " 
                    << 2*numComponents << std::endl;
          return false;
        }

    // we try to the Component in nicely, i.e. in
    // position given by the newComponents Tag

    int newComponentTag= newComponent->getTag();
    newComponent->set_owner(this);

    if((newComponentTag >= 0) && (newComponentTag < sizeComponentArray))
      {
        if(theComponents[newComponentTag]==nullptr)
          { // it will go in nicely
            theComponents[newComponentTag]= newComponent;
            numComponents++;
            if(newComponentTag > positionLastEntry)
              positionLastEntry= newComponentTag;
            return true;
          }
      }

    // it won't go in nicely, so put wherever we can get it in
    while(theComponents[positionLastNoFitEntry] != 0 && positionLastNoFitEntry < sizeComponentArray )
      positionLastNoFitEntry++;

    // just in case we don't get a location ..  though we should!!
    if(positionLastNoFitEntry == sizeComponentArray)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not - find a vacant spot after enlarging!!\n";
        return false;
      }
    theComponents[positionLastNoFitEntry]= newComponent;
    numComponents++;
    if(positionLastNoFitEntry > positionLastEntry)
      positionLastEntry= positionLastNoFitEntry;
    fitFlag= false;
    transmitIDs= true; //Component added.
    return true;  // o.k.
  }

//! @brief Remove the component with the given tag.
//!
//! To remove the component whose tag is given by \p tag from the
//! container and return a pointer to the object. If tag is not too large
//! and all components have been added nicely, the contents of the array
//! at location \p tag is set to \f$0\f$ and its old contents
//! returned. Otherwise if the tag is not too large, the contents at
//! position \p tag is first checked to see if it is pointing to an
//! object and if this object has the same tag as \p tag, if it does
//! the contents of the array is set to \f$0\f$ and the object returned. If
//! the object is not at it's nice location, the array is searched from
//! the start to the position holding the last entry to see if the array
//! points to the object with the appropriate tag. If it 
//! does the array location is set to \f$0\f$ and the object returned,
//! otherwise \f$0\f$ is returned. If the object has not been found after the
//! last possible location has been checked, \f$0\f$ is returned.
bool XC::ArrayOfTaggedObjects::removeComponent(int tag)
  {
    bool retval= false;

    // first check to see if object is located at pos given by tag
    if((tag >= 0) && (tag < sizeComponentArray))
      {
        if(fitFlag == true)// if all objects in nicely then has to be at location given by tag
          {
            // if the object is there decrement numComponents added and
            // check if need to reset positionLastEntry marker
            if(theComponents[tag])
              {
                delete theComponents[tag];
                theComponents[tag]= nullptr;
                retval= true;                
                numComponents--;
                if(positionLastEntry == tag)
                  {
                    for(int i= positionLastEntry; i>=0; i--)
                      if(theComponents[i])
                        {
                          positionLastEntry= i;
                          i= -1; // break out of loop
                        }
                  }
              }
          }
        else  // it still may be at nice location so do as above
          if(theComponents[tag])
            if((theComponents[tag]->getTag()) == tag)
              {
                delete theComponents[tag];
                theComponents[tag]= nullptr;
                if(positionLastEntry == tag)
                  {
                    for(int i= positionLastEntry;i>=0;i--)
                      if(theComponents[i])
                        {
                          positionLastEntry= i;
                          i= -1; // break out of loop
                        }
                  }
                positionLastNoFitEntry= 0;
                retval= true;
                numComponents--;
              }
      }
    else
      {
        // else we have to look through array until we find it or
        // reach lastPosition used
        for(int i=0; i<=positionLastEntry; i++)
          if(theComponents[i])
            if(theComponents[i]->getTag() == tag)
              {
                // we found the component so do as above again
                // and then return a pointer to the component
                delete theComponents[i];
                theComponents[i]= nullptr;
                if(positionLastEntry == i)
                  {
                    for(int j= positionLastEntry;j>=0;j--)
                      if(theComponents[j])
                        {
                          positionLastEntry= j;
                          j= -1; // break out of loop
                        }
                  }
                positionLastNoFitEntry= 0;
                retval=true;
                numComponents--;
              }
      }
    transmitIDs= retval; //Removes a component.
    return retval;
  }

//! @brief Returns the number of components currently stored in the container.
int XC::ArrayOfTaggedObjects::getNumComponents(void) const
  { return numComponents; }

//! @brief Returns a pointer to the object with the given tag.
//!
//! To return a pointer to the TaggedObject whose identifier is given by
//! \p tag. If tag is not too large and all components have been added nicely,
//! the contents of the array at location \p tag is returned. Otherwise if the
//! tag is not too large, the contents at position \p tag is first checked to
//! see if it is pointing to an object and if this object has the same tag as
//! {\em tag}, the object returned. If the object is not at it's nice location,
//! the array is searched from the start to the position holding the last entry
//! to see if the array points to the object with the appropriate tag. If it
//! does the object is returned. If the object has not been found after the
//! last possible location has been checked, \f$0\f$ is returned.
XC::TaggedObject *XC::ArrayOfTaggedObjects::getComponentPtr(int tag)
  {
    const ArrayOfTaggedObjects *cthis= static_cast<const ArrayOfTaggedObjects *>(this);
    return const_cast<TaggedObject *>(cthis->getComponentPtr(tag));
  }

//! @brief Returns a pointer to the object identified by the
//! index being passed as parameter. Const version of the method.
const XC::TaggedObject *XC::ArrayOfTaggedObjects::getComponentPtr(int tag) const
  {
    // first check it's not where we would like it
    if((tag >= 0) && (tag < sizeComponentArray))
      {
        if(fitFlag == true) // either its at nice position or not entered
          return theComponents[tag];
        else
          {                    // it still may be at nice location
            if(theComponents[tag])
              if((theComponents[tag]->getTag()) == tag)
                return theComponents[tag];
          }
      }
    // else we have to look through array until we find it or
    // reach lastPosition used
    for(int i=0; i<=positionLastEntry; i++)
      if(theComponents[i] != 0)
        if(theComponents[i]->getTag() == tag)
          return theComponents[i];
    // its not in the array
    return 0;
  }

//! @brief Reset the iter to point to first component and then return a reference
//! to the iter.
//!
//! To return an iter for iterating through the objects that have been
//! added to the container. Each container object has its own iter. This
//! iter() is reset to point to the start and a reference to this iter is
//! returned. 
XC::TaggedObjectIter &XC::ArrayOfTaggedObjects::getComponents(void)
  {
    myIter.reset();
    return myIter;
  }

//! @brief Return a new iterator to the components.
XC::ArrayOfTaggedObjectsIter XC::ArrayOfTaggedObjects::getIter(void)
  {
    // return a new iter to the components, more expensive to use
    // this iter as two memory calls to heap .. needed if user needs
    // to have multiple iters running in same code segment!
    return ArrayOfTaggedObjectsIter(*this);
  }


//! @brief Return an empty copy of the container.
//!
//! To return an empty copy of the container. Creates a new
//! ArrayOfTaggedObjects object using the current size of the array as the
//! argument for the constructor. It is up to the user of this method to
//! invoke the destructor on the new object.
XC::TaggedObjectStorage *XC::ArrayOfTaggedObjects::getEmptyCopy(void)
  {
    ArrayOfTaggedObjects *theCopy= new ArrayOfTaggedObjects(Owner(), sizeComponentArray, containerName);
    if(!theCopy)
      std::cerr << getClassName() << "::" << __FUNCTION__
 	        << "; out of memory\n";
    return theCopy;
  }

//! @brief Free memory.
//!
//! //! To remove all objects from the container and {\bf to invoke the
//! destructor on these objects}. Goes through the array, invoking the
//! destructor on any object pointed to by a cell and then setting this
//! cell to \f$0\f$. Resets the internal member data to indicate that zero
//! components have been added to the container.
void XC::ArrayOfTaggedObjects::clearAll(bool invokeDestructors)
  {
//XXX Check if this can produce a memory leak (with invokeDestructors= false).
    transmitIDs= true;
    if(numComponents>0)
      {
        if(invokeDestructors)
          { clearComponents(); }
        else
          {
            //just set the array pointers to 0
            const size_t sz= theComponents.size();
            for(size_t i=0; i<sz; i++)
              {
                if(theComponents[i])
                  { theComponents[i]= nullptr; }
              }
          }
        positionLastEntry= 0;
        positionLastNoFitEntry= 0;
        fitFlag= true;
        numComponents= 0;
      }
  }

//! @brief Method which invokes Print on all components
void XC::ArrayOfTaggedObjects::Print(std::ostream &s, int flag) const
  {
    // go through the array invoking Print on non-zero entries
    for(int i=0; i<=positionLastEntry; i++)
      if(theComponents[i] != 0)
        theComponents[i]->Print(s, flag);
  }


