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
//MovableMap
                                                                        
                                                                        
#ifndef MovableMap_h
#define MovableMap_h

#include "MovablePointerContainer.h"
#include "MovableID.h"
#include "MovableString.h"
#include <string>
#include <map>
#include "utility/tagged/TaggedObject.h"
#include "CommParameters.h"

namespace XC {

//! @ingroup IPComm
//
//! @brief Template class for maps that can move between processes.
template <class T>
class MovableMap: public MovablePointerContainer<T>
  {
  protected:
    typedef std::map<std::string,T *> map_objects;
    typedef typename map_objects::iterator iterator;
    map_objects objects;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    MovableMap(const map_objects &map,T *(FEM_ObjectBroker::*pF)(int));
    const map_objects &getMap(void) const
      { return objects; }

  };


//! @brief Constructor.
template <class T>
MovableMap<T>::MovableMap(const map_objects &map,T *(FEM_ObjectBroker::*pF)(int))
  : MovablePointerContainer<T>(0,pF), objects(map) {}

//! @brief Send members through the channel being passed as parameter.
template <class T>
int XC::MovableMap<T>::sendData(CommParameters &cp)
  {
    const size_t sz= objects.size();
    this->setDbTagDataPos(0,sz);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        DbTagData dbTags(sz);
        ID classTags(sz);
        int loc= 0;
        for(iterator i=objects.begin();i!=objects.end();i++,loc++)
          {
            res+= cp.sendString((*i).first,labelData,CommMetaData(loc));
            classTags(loc)= (*i).second->getClassTag();
            res+= cp.sendMovable(*(*i).second,dbTags,CommMetaData(loc));
          }
	DbTagData &dbTagData= this->getDbTagData();
        res+= labelData.send(dbTagData,cp,CommMetaData(1));
        res+= dbTags.send(dbTagData,cp,CommMetaData(2));
        res+= cp.sendID(classTags,dbTagData,CommMetaData(3));
      }
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
template <class T>
int MovableMap<T>::recvData(const CommParameters &cp)
  {
    const size_t sz= this->getDbTagDataPos(0);
    int res= 0;
    if(sz>0)
      {
	DbTagData &dbTagData= this->getDbTagData();
        DbTagData labelData(sz);
        int res= labelData.receive(dbTagData,cp,CommMetaData(1));
        DbTagData dbTags(sz);
        res+= dbTags.receive(dbTagData,cp,CommMetaData(2));
        ID classTags(sz);
        res+= cp.receiveID(classTags,dbTagData,CommMetaData(3));
        std::string label;
        T *tmp= nullptr;
        for(size_t i= 0;i<sz;i++)
          {
            res+= cp.receiveString(label,labelData,CommMetaData(i));
            const int dbTag= dbTags.getDbTagDataPos(i);
            tmp= this->getBrokedObject(dbTag,classTags(i),cp);
            if(tmp)
              {
		 res+= tmp->recvSelf(cp);
                 objects[label]= tmp;
              }
            else
	      std::cerr << "Error in MovableMap::recvData label= " 
                        << label << std::endl;
          }
      }
    return res;
  }

template <class T>
int sendMap(const std::map<std::string,T *> &m,CommParameters &cp,DbTagData &dt,const CommMetaData &meta)
  {
    MovableMap<T> mm(m,nullptr);
    return cp.sendMovable(mm,dt,meta);
  }

template <class T>
int receiveMap(std::map<std::string,T *> &v,const CommParameters &cp,DbTagData &dt,const CommMetaData &meta,T *(FEM_ObjectBroker::*ptrFunc)(int))
  {
    MovableMap<T> mm(v,ptrFunc);
    int res= cp.receiveMovable(mm,dt,meta);
    v= mm.getMap();
    return res;
  } 

} // end of XC namespace

#endif

