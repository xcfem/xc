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
//ModelComponentContainer.h

#ifndef ModelComponentContainer_H
#define ModelComponentContainer_H

#include "preprocessor/multi_block_topology/ModelComponentContainerBase.h"
#include <map>
#include "boost/python/list.hpp"

namespace XC {

//! @ingroup MultiBlockTopology
//
//! @brief Container for model entities.
template <class T>
class ModelComponentContainer: public ModelComponentContainerBase, public std::map<ModelComponentContainerBase::Indice,T *>
  {
  private:
    int sendObjects(Communicator &);
    int receiveObjects(const size_t &sz,const Communicator &);
  protected:
    virtual int sendData(Communicator &);
    virtual int recvData(const Communicator &);
  public:
    typedef typename std::map<Indice,T *> map_base;
    typedef typename std::pair<Indice,T *> pair;
    typedef typename map_base::iterator iterator;
    typedef typename map_base::const_iterator const_iterator;

    ModelComponentContainer(MultiBlockTopology *mbt= nullptr);

    T * busca(const Indice &);
    const T * busca(const Indice &) const;
    bool exists(const Indice &) const;
    T * get(const Indice &);
    boost::python::list getKeys(void) const;

    void clearAll(void);
    virtual ~ModelComponentContainer(void);
  };

//! @brief Constructor.
template <class T>
ModelComponentContainer<T>::ModelComponentContainer(MultiBlockTopology *mbt)
  : ModelComponentContainerBase(mbt) {}

//! @brief Return a pointer to the geometry entity whose identifier
//! is passed as parameter.
template <class T>
T * ModelComponentContainer<T>::busca(const Indice &id)
  {
    T * retval= nullptr;
    iterator i= this->find(id);
    if(i!= this->end()) //Entity exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Return a pointer to the entity whose identifier is passed as parameter.
template <class T>
const T * ModelComponentContainer<T>::busca(const Indice &id) const
  {
    const T * retval= nullptr;
    const_iterator i= this->find(id);
    if(i!= this->end()) //Entity exists.
      retval= (*i).second;
    return retval;
  }

//! @brief Return true if the entity already exists.
template <class T>
bool ModelComponentContainer<T>::exists(const Indice &id) const
  { return (busca(id)!=nullptr); }


//! @brief Return the entity with tag iEnt.
template <class T>
T * ModelComponentContainer<T>::get(const size_t &iEnt)
  {
    T *retval= busca(iEnt);
    if(!retval)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; entity: '" 
                << iEnt << "' not found.\n";
    return retval;   
  }

//! @brief Erase all the entities.
template <class T>
void ModelComponentContainer<T>::clearAll(void)
  {
    for(iterator i=this->begin();i!=this->end();i++)
      {
        T * tmp= (*i).second;
        if(tmp)
          delete tmp;
        tmp= nullptr;
      }
    this->clear();
  }

//! @brief Return the container's keys.
template <class T>
boost::python::list ModelComponentContainer<T>::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Destructor.
template <class T>
ModelComponentContainer<T>::~ModelComponentContainer(void)
  { clearAll(); }

//! @brief Send objects through the communicator argument.
template <class T>
int ModelComponentContainer<T>::sendObjects(Communicator &comm)
  {
    const int size= this->size();
    int res= 0;
    if(size>0)
      {
        for(const_iterator i=this->begin();i!=this->end();i++)
	  {
            T *ptr= (*i).second;
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            if(tmp)
              {
                tmp->setDbTag(comm);
                res= tmp->sendSelf(comm);
                if(res<0)
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
    		              << "; object failed in sendSelf.\n";
                    break;
                  }
              }
            else
    	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; the object is not movable." << std::endl;
	  }
      }
    return res;
  }
  
//! @brief Receive the objects through the communicator being
//! passed as parameter.
template <class T>
int XC::ModelComponentContainer<T>::receiveObjects(const size_t &sz, const Communicator &comm)
  {
    int res= 0;

    // We need to "broke" the object here just as they do
    // with TaggedObjectStorage.
    // static T t_object= T();
    // for(size_t i= 0;i<sz;i++)
    //   {
    // 	res+= t_object.recvSelf(comm);
    // 	T *ptr= dynamic_cast<T *>(t_object.getCopy());
    //     (*this)[t_object.getTag()]= ptr;
    //   }
    return res;
  }
  
//! @brief Send members through the communicator argument.
template <class T>
int ModelComponentContainer<T>::sendData(Communicator &comm)
  {
    int res= ModelComponentContainerBase::sendData(comm);
    const size_t sz= this->size();
    res+= comm.sendSzt(sz,getDbTagData(),CommMetaData(1));
    if(sz>0)
      { res+= sendObjects(comm); }
    return res;
  }

//! @brief Receive data through the communicator argument.
template <class T>
int ModelComponentContainer<T>::recvData(const Communicator &comm)
  {
    int res= ModelComponentContainerBase::recvData(comm);
    size_t sz= 0;
    res+= comm.receiveSzt(sz,getDbTagData(),CommMetaData(1));
    if(sz>0)
      { res+= receiveObjects(sz, comm); }
    return res;
  }

} //end of XC namespace
#endif
