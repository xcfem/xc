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
//MovableID.cc

#include "MovableID.h"

#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableID::MovableID(const size_t &sz)
  :ID(sz),MovableObject(0) {}

//! @brief Constructor.
XC::MovableID::MovableID(const ID &v)
  :ID(v),MovableObject(0) {}

//! @brief Asigna el ID.
void XC::MovableID::setID(const ID &v)
  { ID::operator=(v); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MovableID::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envia el ID with the parameters (channel,...) being passed as argument.
int XC::MovableID::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dbTag= getDbTag();
    inicComm(2);
    const int dataTag= cp.getDbTag();
    const int sz= Size();
    setDbTagDataPos(0,sz);
    setDbTagDataPos(1,dataTag);

    int res= cp.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "MovableID::sendSelf() - failed to send ID data\n";

    if(sz>0)
      {
        res+= cp.sendID(*this,dataTag);
        if(res<0)
          std::cerr << "MovableID::sendSelf() - failed to send Disp data\n";
      }
    return res;
  }

//! @brief Receives the ID through the communicator being passed as argument.
int XC::MovableID::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dbTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "MovableID::recvSelf() - failed to receive ID data\n";
    else
      {
        const int sz= getDbTagDataPos(0);
        resize(sz);
        if(sz>0)
          {
            const int dataTag= getDbTagDataPos(1);
            if(sz>0)
              res+= cp.receiveID(*this,dataTag);
            if(res<0)
              std::cerr << "MovableID::recvSelf - failed to receive data\n";
          }
      }
    return 0;
  }

