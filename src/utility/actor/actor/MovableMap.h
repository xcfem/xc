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

#include "MovableObject.h"
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
class MovableMap: public MovableObject
  {
  protected:
    typedef std::map<std::string,T *> map_objects;
    typedef typename map_objects::iterator iterator;
    map_objects objects;
    T *(FEM_ObjectBroker::*ptrFunc)(int);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    MovableMap(const map_objects &map,T *(FEM_ObjectBroker::*pF)(int));
    const map_objects &getMap(void) const
      { return objects; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };


//! @brief Constructor.
template <class T>
XC::MovableMap<T>::MovableMap(const map_objects &map,T *(FEM_ObjectBroker::*pF)(int))
  :MovableObject(0), objects(map), ptrFunc(pF) {}

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
template <class T>
XC::DbTagData &XC::MovableMap<T>::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
template <class T>
int XC::MovableMap<T>::sendData(CommParameters &cp)
  {
    const size_t sz= objects.size();
    setDbTagDataPos(0,sz);
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
        res+= labelData.send(getDbTagData(),cp,CommMetaData(1));
        res+= dbTags.send(getDbTagData(),cp,CommMetaData(2));
        res+= cp.sendID(classTags,getDbTagData(),CommMetaData(3));
      }
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
template <class T>
int MovableMap<T>::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        int res= labelData.receive(getDbTagData(),cp,CommMetaData(1));
        DbTagData dbTags(sz);
        res+= dbTags.receive(getDbTagData(),cp,CommMetaData(2));
        ID classTags(sz);
        res+= cp.receiveID(classTags,getDbTagData(),CommMetaData(3));
        std::string label;
        T *tmp= NULL;
        for(size_t i= 0;i<sz;i++)
          {
            res+= cp.receiveString(label,labelData,CommMetaData(i));
            const int dbTag= dbTags.getDbTagDataPos(i);
            tmp= getBrokedMovable(dbTag,classTags(i),cp,ptrFunc);
            if(tmp)
              {
		 res+= tmp->recvSelf(cp);
                 objects[label]= tmp;
              }
            else
	      std::cerr << "Error en MovableMap::recvData label= " 
                        << label << std::endl;
          }
      }
    return res;
  }

//! @brief Envia el objeto through the channel being passed as parameter.
template <class T>
int XC::MovableMap<T>::sendSelf(CommParameters &cp)
  {
    inicComm(5);
    int result= sendData(cp);

    const int dataTag= getDbTag(cp);
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "MovableMap::sendSelf() - failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
template <class T>
int MovableMap<T>::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
    const int dataTag = getDbTag();  
    int result= cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "MovableMap::recvSelf() - failed to receive data\n";
    else
      result+= recvData(cp);
    return result;
  }

template <class T>
int sendMap(const std::map<std::string,T *> &m,CommParameters &cp,DbTagData &dt,const CommMetaData &meta)
  {
    MovableMap<T> mm(m,NULL);
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

