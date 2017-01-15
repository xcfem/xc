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
//ArcLengthBase.cpp


#include <solution/analysis/integrator/static/ArcLengthBase.h>
#include <utility/matrix/Vector.h>

#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor.
XC::ArcLengthBase::ArcLengthBase(SoluMethod *owr,int classTag,double arcLength, double alpha)
  :ProtoArcLength(owr,classTag,arcLength), alpha2(alpha*alpha) {}

//! @brief Devuelve el valor de dLambda para el método newStep.
double XC::ArcLengthBase::getDLambdaNewStep(void) const
  {
    const Vector &dUhat= vectores.getDeltaUhat();
    // determine delta lambda(1) == dlambda
    double retval = sqrt(arcLength2/((dUhat^dUhat)+alpha2));
    retval *= signLastDeltaLambdaStep; // base sign of load change
                                        // on what was happening last step
    return retval;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ArcLengthBase::sendData(CommParameters &cp)
  {
    int res= ProtoArcLength::sendData(cp);
    res+= cp.sendDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ArcLengthBase::recvData(const CommParameters &cp)
  {
    int res= ProtoArcLength::recvData(cp);
    res+= cp.receiveDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::ArcLengthBase::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ArcLengthBase::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

