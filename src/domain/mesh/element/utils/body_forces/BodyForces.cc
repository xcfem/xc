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
//BodyForces.cc

#include "BodyForces.h"

XC::BodyForces::BodyForces(const size_t &sz)
  : MovableObject(-1), forces(sz) {} //XXX Definition of classTag pending.

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::BodyForces::getDbTagData(void) const
  {
    static DbTagData retval(2); 
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BodyForces::sendData(CommParameters &cp)
  {
    int res= cp.sendVector(forces,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BodyForces::recvData(const CommParameters &cp)
  {
    int res= cp.receiveVector(forces,this->getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object.
int XC::BodyForces::sendSelf(CommParameters &cp)
  {
    inicComm(2);

    int res= this->sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "BodyForces::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object
int XC::BodyForces::recvSelf(const CommParameters &cp)
  {
    inicComm(2);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BodyForces::recvSelf -- failed to receive ID data\n";
    else
      res+= this->recvData(cp);
    return res;
  }

//! @brief Sends object.
void XC::BodyForces::Print(std::ostream &os) const
  {
    os << forces;
  }  
