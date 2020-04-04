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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.10 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTBase.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTBase class.
//
// What: "@(#) HHTBase.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHTBase.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
XC::HHTBase::HHTBase(AnalysisAggregation *owr,int classTag)
  : HHTRayleighBase(owr,classTag), beta(0.0), c1(0.0) {}

//! @brief Constructor.
//!
//! Sets \f$\alpha\f$ to \p alpha and \f$\gamma\f$ to \f$(1.5 - \alpha)\f$.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
XC::HHTBase::HHTBase(AnalysisAggregation *owr,int classTag,double _alpha)
  : HHTRayleighBase(owr,classTag,_alpha),
    beta((2-_alpha)*(2-_alpha)*0.25), c1(0.0) {}

//! @brief Constructor.
//!
//! Sets \f$\alpha\f$ to \p alpha, \f$\gamma\f$ to \f$(1.5 - \alpha)\f$
//! and \f$\beta\f$ to \f$0.25*(2-\alpha)^2\f$.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param rF: value of the Rayleigh damping factors.
XC::HHTBase::HHTBase(AnalysisAggregation *owr,int classTag,double _alpha,const RayleighDampingFactors &rF)
  : HHTRayleighBase(owr,classTag,_alpha,rF),
    beta((2-_alpha)*(2-_alpha)*0.25), c1(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param _beta: value for the beta parameter.
//! @param _gamma: value for the gamma parameter.
XC::HHTBase::HHTBase(AnalysisAggregation *owr,int classTag,double _alpha, double _beta, double _gamma)
  : HHTRayleighBase(owr,classTag,_alpha,_gamma),
    beta(_beta), c1(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _alpha: value for the alpha parameter.
//! @param _beta: value for the beta parameter.
//! @param _gamma: value for the gamma parameter.
//! @param rF: value of the Rayleigh damping factors.
XC::HHTBase::HHTBase(AnalysisAggregation *owr,int classTag,double _alpha, double _beta, double _gamma,const RayleighDampingFactors &rF)
    : HHTRayleighBase(owr,classTag,_alpha,_gamma,rF), beta(_beta), c1(0.0) {}


//! @brief Send object members through the channel being passed as parameter.
int XC::HHTBase::sendData(Communicator &comm)
  {
    int res= HHTRayleighBase::sendData(comm);
    res+= comm.sendDoubles(beta,c1,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::HHTBase::recvData(const Communicator &comm)
  {
    int res= HHTRayleighBase::recvData(comm);
    res+= comm.receiveDoubles(beta,c1,getDbTagData(),CommMetaData(8));
    return res;
  }

