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
//InternalParamsLRIn.cc

#include "material/uniaxial/PY/InternalParamsLRIn.h"

//! @brief Constructor with data
XC::InternalParamsLRIn::InternalParamsLRIn(const double &V,const double &r,const double &t,const double &yl,const double &yr,const double &pl,const double &pr)
  : InternalParamsLR(V,r,t,yl,yr), RLeft(pl), RRight(pr) {}

//! @brief Send object members through the communicator argument.
int XC::InternalParamsLRIn::sendData(Communicator &comm)
  {
    int res= InternalParamsLR::sendData(comm);
    res+= comm.sendDoubles(RLeft,RRight,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::InternalParamsLRIn::recvData(const Communicator &comm)
  {
    int res= InternalParamsLR::recvData(comm);
    res+= comm.receiveDoubles(RLeft,RRight,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::InternalParamsLRIn::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(2); 
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "InternalParamsLRIn::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::InternalParamsLRIn::recvSelf(const Communicator &comm)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "InternalParamsLRIn::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "InternalParamsLRIn::recvSelf - failed to receive data.\n";
      }
    return res;
  }
