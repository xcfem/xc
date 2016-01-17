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
//TaggedObjectStorage.cc

#include "TaggedObjectStorage.h"
#include "utility/tagged/TaggedObject.h"
#include "TaggedObjectIter.h"
#include <boost/any.hpp>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/actor/actor/MovableID.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"

XC::ID XC::TaggedObjectStorage::dbTags;
XC::ID XC::TaggedObjectStorage::classTags;
XC::ID XC::TaggedObjectStorage::objTags;
const int XC::TaggedObjectStorage::posDbTag1;
const int XC::TaggedObjectStorage::posDbTag2;
const int XC::TaggedObjectStorage::posDbTag3;

//! @brief Constructor.
XC::TaggedObjectStorage::TaggedObjectStorage(EntCmd *owr,const std::string &contrName)
  : EntCmd(owr), MovableObject(0), containerName(contrName), transmitIDs(true) {}

//! @brief Copia en éste los componentes del contenedor que se pasa como parámetro.
void XC::TaggedObjectStorage::copia(const TaggedObjectStorage &otro)
  {
    TaggedObject *ptr= nullptr;
    TaggedObjectStorage &otro_no_const= const_cast<TaggedObjectStorage &>(otro);
    TaggedObjectIter &theIter= otro_no_const.getComponents();
    while((ptr= theIter()) != nullptr)
      addComponent(ptr->getCopy());
  }

//! @brief Devuelve verdadero si existe la componente
//! cuyo tag se pasa como parámetro.
bool XC::TaggedObjectStorage::existComponent(int tag)
  { return (getComponentPtr(tag)!=nullptr); }

//! @brief Devuelve los DBTags de los objetos.
const XC::ID &XC::TaggedObjectStorage::getDBTags(CommParameters &cp)
  {
    static ID retval;
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectIter &theIter= getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            retval(loc)= tmp->getDbTag(cp);
            loc++;
          }        
      }
    return retval;
  }

//! @brief Devuelve los classTags de los objetos.
const XC::ID &XC::TaggedObjectStorage::getClassTags(void) const
  {
    static ID retval;
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectStorage *this_no_const= const_cast<TaggedObjectStorage *>(this);
        TaggedObjectIter &theIter=  this_no_const->getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            retval(loc)= tmp->getClassTag();
            loc++;
          }        
      }
    return retval;
  }

//! @brief Devuelve los tags de los objetos.
const XC::ID &XC::TaggedObjectStorage::getObjTags(void) const
  {
    static ID retval;
    retval.Zero();
    const int size= getNumComponents();
    if(size>0)
      {
        retval.resize(size);
        TaggedObject *ptr;
        TaggedObjectStorage *this_no_const= const_cast<TaggedObjectStorage *>(this);
        TaggedObjectIter &theIter= this_no_const->getComponents();
        int loc =0;
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            retval(loc)= ptr->getTag();
            loc++;
          }
      }
    return retval;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::TaggedObjectStorage::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Devuelve los dbTags y classTags de los objetos a través del canal que se pasa como parámetro.
int XC::TaggedObjectStorage::sendObjectTags(CommParameters &cp)
  {
    const int size= getNumComponents();
    int res= 0;
    if(size>0)
      {
        dbTags= getDBTags(cp);
        res+= cp.sendID(dbTags,getDbTagData(),CommMetaData(posDbTag1));
        classTags= getClassTags();
        res+= cp.sendID(classTags,getDbTagData(),CommMetaData(posDbTag2));
        objTags= getObjTags();
        res+= cp.sendID(objTags,getDbTagData(),CommMetaData(posDbTag3));
      }
    if(res<0)
      std::cerr << "TaggedObjectStorage::sendObjectTags - channel failed to send the IDs.\n";
    return res;
  }

//! @brief Recibe los dbTags o classTags de los objetos a través del canal que se pasa como parámetro.
const XC::ID &XC::TaggedObjectStorage::receiveTags(int posDbTag,int sz,const CommParameters &cp)
  {
    static ID retVal;
    int res= 0;
    if(sz>0)
      {
        retVal.resize(sz);
        res= cp.receiveID(retVal,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "TaggedObjectStorage::receiveTags - channel failed to receive the IDs.\n";
    return retVal;
  }

//! @brief Recibe los tags para los objetos por el canal que se pasa como parámetro.
int XC::TaggedObjectStorage::recibeObjectTags(const int &sz,const CommParameters &cp)
  {
    int res= 0;
    dbTags= receiveTags(posDbTag1,sz,cp);
    classTags= receiveTags(posDbTag2,sz,cp);
    objTags= receiveTags(posDbTag3,sz,cp);
    return res;
  }

//! @brief Envía los objetos a través del canal que se pasa como parámetro.
int XC::TaggedObjectStorage::sendObjects(CommParameters &cp)
  {
    const int size= getNumComponents();
    int res= 0;
    if(size>0)
      {
        TaggedObject *ptr;
        TaggedObjectIter &theIter= getComponents();
        // loop over nodes in mesh adding their dbTag to the ID
        while((ptr= theIter()) != nullptr)
          {
            MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
            if(tmp)
              {
                tmp->setDbTag(cp);
                res= tmp->sendSelf(cp);
                if(res<0)
                  {
                    std::cerr << "TaggedObjectStorage::send - object with tag "
                              << ptr->getTag() << " failed in sendSelf\n";
                    break;
                  }
              }
            else
	      std::cerr << "El objeto de tag: "
                        << ptr->getTag() << " no es móvil." << std::endl;
          }
      }
    return res;
  }

//! @brief Recibe los objetos del contenedor a través del canal que se pasa como parámetro.
int XC::TaggedObjectStorage::receiveObjects(const CommParameters &cp)
  {
    TaggedObject *ptr= nullptr;
    TaggedObjectIter &theIter= getComponents();
    int res= 0;
    while((ptr= theIter()) != nullptr)
      {
        MovableObject *tmp= dynamic_cast<MovableObject *>(ptr);
        if(tmp)
          {
            res= tmp->recvSelf(cp);
            if(res<0)
              {
                std::cerr << "TaggedObjectStorage::receive - object with tag "
                          << ptr->getTag() << " failed.\n";
                break;
              }
          }
        else
	  std::cerr << "El objeto de tag: " << ptr->getTag() << " no es móvil." << std::endl;
      }
    return res;
  }



//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::TaggedObjectStorage::sendData(CommParameters &cp)
  {
    const int dbTag= getDbTag(cp);
    const int sz= getNumComponents();
    setDbTagDataPos(0,dbTag);
    setDbTagDataPos(1,sz);

    int res= 0;
    if(sz>0)
      {
        res+= sendObjects(cp);
        if(transmitIDs)
          {
            res+= sendObjectTags(cp);
            transmitIDs= false; //Ya se han enviado.
          }
      }
    return res;
  }

int XC::TaggedObjectStorage::sendSelf(CommParameters &cp)
  {
    inicComm(5);
    int res= sendData(cp);

    const int dbTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "TaggedObjectStorage::sendSelf() - failed to send data.\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::TaggedObjectStorage::recvSelf(const CommParameters &cp)
  { return receiveObjects(cp); }

