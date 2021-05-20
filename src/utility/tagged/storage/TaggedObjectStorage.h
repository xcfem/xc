// -*-c++-*-
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

// $Revision: 1.3 $
// $Date: 2005/11/23 22:37:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/storage/TaggedObjectStorage.h,v $


#ifndef TaggedObjectStorage_h
#define TaggedObjectStorage_h

// Written: fmk
// Revision: A
//
// Description: This file contains the class definition for
// TaggedObjectStorage. 
//
// What: "@(#) TaggedObjectStorage.h, revA"

#include "utility/kernel/CommandEntity.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class TaggedObject;
class TaggedObjectIter;

//! @ingroup Tagged
//
//! @brief TaggedObjectStorage. A TaggedObjectStorage object a container object
//! used to hold objects of type TaggedObject; each object of which has
//! some UNIQUE identifier.
//!
//! TaggedObjectStorage is used as a container object to store and
//! provide access to objects of type TaggedObject. Each TaggedObject
//! object stored in a TaggedObjectStorage object must have a unique
//! integer tag to distinguish it from other the other objects stored. The
//! TaggedObjectStorage class is an abstract base class, it just defines
//! the interface all concrete subclasses must provide. The interface
//! defines methods to add and to remove the components, and methods to
//! obtain access to the components.
class TaggedObjectStorage: public CommandEntity, public MovableObject
  {
    static ID dbTags;
    static ID classTags;
    static ID objTags;
    static const int posDbTag1= 2;
    static const int posDbTag2= 3;
    static const int posDbTag3= 4;
    const ID &getDBTags(Communicator &);
    int sendObjectTags(Communicator &);
    const ID &receiveTags(int posDbTag,int size,const Communicator &);
    int receiveObjectTags(const int &sz,const Communicator &);
    int sendObjects(Communicator &);
    template <class T>
    int createObjects(const Communicator &,T *(FEM_ObjectBroker::*p)(int));
    int receiveObjects(const Communicator &);
  protected:
    std::string containerName; //!< Container name.
    bool transmitIDs;
    DbTagData &getDbTagData(void) const;

    template <class T>
    int receiveData(const Communicator &,T *(FEM_ObjectBroker::*p)(int));
    void copy(const TaggedObjectStorage &);
  public:
    TaggedObjectStorage(CommandEntity *owr,const std::string &containerName);
    virtual ~TaggedObjectStorage(void) {}

    // public methods to populate the container
    //! @brief To provide an indication to the container object that \p newSize
    //! components are likely to be added. This is only a hint, it should be
    //! acceptable for more or less objects than \p newSize to be added to
    //! the container.
    virtual int setSize(int newSize) =0;
    //! @brief To add the object \p newComponent to the container. To return
    //! \p true if the object was added to the container, \p false
    //! otherwise. The object should not be added if another object with a
    //! similar tag already exists in the container.
    virtual bool addComponent(TaggedObject *newComponent) =0;
    //! @brief To remove the component whose tag is given by \p tag from the
    //! container. To return a pointer to the removed object if successful,
    //!\f$0\f$ if not.
    virtual bool removeComponent(int tag) =0;
    //! @brief Returns the current number of components.
    virtual int getNumComponents(void) const =0;

    bool existComponent(int tag);
    //! @brief To return a pointer to the TaggedObject whose identifier is
    //! given by \p tag. If the object has not been added to the container
    //! nullptr is to be returned.
    virtual TaggedObject *getComponentPtr(int tag) =0;
    //! @brief To return a pointer to the TaggedObject whose identifier is
    //! given by \p tag. If the object has not been added to the container
    //! nullptr is to be returned.
    virtual const TaggedObject *getComponentPtr(int tag) const=0;
    //! @brief To return an iter for iterating through the objects that have
    //! been added to the container.
    virtual TaggedObjectIter &getComponents(void) =0;

    //! To return an empty copy of the container.
    virtual TaggedObjectStorage *getEmptyCopy(void)=0;
    //! @brief To remove all objects from the container and {\bf to invoke the
    //! destructor on these objects} if \p invokeDestructor is \p true.
    virtual void clearAll(bool invokeDestructors = true) =0;

    const ID &getClassTags(void) const;
    const ID &getObjTags(void) const;
    bool getTransmitIDsFlag(void) const
      { return transmitIDs; }
    virtual int sendData(Communicator &);
    template <class T>
    int receive(int dbTag,const Communicator &,T *(FEM_ObjectBroker::*p)(int));
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    //! Invoke {\em Print(s,flag)} on all objects which have been added to
    //! the container. 
    virtual void Print(std::ostream &s, int flag =0) const=0;
  };

template <class T>
int TaggedObjectStorage::createObjects(const Communicator &comm,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    int retval= 0;
    const int size= dbTags.Size();
    if(size != 0)
      {
        // now for each node we 1) get a new node of the correct type from the ObjectBroker
        // 2) ensure the node exists and set it's dbTag, 3) we invoke recvSelf on this new
        // blank node and 4) add this node to the mesh
        int loc = 0;
        for(int i=0; i<size; i++)
          {
            T *ptr= nullptr;
            ptr= comm.getBrokedTagged(ptr,dbTags(loc),objTags(loc),classTags(loc),ptrFunc);
            if(ptr)
              {
                if(!addComponent(ptr))
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; could not add node with tag: "
                              << ptr->getTag() << " into mesh\n!";
                    retval= -3;
                    break;
                  }
              }
            else
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; cannot create node with classTag: " 
                          << classTags(loc) << std::endl;
                retval= -2;
                break;
              }
            loc++;
          }
      }
    return retval;
  }

//! @brief Receives members through the communicator argument.
template <class T>
int TaggedObjectStorage::receiveData(const Communicator &comm,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    setDbTag(getDbTagDataPos(0));
    int sz= getDbTagDataPos(1);
    int res= 0;
    if(sz>0)
      {
        if(transmitIDs)
          {
            clearAll();
            receiveObjectTags(sz,comm);
            res+= this->createObjects(comm,ptrFunc);
          }
        res+= this->receiveObjects(comm);
      }
    return res;
  }

//! @brief Receives members through the communicator argument.
template <class T>
int TaggedObjectStorage::receive(int dbTag,const Communicator &comm,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    inicComm(5);
    setDbTag(dbTag);
    int res= comm.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to recv the initial ID\n";
    else
      res+= TaggedObjectStorage::receiveData(comm,ptrFunc);
    return res;
  }

} // end of XC namespace

#endif


