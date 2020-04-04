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
//MovableContainer
                                                                        
                                                                        
#ifndef MovableContainer_h
#define MovableContainer_h

#include "utility/actor/actor/MovableObject.h"
#include <deque>

namespace XC {

//! @ingroup IPComm
//
//! @brief Container that can move between processes.
template <class C>
class MovableContainer: public MovableObject
  {
  public:
    typedef typename C::iterator iterator;
    typedef typename C::const_iterator const_iterator;
    typedef typename C::reference reference;
    typedef typename C::const_reference const_reference;
  protected:
    C &container;
    DbTagData &getDbTagData(void) const;
    virtual int sendItem(const_reference s,Communicator &,DbTagData &, const CommMetaData &)= 0;
    virtual int receiveItem(reference s,const Communicator &,DbTagData &, const CommMetaData &)= 0;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    explicit MovableContainer(C &);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };

//! @brief Constructor.
template <class C>
XC::MovableContainer<C>::MovableContainer(C &v)
  : MovableObject(0), container(v) {}


//! @brief Returns a vector to store the dbTags
//! of the class members.
template <class C>
XC::DbTagData &XC::MovableContainer<C>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send data through the communicator argument.
template <class C>
int XC::MovableContainer<C>::sendData(Communicator &comm)
  {
    const size_t sz= container.size();
    DbTagData &dt= getDbTagData();

    dt.setDbTagDataPos(0, sz);

    DbTagData dbTags(sz);

    int res= 0;
    int loc= 0;
    for(const_iterator i= container.begin();i!=container.end();i++)
      res+= this->sendItem(*i,comm,dbTags,CommMetaData(loc++));
    res+= dbTags.send(dt,comm,CommMetaData(1));
    if(res<0)
      std::cerr << "MovableContainer::sendSelf() - failed to send ID.\n";
    return res;
  }

//! @brief Receive data through the communicator argument.
template <class C>
int XC::MovableContainer<C>::recvData(const Communicator &comm)
  {
    DbTagData &dt= getDbTagData();
    const int sz= dt.getDbTagDataPos(0);
    container.resize(sz);
    DbTagData dbTags(sz);


    int res= dbTags.receive(dt,comm,CommMetaData(1));
    int loc= 0;
    for(iterator i= container.begin();i!=container.end();i++)
      res+= this->receiveItem(*i,comm,dbTags,CommMetaData(loc++));
    return res;
  }

//! @brief Sends container through the communicator argument.
template <class C>
int XC::MovableContainer<C>::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    this->inicComm(2);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "MovableContainer::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receive the container through the communicator argument.
template <class C>
int XC::MovableContainer<C>::recvSelf(const Communicator &comm)
  {
    this->inicComm(2);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "MovableContainer::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
          std::cerr << "MovableContainer::" << __FUNCTION__
	            << "; failed to receive data.\n";
      }
    return res;
  }


} // end of XC namespace

#endif

