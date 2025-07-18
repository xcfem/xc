// -*-c++-*-
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
//MovableSTDVector
                                                                        
                                                                        
#ifndef MovableSTDVector_h
#define MovableSTDVector_h

#include "MovablePointerContainer.h"
#include "MovableID.h"
#include <vector>
#include "utility/tagged/TaggedObject.h"
#include "Communicator.h"

namespace XC {

//! @ingroup IPComm
//
//! @brief Template class for std::vectors that can move between processes.
template <class T>
class MovableSTDVector: public MovablePointerContainer<T>
  {
  protected:
    typedef std::vector<T *> vector_objects;
    typedef typename vector_objects::const_iterator const_iterator;
    typedef typename vector_objects::iterator iterator;
    vector_objects objects;

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MovableSTDVector(const vector_objects &deque,T *(FEM_ObjectBroker::*pF)(int));
    const vector_objects &getVector(void) const
      { return objects; }

   };


//! @brief Constructor.
template <class T>
XC::MovableSTDVector<T>::MovableSTDVector(const vector_objects &deque,T *(FEM_ObjectBroker::*pF)(int))
  : MovablePointerContainer<T>(0,pF), objects(deque) {}

//! @brief Send members through the communicator argument.
template <class T>
int XC::MovableSTDVector<T>::sendData(Communicator &comm)
  {
    const size_t sz= objects.size();
    this->setDbTagDataPos(0,sz);
    int res= 0;
    if(sz>0)
      {
	DbTagData dbTags(sz);
	ID classTags(sz);
	for(size_t i= 0;i<sz;i++)
	  {
	    T *obj= objects[i];
	    if(obj)
	      {
	        classTags(i)= obj->getClassTag();
	        res+= comm.sendMovable(*obj,dbTags,CommMetaData(i));
	      }
	    else
	      std::cerr << "MovableSTDVector::sendData; found null"
		        << " pointer in position: " << i << std::endl;
	  }
	DbTagData &dbTagData= this->getDbTagData();
        res+= dbTags.send(dbTagData,comm,CommMetaData(1));
        res+= comm.sendID(classTags,dbTagData,CommMetaData(2));
      }
    return res;
  }

//! @brief Receives members through the communicator argument.
template <class T>
int MovableSTDVector<T>::recvData(const Communicator &comm)
  {
    const size_t sz= this->getDbTagDataPos(0);
    int res= 0;
    if(sz>0)
      {
	DbTagData &dbTagData= this->getDbTagData();
        DbTagData dbTags(sz);
        res+= dbTags.receive(dbTagData,comm,CommMetaData(1));
        ID classTags(sz);
        res+= comm.receiveID(classTags,dbTagData,CommMetaData(2));
        T *tmp= nullptr;
	for(size_t i= 0;i<sz;i++)
	  {
            const int dbTag= dbTags.getDbTagDataPos(i);
            tmp= this->getBrokedObject(dbTag,classTags(i),comm);
            if(tmp)
              {
		 res+= tmp->recvSelf(comm);
                 objects[i]= tmp;
              }
            else
	      std::cerr << "Error in MovableSTDVector::recvData i= " 
                        << i << std::endl;
	  }
      }
    return res;
  }


template <class T>
int sendSTDVector(const std::vector<T *> &m,Communicator &comm,DbTagData &dt,const CommMetaData &meta)
  {
    MovableSTDVector<T> mm(m,nullptr);
    return comm.sendMovable(mm,dt,meta);
  }

template <class T>
int receiveSTDVector(std::vector<T *> &v,const Communicator &comm,DbTagData &dt,const CommMetaData &meta,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    MovableSTDVector<T> mm(v,ptrFunc);
    int res= comm.receiveMovable(mm,dt,meta);
    v= mm.getVector();
    return res;
  } 

} // end of XC namespace

#endif

