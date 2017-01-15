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
  :UniaxialMaterial(tag,classtag), trialStrain(0.0),E(e),ezero(e0) {}

//! @brief Establece el valor de la tensión inicial.
int XC::ElasticBaseMaterial::setInitialStrain(double strain)
  {
    ezero= strain;
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ElasticBaseMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ElasticBaseMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticBaseMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ElasticBaseMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ElasticBaseMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
           std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }
