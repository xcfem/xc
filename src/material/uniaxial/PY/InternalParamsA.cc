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

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::InternalParamsA::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(v,R,Tang,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::InternalParamsA::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(v,R,Tang,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::InternalParamsA::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(1); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "InternalParamsA::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::InternalParamsA::recvSelf(const CommParameters &cp)
  {
    inicComm(1);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "InternalParamsA::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
           std::cerr << "InternalParamsA::recvSelf - failed to receive data.\n";
      }
    return res;
  }
