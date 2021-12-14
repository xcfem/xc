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
//InternalParams.cc

#include "material/uniaxial/PY/InternalParamsA.h"

/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::InternalParamsA::InternalParamsA(const double &V,const double &r,const double &t)
  : MovableObject(0),v(V), R(r), Tang(t)  {}

void XC::InternalParamsA::getField(const double &V)
  {
    v= V;
    R= Tang*v;
  }

const double &XC::InternalParamsA::updateField(const double &Tp)
  {
    double dv= residual(Tp);
    v+= dv;
    return v;
  }

void XC::InternalParamsA::revertToStart(const double &tg)
  {
    v= 0.0;
    R= 0.0;
    Tang= tg;
  }

//! @brief Send object members through the communicator argument.
int XC::InternalParamsA::sendData(Communicator &comm)
  {
    int res= comm.sendDoubles(v,R,Tang,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::InternalParamsA::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(v,R,Tang,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::InternalParamsA::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(1); 
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "InternalParamsA::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::InternalParamsA::recvSelf(const Communicator &comm)
  {
    inicComm(1);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "InternalParamsA::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "InternalParamsA::recvSelf - failed to receive data.\n";
      }
    return res;
  }
