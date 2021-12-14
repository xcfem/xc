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

#include "ParameterIDBeamIntegration.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag, const Vector &pt, const Vector &wt)
  : UserDefinedBeamIntegrationBase(classTag,pt,wt), parameterID(0)
  {}

XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag, const Vector &pt)
  : UserDefinedBeamIntegrationBase(classTag,pt), parameterID(0)
  {}

XC::ParameterIDBeamIntegration::ParameterIDBeamIntegration(int classTag)
  : UserDefinedBeamIntegrationBase(classTag), parameterID(0)
  {}


int XC::ParameterIDBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    return -1;
  }

int XC::ParameterIDBeamIntegration::updateParameter(int parameterID, Information &info)
  {
    // Does nothing for now -- MHS
    return 0;
  }

int XC::ParameterIDBeamIntegration::activateParameter(int paramID)
  {
    parameterID = paramID;
    // For Terje to do
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::ParameterIDBeamIntegration::sendData(Communicator &comm)
  {
    int res= UserDefinedBeamIntegrationBase::sendData(comm);
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ParameterIDBeamIntegration::recvData(const Communicator &comm)
  {
    int res= UserDefinedBeamIntegrationBase::recvData(comm);
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(5));
    return res;
  }

void XC::ParameterIDBeamIntegration::Print(std::ostream &s, int flag) const
  {
    s << "ParameterID" << std::endl;
    UserDefinedBeamIntegrationBase::Print(s);
    s << " Points: " << pts;
    s << " Weights: " << wts;
    double sum = 0.0;
    int N = wts.Size();
    for (int i = 0; i < N; i++)
      sum += fabs(wts(i));
    s << " Condition Number: " << sum << std::endl;
  }
