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
//NodeLockers.cc

#include "NodeLockers.h"
#include "domain/domain/Domain.h"


//Constraints.
#include <domain/constraints/SFreedom_Constraint.h>



#include "utility/actor/actor/MovableString.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/ID.h"

//! @brief Default constructor.
XC::NodeLockers::NodeLockers(Mesh *owr)
  : EntCmd(owr), MovableObject(0),  tag(0), code("nil") {}

//! @brief Returns a pointer to the bloqueador cuyo nombre being passed as parameter.
XC::NodeLocker *XC::NodeLockers::busca_node_locker(const std::string &nmb)
  {
    NodeLocker *retval= nullptr;
    map_node_lockers::iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      retval= i->second;
    return retval;
  }

//! @brief Returns a pointer to the bloqueador cuyo nombre being passed as parameter.
const XC::NodeLocker *XC::NodeLockers::busca_node_locker(const std::string &nmb) const
  {
    const XC::NodeLocker *retval= nullptr;
    map_node_lockers::const_iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      retval= i->second;
    return retval;
  }

//! @brief Returns a pointer to the bloqueador cuyo tag being passed as parameter.
XC::NodeLocker *XC::NodeLockers::buscaNodeLocker(const int &tag)
  {
    NodeLocker *retval= nullptr;
    for(map_node_lockers::iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      if(i->second->getTag()==tag)
        {
          retval= i->second;
          break;
        }
    return retval;
  }

//! @brief Returns a pointer to the bloqueador cuyo tag being passed as parameter.
const XC::NodeLocker *XC::NodeLockers::buscaNodeLocker(const int &tag) const
  {
    NodeLocker *retval= nullptr;
    for(map_node_lockers::const_iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      if(i->second->getTag()==tag)
        {
          retval= i->second;
          break;
        }
    return retval;
  }

XC::NodeLocker *XC::NodeLockers::nuevoNodeLocker(const std::string &nmb)
  {
    NodeLocker *retval= nullptr;
    if(!nmb.empty())
      {
        retval= busca_node_locker(nmb);
        if(!retval)
          {
            retval= new NodeLocker(tag);
            if(retval)
              {
                node_lockers[nmb]= retval;
                tag++;
              }
            else
              std::cerr << "NodeLockers::nuevoNodeLocker; no se pudo crear: '"
                        << nmb << "'\n";
          }
        else
	  std::clog << "NodeLockers::nuevoNodeLocker; ya existe: '"
                    << nmb << "'\n";
      }
    else
      std::cerr << "NodeLockers::nuevoNodeLocker; se pasó un nombre vacío." << std::endl;
    return retval;
  }

void XC::NodeLockers::borraNodeLocker(const std::string &nmb)
  {
    map_node_lockers::iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      {
        delete (*i).second;
        node_lockers.erase(i);
      }
    else
      std::cerr << "NodeLockers::borraNodeLocker; no se encontró: '"
                << nmb << "'\n";
  }

void XC::NodeLockers::removeFromDomain(const std::string &cod)
  {
    NodeLocker *p= busca_node_locker(cod);
    if(p)
      getDomain()->removeNodeLocker(p);
    else
      {
        std::cerr << "removeFromDomain: no se encontró la hipótesis: " 
                  << cod << std::endl;
      }
  }

void XC::NodeLockers::removeAllFromDomain(void)
  {
    std::deque<std::string> nombres;
    for(map_node_lockers::iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      nombres.push_back((*i).first);
    for(std::deque<std::string>::const_iterator i= nombres.begin();i!=nombres.end();i++)
      removeFromDomain(*i);
  }

//! @brief Borra todos los objetos.
void XC::NodeLockers::clearAll(void)
  {
    removeAllFromDomain();
    for(map_node_lockers::iterator i= node_lockers.begin();i!= node_lockers.end();i++)
      delete (*i).second;
    node_lockers.erase(node_lockers.begin(),node_lockers.end());
  }

//! @brief Destructor.
XC::NodeLockers::~NodeLockers(void)
  { clearAll(); }

const std::string XC::NodeLockers::getCurrentNodeLockerId(void) const
  { return code; }

XC::NodeLocker *XC::NodeLockers::getCurrentNodeLockerPtr(void)
  { return busca_node_locker(code); }

const XC::NodeLocker *XC::NodeLockers::getCurrentNodeLockerPtr(void) const
  { return busca_node_locker(code); }

//! @brief Returns the domain.
XC::Domain *XC::NodeLockers::getDomain(void)
  {
    Domain *retval= nullptr;
    Mesh *mesh= dynamic_cast<Mesh *>(Owner());
    if(mesh)
      retval= mesh->getDomain();
    else
      std::cerr << "(NodeLockers) mesh needed." << std::endl;
    return retval;
  }

//! @brief Returns the domain.
const XC::Domain *XC::NodeLockers::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const Mesh *mesh= dynamic_cast<const Mesh *>(Owner());
    if(mesh)
      retval= mesh->getDomain();
    else
      std::cerr << "(NodeLockers) mesh needed." << std::endl;
    return retval;
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodeLockers::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::NodeLockers::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    const size_t sz= node_lockers.size();      
    setDbTagDataPos(1,sz);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        DbTagData objData(sz);
        int loc= 0;
        for(std::map<std::string,NodeLocker *>::iterator i=node_lockers.begin();i!=node_lockers.end();i++,loc++)
          {
            res+= cp.sendString((*i).first,labelData,CommMetaData(loc));
            res+= cp.sendMovable(*(*i).second,objData,CommMetaData(loc));
          }
        res+= labelData.send(getDbTagData(),cp,CommMetaData(2));
        res+= objData.send(getDbTagData(),cp,CommMetaData(3));
      }
    res+= cp.sendString(code,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::NodeLockers::recvData(const CommParameters &cp)
  {
    tag= getDbTagDataPos(0);
    const size_t sz= getDbTagDataPos(1);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        res+= labelData.receive(getDbTagData(),cp,CommMetaData(2));
        DbTagData objData(sz);
        res+= objData.receive(getDbTagData(),cp,CommMetaData(3));
        std::string label;
        NodeLocker *tmp= nullptr;
        for(size_t i= 0;i<sz;i++)
          {
            res+= cp.receiveString(label,labelData,CommMetaData(i));
            tmp= nuevoNodeLocker(label);
            if(tmp)
              res+= cp.receiveMovable(*tmp,objData,CommMetaData(i));
            else
	      std::cerr << "Error en NodeLockers::recvData label= " 
                        << label << std::endl;
          }
      }
    res+= cp.receiveString(code,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::NodeLockers::sendSelf(CommParameters &cp)
  {
    inicComm(5);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "NodeLockers::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::NodeLockers::recvSelf(const CommParameters &cp)
  {
    inicComm(5);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "NodeLockers::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }
