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
//MovablePointerContainer
                                                                        
                                                                        
#ifndef MovablePointerContainer_h
#define MovablePointerContainer_h

#include "MovableObject.h"
#include "Communicator.h"

namespace XC {

//! @ingroup IPComm
//
//! @brief Template class for maps that can move between processes.
template <class T>
class MovablePointerContainer: public MovableObject
  {
  protected:
    T *(FEM_ObjectBroker::*ptrFunc)(int);

    DbTagData &getDbTagData(void) const;
    T *getBrokedObject(const int &,const int &,const Communicator &);
    virtual int sendData(Communicator &)= 0;
    virtual int recvData(const Communicator &)= 0;
  public:
    MovablePointerContainer(const int &, T *(FEM_ObjectBroker::*pF)(int));
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };


//! @brief Constructor.
template <class T>
XC::MovablePointerContainer<T>::MovablePointerContainer(const int &classTag,T *(FEM_ObjectBroker::*pF)(int))
  :MovableObject(classTag), ptrFunc(pF) {}

//! @brief Returns a vector to store the dbTags
//! of the class members.
template <class T>
XC::DbTagData &XC::MovablePointerContainer<T>::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Returns an empty object of the class identified by classTag.
template <class T>
T *XC::MovablePointerContainer<T>::getBrokedObject(const int &dbTag,const int &classTag,const Communicator &comm)
  { return getBrokedMovable(dbTag,classTag,comm,this->ptrFunc); }
 
//! @brief Send the object through the channel being passed as parameter.
template <class T>
int XC::MovablePointerContainer<T>::sendSelf(Communicator &comm)
  {
    inicComm(5);
    int result= this->sendData(comm);

    const int dataTag= getDbTag(comm);
    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "MovablePointerContainer::sendSelf() - failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
template <class T>
int MovablePointerContainer<T>::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag = getDbTag();  
    int result= comm.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "MovablePointerContainer::recvSelf() - failed to receive data\n";
    else
      result+= recvData(comm);
    return result;
  }

} // end of XC namespace

#endif

