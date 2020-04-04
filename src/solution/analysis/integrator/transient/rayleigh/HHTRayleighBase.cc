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
//HHTRayleighBase.cpp

#include <solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
XC::HHTRayleighBase::HHTRayleighBase(AnalysisAggregation *owr,int classTag)
  : RayleighBase(owr,classTag),
    alpha(1.0), gamma(0.0), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
//!
//! Sets \f$\alpha\f$ to \p alpha and \f$\gamma\f$ to \f$(1.5 - \alpha)\f$.
//! 
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
XC::HHTRayleighBase::HHTRayleighBase(AnalysisAggregation *owr,int classTag,double _alpha)
    : RayleighBase(owr,classTag),
    alpha(_alpha),gamma(1.5-_alpha), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
//!
//! Sets \f$\alpha\f$ to \p alpha and \f$\gamma\f$ to \f$(1.5 - \alpha)\f$.
//! 
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param rF: value of the Rayleigh damping factors.
XC::HHTRayleighBase::HHTRayleighBase(AnalysisAggregation *owr,int classTag,double _alpha,const RayleighDampingFactors &rF)
    : RayleighBase(owr,classTag,rF),
    alpha(_alpha), gamma(1.5-_alpha), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param _gamma: value for the gamma parameter.
XC::HHTRayleighBase::HHTRayleighBase(AnalysisAggregation *owr,int classTag,double _alpha, double _gamma)
    : RayleighBase(owr,classTag),
    alpha(_alpha), gamma(_gamma), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param _gamma: value for the gamma parameter.
//! @param rF: value of the Rayleigh damping factors.
XC::HHTRayleighBase::HHTRayleighBase(AnalysisAggregation *owr,int classTag,double _alpha, double _gamma,const RayleighDampingFactors &rF)
    : RayleighBase(owr,classTag,rF), alpha(_alpha), gamma(_gamma), c2(0.0), c3(0.0), 
    Ut(0), U(0), Ualpha(0) {}


//! @brief Send object members through the communicator argument.
int XC::HHTRayleighBase::sendData(Communicator &comm)
  {
    int res= RayleighBase::sendData(comm);
    res+= comm.sendMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(U,getDbTagData(),CommMetaData(5));
    res+= comm.sendMovable(Ualpha,getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(alpha,gamma,c2,c3,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HHTRayleighBase::recvData(const Communicator &comm)
  {
    int res= RayleighBase::recvData(comm);
    res+= comm.receiveMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(U,getDbTagData(),CommMetaData(5));
    res+= comm.receiveMovable(Ualpha,getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(alpha,gamma,c2,c3,getDbTagData(),CommMetaData(7));
    return res;
  }

