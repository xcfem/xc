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
//NewmarkBase2.cpp

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>


//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
XC::NewmarkBase2::NewmarkBase2(SolutionStrategy *owr,int classTag)
  :NewmarkBase(owr,classTag), beta(0.0), c1(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param theGamma: gamma factor for Newmark method.
//! @param theBeta: beta factor for Newmark method.
XC::NewmarkBase2::NewmarkBase2(SolutionStrategy *owr,int classTag, double theGamma, double theBeta)
  :NewmarkBase(owr,classTag,theGamma), beta(theBeta), c1(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param theGamma: gamma factor for Newmark method.
//! @param theBeta: beta factor for Newmark method.
//! @param fF: Rayleigh damping factors.
XC::NewmarkBase2::NewmarkBase2(SolutionStrategy *owr,int classTag,double theGamma, double theBeta,const RayleighDampingFactors &rF)
  :NewmarkBase(owr,classTag,theGamma,rF), beta(theBeta), c1(0.0) {}

//! @brief Send object members through the communicator argument.
int XC::NewmarkBase2::sendData(Communicator &comm)
  {
    int res= NewmarkBase::sendData(comm);
    res+= comm.sendDoubles(beta,c1,getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::NewmarkBase2::recvData(const Communicator &comm)
  {
    int res= NewmarkBase::recvData(comm);
    res+= comm.receiveDoubles(beta,c1,getDbTagData(),CommMetaData(13));
    return res;
  }

