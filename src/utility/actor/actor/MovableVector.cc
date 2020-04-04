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
//MovableVector.cc

#include "MovableVector.h"

#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "Communicator.h"
#include "CommMetaData.h"

//! @brief Constructor.
XC::MovableVector::MovableVector(const size_t &sz)
  :Vector(sz),MovableObject(VECTOR_TAG_Vector) {}

//! @brief Constructor.
XC::MovableVector::MovableVector(const Vector &v)
  :Vector(v),MovableObject(VECTOR_TAG_Vector) {}

//! @brief Asigna el vector.
void XC::MovableVector::setVector(const Vector &v)
  { Vector::operator=(v); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MovableVector::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envia el vector through the communicator argument.
int XC::MovableVector::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dbTag= getDbTag();
    inicComm(2);
    const int dataTag= comm.getDbTag();
    const int sz= Size();
    setDbTagDataPos(0,sz);
    setDbTagDataPos(1,dataTag);

    int res= comm.sendIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "MovableVector::sendSelf() - failed to send ID data\n";

    if(sz>0)
      {
        res+= comm.sendVector(*this,dataTag);
        if(res<0)
          std::cerr << "MovableVector::sendSelf() - failed to send Disp data\n";
      }
    return res;
  }

//! @brief Receive the vector through the communicator argument.
int XC::MovableVector::recvSelf(const Communicator &comm)
  {
    inicComm(2);
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "MovableVector::recvSelf() - failed to receive ID data\n";
    else
      {
        const int sz= getDbTagDataPos(0);
        resize(sz);
        if(sz>0)
          {
            const int dataTag= getDbTagDataPos(1);
            res+= comm.receiveVector(*this,dataTag);
            if(res<0)
              std::cerr << "MovableVector::recvSelf - failed to receive data\n";
          }
      }
    return res;
  }

