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
//PulseBaseSeries.cpp

#include "PulseBaseSeries.h"
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"



//! @brief Constructor.
XC::PulseBaseSeries::PulseBaseSeries(int classTag,const double &startTime,const double &finishTime,const double &factor)
  : CFactorSeries(classTag,factor), tStart(startTime),tFinish(finishTime) {}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PulseBaseSeries::sendData(CommParameters &cp)
  {
    int res= CFactorSeries::sendData(cp);
    res+= cp.sendDoubles(tStart,tFinish,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PulseBaseSeries::recvData(const CommParameters &cp)
  {
    int res= CFactorSeries::recvData(cp);
    res+= cp.receiveDoubles(tStart,tFinish,getDbTagData(),CommMetaData(1));
    return res;
  }


//! @brief Sends object through the channel being passed as parameter.
int XC::PulseBaseSeries::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PulseBaseSeries::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PulseBaseSeries::recvSelf(const CommParameters &cp)
  {
    inicComm(2);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PulseBaseSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }

//! @brief Imprime el objeto.
void XC::PulseBaseSeries::Print(std::ostream &s, int flag) const
  {
    s << "Pulse Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
  }
