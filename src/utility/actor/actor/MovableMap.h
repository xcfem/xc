//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
template <class T>
XC::DbTagData &XC::MovableMap<T>::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
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

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
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

//! @brief Envia el objeto por el canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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

