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
//RayleighBase.cpp

#include <solution/analysis/integrator/transient/RayleighBase.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
XC::RayleighBase::RayleighBase(AnalysisAggregation *owr,int classTag)
    : DampingFactorsIntegrator(owr,classTag), deltaT(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param fF: Rayleigh damping factors.
XC::RayleighBase::RayleighBase(AnalysisAggregation *owr,int classTag,const RayleighDampingFactors &rF)
  : DampingFactorsIntegrator(owr,classTag,rF),deltaT(0.0) {}



//! @brief Send object members through the channel being passed as parameter.
int XC::RayleighBase::sendData(Communicator &comm)
  {
    int res= DampingFactorsIntegrator::sendData(comm);
    res+= comm.sendDouble(deltaT,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::RayleighBase::recvData(const Communicator &comm)
  {
    int res= DampingFactorsIntegrator::recvData(comm);
    res+= comm.receiveDouble(deltaT,getDbTagData(),CommMetaData(3));
    return res;
  }
