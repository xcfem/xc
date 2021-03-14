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
//ElasticBaseMaterial.cc

#include <material/uniaxial/ElasticBaseMaterial.h>

#include "utility/matrix/Vector.h"


//! @brief Constructor.
XC::ElasticBaseMaterial::ElasticBaseMaterial(int tag, int classtag, double e,double e0)
  : UniaxialMaterial(tag,classtag), trialStrain(0.0), E(e), ezero(e0) {}

//! @brief Sets initial stress.
int XC::ElasticBaseMaterial::setInitialStrain(const double &strain)
  {
    ezero= strain;
    return 0;
  }

//! @brief Increments the initial strain.
int XC::ElasticBaseMaterial::incrementInitialStrain(const double &initStrainIncrement)
  {
    ezero+= initStrainIncrement;
    return 0;
  }

//! @brief Zeroes the initial strain.
void XC::ElasticBaseMaterial::zeroInitialStrain(void)
  { ezero= 0.0; }

//! @brief Revert the material to its initial state.
int XC::ElasticBaseMaterial::revertToStart(void)
  {
    UniaxialMaterial::revertToStart();
    trialStrain= 0.0;
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::ElasticBaseMaterial::sendData(Communicator &comm)
  {
    int res= UniaxialMaterial::sendData(comm);
    res+= comm.sendDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ElasticBaseMaterial::recvData(const Communicator &comm)
  {
    int res= UniaxialMaterial::recvData(comm);
    res+= comm.receiveDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ElasticBaseMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3); 
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ElasticBaseMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticBaseMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
           std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }
