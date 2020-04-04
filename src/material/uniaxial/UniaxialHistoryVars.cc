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

#include "UniaxialHistoryVars.h"

XC::UniaxialHistoryVars::UniaxialHistoryVars(void)
  :MovableObject(0), minStrain(0.0), unloadSlope(0.0), endStrain(0.0) {}


//! @brief Returns the initial material state.
int XC::UniaxialHistoryVars::revertToStart(const double &E)
  {
    minStrain= 0.0;
    endStrain= 0.0;
    unloadSlope= E;
    return 0;
  }

void XC::UniaxialHistoryVars::zero(void)
  {
    minStrain= 0.0;
    unloadSlope= 0.0;
    endStrain= 0.0;
  }

//! @brief Send object members through the communicator argument.
int XC::UniaxialHistoryVars::sendData(Communicator &comm)
  {
    int res= comm.sendDoubles(minStrain,unloadSlope,endStrain,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::UniaxialHistoryVars::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(minStrain,unloadSlope,endStrain,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::UniaxialHistoryVars::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(10); 
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "UniaxialHistoryVars::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::UniaxialHistoryVars::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "UniaxialHistoryVars::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "UniaxialHistoryVars::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::UniaxialHistoryVars::Print(std::ostream &s, int flag) const
  {
    s << "UniaxialHistoryVars, min. strain: " << minStrain << std::endl;
    s << "  unload slope: " << unloadSlope << std::endl;
    s << "  end strain: " << endStrain << std::endl;
  }


