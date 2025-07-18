// -*-c++-*-
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

// $Revision: 1.0 $
// $Date: 2019-01-28 17:53:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/SecStifDamping.cpp,v $

// Written: Yuli Huang (yulee@berkeley.edu)
// Created: 02/2020
// Revision: A
// 
// Description: This file contains the implementation for the SecStifDamping class.
// FRDamping provides the abstraction of an elemental damping imposition
// providing quasi tangent stiffness proportional damping
//
// What: "@(#) SecStifDamping.cpp, revA"

#include "SecStifDamping.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "utility/actor/actor/CommMetaData.h"
#include "domain/load/pattern/TimeSeries.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//extern StaticAnalysis *theStaticAnalysis;

//! @brief Constructor.
XC::SecStifDamping::SecStifDamping(int tag, double b, double t1, double t2, TimeSeries *f)
  : Damping(tag, DMP_TAG_SecStifDamping, t1, t2, f),
    beta(b)
  {
    if (beta <= 0.0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << ":  Invalid damping factor."
	        << Color::def << std::endl;
  }


//! @brief Default constructor.
XC::SecStifDamping::SecStifDamping(void)
  : Damping(0, DMP_TAG_SecStifDamping, 0.0, 0.0, nullptr),
    beta(0.0)
  {}

int XC::SecStifDamping::update(const Vector &q)
  {       
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    //StaticAnalysis **theStaticAnalysis = OPS_GetStaticAnalysis();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    StaticAnalysis *theStaticAnalysis= nullptr;
    if(theStaticAnalysis)
      {
	qd.Zero();
      }
    else if (dT > 0.0)
      {
	q0 = q;
	if (t > ta && t < td)
	  {
	    qd = (beta / dT) * (q0 - q0C);
	    if (fac) qd *= fac->getFactor(t);
	  }
	else
	  {
	    qd.Zero();
	  }
      }
    return 0;
  }

double XC::SecStifDamping::getStiffnessMultiplier(void) const
  {
    const double t= theDomain->getCurrentTime();
    const double dT= theDomain->getDt();
    double km= 0.0;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    if(!theStaticAnalysis && dT > 0.0 && t > ta && t < td)
       km = beta/dT;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    return 1.0 + km;
}


XC::Damping *XC::SecStifDamping::getCopy(void) const
  { return new SecStifDamping(*this); }

//! @brief Send object members through the communicator argument.
int XC::SecStifDamping::sendData(Communicator &comm)
  {
    int res= Damping::sendData(comm);
    res+= comm.sendDouble(beta,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SecStifDamping::recvData(const Communicator &comm)
  {
    int res= Damping::recvData(comm);
    res+= comm.receiveDouble(beta, getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::SecStifDamping::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::SecStifDamping::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
	            << Color::def << std::endl;
      }
    return res;
  }

void XC::SecStifDamping::Print(std::ostream &s, int flag)
  {
    s << "\nDamping: " << this->getTag() << " Type: SecStifDamping"
      << "\tdamping factor: " << beta << std::endl
      << "\tactivation time: " << ta << std::endl
      << "\tdeactivation time: " << td << std::endl;
  }

int XC::SecStifDamping::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {

    if(argv[0]=="dampingFactor")
      {
	param.setValue(beta);
	return param.addObject(1, this);
      }
    return -1;
  }


int XC::SecStifDamping::updateParameter(int parameterID, Information &info)
  {
  switch(parameterID) {
  case 1:
    beta = info.theDouble;
    return 0;
  default:
    return -1;
  }
}

