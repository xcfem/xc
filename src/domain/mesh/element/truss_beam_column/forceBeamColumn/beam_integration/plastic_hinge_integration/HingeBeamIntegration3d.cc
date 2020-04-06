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
//HingeBeamIntegration3d.cc

#include "HingeBeamIntegration3d.h"
#include "domain/mesh/element/utils/fvectors/FVector.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/component/Parameter.h"

#include <utility/matrix/Vector.h>
#include <domain/mesh/element/utils/Information.h>
#include "utility/matrix/ID.h"

XC::HingeBeamIntegration3d::HingeBeamIntegration3d(int classTag,double e,double a,double iz,double iy,double g,double j,double lpi,double lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj),ctes_scc(e,a,iz,iy,g,j) {}

XC::HingeBeamIntegration3d::HingeBeamIntegration3d(int classTag, const CrossSectionProperties3d &cts,const double &lpi,const double &lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj), ctes_scc(cts) {}

void XC::HingeBeamIntegration3d::addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0)
  {
    if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
      beamMecLoad->addElasticDeformations(L,ctes_scc,lpI,lpJ,loadFactor,v0);
    return;
  }

int XC::HingeBeamIntegration3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    std::cerr << "Passing null pointer." << std::endl;
    int retval= ctes_scc.setParameter(argv,param, nullptr);
    if(retval <= 0)
      retval= PlasticLengthsBeamIntegration::setParameter(argv, param);
    return retval;
  }

int XC::HingeBeamIntegration3d::updateParameter(int parameterID, Information &info)
  {
    if(parameterID<=6)
      return ctes_scc.updateParameter(parameterID,info);
    else
      return PlasticLengthsBeamIntegration::updateParameter(parameterID-6, info);
  }

int XC::HingeBeamIntegration3d::activateParameter(int parameterID)
  {
    // For Terje to do
    return 0;
  }

//! @brief Send members through the communicator argument.
int XC::HingeBeamIntegration3d::sendData(Communicator &comm)
  {
    int res= PlasticLengthsBeamIntegration::sendData(comm);
    res+=comm.sendMovable(ctes_scc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::HingeBeamIntegration3d::recvData(const Communicator &comm)
  {
    int res= PlasticLengthsBeamIntegration::recvData(comm);
    res+= comm.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::HingeBeamIntegration3d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::HingeBeamIntegration3d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::HingeBeamIntegration3d::Print(std::ostream &s, int flag) const
  {
    s << "HingeMidpoint3d" << std::endl;
    ctes_scc.Print(s);
    s << " lpI = " << lpI;
    s << " lpJ = " << lpJ << std::endl;
    return;
  }
