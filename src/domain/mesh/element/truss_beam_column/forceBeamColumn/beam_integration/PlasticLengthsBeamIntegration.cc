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
//PlasticLengthsBeamIntegration.cc

#include "PlasticLengthsBeamIntegration.h"
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::PlasticLengthsBeamIntegration::PlasticLengthsBeamIntegration(int classTag,double lpi,double lpj)
  : BeamIntegration(classTag), lpI(lpi), lpJ(lpj), parameterID(0) {}

XC::PlasticLengthsBeamIntegration::PlasticLengthsBeamIntegration(int classTag)
  : BeamIntegration(classTag), lpI(0.0), lpJ(0.0), parameterID(0) {}


//! @brief Send object members through the channel being passed as parameter.
int XC::PlasticLengthsBeamIntegration::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(lpI,lpJ,getDbTagData(),CommMetaData(1));
    res+= cp.sendInt(parameterID,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PlasticLengthsBeamIntegration::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(lpI,lpJ,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(parameterID,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PlasticLengthsBeamIntegration::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PlasticLengthsBeamIntegration::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Assigns to the param the value identified by the string in argv.
int XC::PlasticLengthsBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    const size_t sz= argv.size();
    if(sz>0)
      {
	if(argv[0] == "lpI")
	  {
	    param.setValue(lpI);
	    retval= param.addObject(1, this);
	  }
	else if(argv[0] == "lpJ")
	  {
	    param.setValue(lpJ);
  	    retval= param.addObject(2, this);
	  }
	else if(argv[0] == "lp")
	  {
	    param.setValue(lpI);
	    retval= param.addObject(3, this);
	  }
      }
    return retval;
  }

//! @brief Update the value of the parameter.
int XC::PlasticLengthsBeamIntegration::updateParameter(int parameterID,Information &info)
  {
    switch (parameterID)
      {
      case 1:
	lpI = info.theDouble;
	return 0;
      case 2:
	lpJ = info.theDouble;
	return 0;
      case 3:
	lpI = lpJ = info.theDouble;
	return 0;
      default:
	return -1;
    }
  }

//! @brief Activate parameter.
int XC::PlasticLengthsBeamIntegration::activateParameter(int paramID)
  {
    parameterID = paramID;
    // For Terje to do
    return 0;
  }

void XC::PlasticLengthsBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "PlasticLengths" << std::endl;
    s << " lpI = " << lpI;
    s << " lpJ = " << lpJ << std::endl;
    return;
  }
