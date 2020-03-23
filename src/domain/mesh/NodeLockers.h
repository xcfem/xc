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
//NodeLockers.h

#ifndef NODELOCKERS_H
#define NODELOCKERS_H

#include "xc_utils/src/kernel/CommandEntity.h"
#include "domain/load/pattern/NodeLocker.h"
#include <map>

namespace XC {

class Mesh;

//! @brief NodeLocker container.
class NodeLockers: public CommandEntity, public MovableObject
  {
    typedef std::map<std::string,NodeLocker *> map_node_lockers; //!< NodeLocker container.
    map_node_lockers node_lockers;
    int tag; //!< Default identifier.
    std::string code; //!< Pattern identifier for the new constraints.
  protected:
    friend class Mesh;
    NodeLocker *busca_node_locker(const std::string &);
    const NodeLocker *busca_node_locker(const std::string &) const;
    NodeLocker *newNodeLocker(const std::string &nmb);
    void borraNodeLocker(const std::string &nmb);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    NodeLockers(Mesh *owr);
    ~NodeLockers(void);
  public:
    const std::string getCurrentNodeLockerId(void) const;
    NodeLocker *getCurrentNodeLockerPtr(void);
    const NodeLocker *getCurrentNodeLockerPtr(void) const;
    void clearAll(void);
    Domain *getDomain(void);
    const Domain *getDomain(void) const;

    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    NodeLocker *buscaNodeLocker(const int &tag);
    const NodeLocker *buscaNodeLocker(const int &tag) const;

    const int &getTag(void) const
      { return tag; }

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif
