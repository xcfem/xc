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
//RayleighDampingFactors.cpp

#include "RayleighDampingFactors.h"

#include "utility/matrix/Vector.h"
#include <domain/mesh/element/utils/Information.h>

//! @brief Constructor.
XC::RayleighDampingFactors::RayleighDampingFactors(void)
  :CommandEntity(), MovableObject(0), alphaM(0.0), betaK(0.0), betaK0(0.0), betaKc(0.0) {}

//! @brief Constructor.
//!
//! @param aM: factor applied to elements or nodes mass matrix.
//! @param bK: factor applied to elements current stiffness matrix.
//! @param bK0: factor applied to elements initial stiffness matrix.
//! @param bKc: factor applied to elements committed stiffness matrix.
XC::RayleighDampingFactors::RayleighDampingFactors(const double &aM,const double &bK,const double &bK0,const double &bKc)
  :CommandEntity(), MovableObject(0), alphaM(aM), betaK(bK), betaK0(bK0), betaKc(bKc) {}

//! @brief constructor
XC::RayleighDampingFactors::RayleighDampingFactors(const Vector &v)
  : CommandEntity(), MovableObject(0), alphaM(v[0]), betaK(v[1]), betaK0(v[2]), betaKc(v[3]) {}

//! @brief Print Rayleigh factors values.
void XC::RayleighDampingFactors::Print(std::ostream &s, int flag) const
  {
    s << "alphaM: " << alphaM << " betaK: "
      << betaK << " betaK0: " << betaK0
      << " betaKc: " << betaKc << std::endl;
  }

//! @brief Update the value of a parameter.
int XC::RayleighDampingFactors::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
        alphaM= info.theDouble;
        return 0;
      case 2:
        betaK = info.theDouble;
        return 0;
      default:
        return 0;
      }
  }

//! @brief Return the current damping value for the natural frequency argument (alphaM/(2*w)+betaK*w/2
double XC::RayleighDampingFactors::getCurrentDampingRatio(const double &w) const
  { return alphaM/(2*w)+betaK*w/2; }

//! @brief Return the initial damping value for the natural frequency argument (alphaM/(2*w)+betaK*w/2
double XC::RayleighDampingFactors::getInitialDampingRatio(const double &w) const
  { return alphaM/(2*w)+betaK0*w/2; }

//! @brief Return the commited damping value for the natural frequency argument (alphaM/(2*w)+betaK*w/2
double XC::RayleighDampingFactors::getCommitedDampingRatio(const double &w) const
  { return alphaM/(2*w)+betaKc*w/2; }

//! @brief Send object members through the communicator argument.
int XC::RayleighDampingFactors::sendData(Communicator &comm)
  {
    int res=comm.sendDoubles(alphaM,betaK,betaK0,betaKc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::RayleighDampingFactors::recvData(const Communicator &comm)
  {
    int res= comm.receiveDoubles(alphaM,betaK,betaK0,betaKc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::RayleighDampingFactors::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::RayleighDampingFactors::recvSelf(const Communicator &comm)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief insertion on an output stream.
std::ostream &XC::operator<<(std::ostream &os,const RayleighDampingFactors &rF)
  {
    rF.Print(os);
    return os;
  }
