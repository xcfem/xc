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
//HingeBeamIntegration2d.cc

#include "HingeBeamIntegration2d.h"
#include "domain/mesh/element/utils/fvectors/FVector.h"
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "material/section/repres/CrossSectionProperties2d.h"

XC::HingeBeamIntegration2d::HingeBeamIntegration2d(int classTag,double e,double a,double i,double lpi,double lpj)
  : PlasticLengthsBeamIntegration(classTag,lpi,lpj), E(e), A(a), I(i) {}

XC::HingeBeamIntegration2d::HingeBeamIntegration2d(int classTag)
  : PlasticLengthsBeamIntegration(classTag), E(0.0), A(0.0), I(0.0) {}


void XC::HingeBeamIntegration2d::addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0)
  {
    if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
      beamMecLoad->addElasticDeformations(L,CrossSectionProperties2d(E,A,I),lpI,lpJ,loadFactor,v0);
    else
      std::cerr << "XC::HingeBeamIntegration2d::addElasticDeformations -- load type unknown." << std::endl;
    return;
  }

//! @brief Send object members through the communicator argument.
int XC::HingeBeamIntegration2d::sendData(Communicator &comm)
  {
    int res= PlasticLengthsBeamIntegration::sendData(comm);
    res+= comm.sendDoubles(E,A,I,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HingeBeamIntegration2d::recvData(const Communicator &comm)
  {
    int res= PlasticLengthsBeamIntegration::recvData(comm);
    res+= comm.receiveDoubles(E,A,I,getDbTagData(),CommMetaData(2));
    return res;
  }

void XC::HingeBeamIntegration2d::Print(std::ostream &s, int flag) const
  {
    PlasticLengthsBeamIntegration::Print(s,flag);
    s << "Hinge2d" << std::endl;
    s << " E = " << E;
    s << " A = " << A;
    s << " I = " << I << std::endl;
    return;
  }
