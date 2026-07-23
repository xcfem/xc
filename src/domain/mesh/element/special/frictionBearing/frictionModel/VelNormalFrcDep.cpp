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

// $Revision$
// $Date$
// $URL$

// Developed: Nhan D. Dao (nhan.unr@gmail.com)
// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/14
// Revision: A
//
// Description: This file contains the class implementation for the
// VelNormalFrcDep friction model.

#include "VelNormalFrcDep.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::VelNormalFrcDep::VelNormalFrcDep(int tag)
  : Coulomb(tag, FRN_TAG_VelNormalFrcDep),
    aSlow(0.0), nSlow(1.0), aFast(0.0), nFast(1.0),
    alpha0(0.0), alpha1(0.0), alpha2(0.0), maxMuFact(2.0),
    DmuDn(0.0), DmuDvel(0.0)
  {
    // does nothing
  }

//! @brief Constructor.
XC::VelNormalFrcDep::VelNormalFrcDep(int tag,
    double aslow, double nslow, double afast, double nfast,
    double _alpha0, double _alpha1, double _alpha2, double maxmufact)
    : Coulomb(tag, FRN_TAG_VelNormalFrcDep),
    aSlow(aslow), nSlow(nslow), aFast(afast), nFast(nfast),
    alpha0(_alpha0), alpha1(_alpha1), alpha2(_alpha2), maxMuFact(maxmufact),
    DmuDn(0.0), DmuDvel(0.0)
  {
    // check that constants are positive and not zero
    if (aSlow <= 0.0  || aFast <= 0.0)  {
        std::cerr << "XC::VelNormalFrcDep::VelNormalFrcDep - "
            << "the aSlow & aFast constants have to be positive.\n";
        exit(-1);
    }
    // check that exponents are <= 1
    if (nSlow > 1.0 || nFast > 1.0)  {
        std::cerr << "XC::VelNormalFrcDep::VelNormalFrcDep - "
            << "the exponents n have to be <= 1.0.\n";
        exit(-1);
    }
    
    // initialize variables
    this->revertToStart();
}



int XC::VelNormalFrcDep::setTrial(double normalForce, double velocity)
  {
    Coulomb::setTrial(normalForce,velocity);
    
    // determine slow and fast COF
    const double muSlow = aSlow*pow(trialN,nSlow-1.0);
    const double muFast = aFast*pow(trialN,nFast-1.0);
    
    // determine rate parameter
    double transRate = alpha0 + alpha1*trialN + alpha2*pow(trialN,2.0);
    
    // get COF for given normal force and trial velocity
    double temp1 = exp(-transRate*fabs(trialVel));
    double temp2 = (muFast - muSlow)*temp1;
    this->mu = muFast - temp2;
    if (this->mu > maxMuFact*muFast || trialN <= 0.0)
        this->mu = maxMuFact*muFast;
    
    // get derivative of COF wrt normal force
    const double DmuSlowDn= aSlow*(nSlow-1.0)*pow(trialN,nSlow-2.0);
    const double DmuFastDn= aFast*(nFast-1.0)*pow(trialN,nFast-2.0);
    this->DmuDn= DmuFastDn - (DmuFastDn - DmuSlowDn)*temp1
      + (alpha1 + 2.0*alpha2*trialN)*fabs(trialVel)*temp2;
    
    // get derivative of COF wrt velocity
    if (trialVel != 0.0)
        this->DmuDvel = transRate*trialVel/fabs(trialVel)*temp2;
    else
        this->DmuDvel = 0.0;
    
    return 0;
  }



double XC::VelNormalFrcDep::getDFFrcDNFrc(void) const
  {
    if (trialN >= 0.0)
        return mu + DmuDn*trialN;
    else
        return 0.0;
  }


double XC::VelNormalFrcDep::getDFFrcDVel(void) const
  {
    if (trialN > 0.0)
        return DmuDvel*trialN;
    else
        return 0.0;
  }


int XC::VelNormalFrcDep::revertToStart()
  {
    trialN   = 0.0;
    trialVel = 0.0;
    mu       = aSlow*pow(trialN,(nSlow-1.0));
    DmuDn    = 0.0;
    DmuDvel  = 0.0;
    
    return 0;
  }


XC::FrictionModel* XC::VelNormalFrcDep::getCopy()
  { return new VelNormalFrcDep(*this); }

//! @brief Send data through the communicator argument.
int XC::VelNormalFrcDep::sendData(Communicator &comm)
  {
    int res= Coulomb::sendData(comm);
    res+= comm.sendDoubles(aSlow, nSlow, aFast, nFast, getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(alpha0, alpha1, alpha2, maxMuFact, getDbTagData(),CommMetaData(5));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VelNormalFrcDep::recvData(const Communicator &comm)
  {
    int res= Coulomb::recvData(comm);
    res+= comm.receiveDoubles(aSlow, nSlow, aFast, nFast, getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(alpha0, alpha1, alpha2, maxMuFact, getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::VelNormalFrcDep::sendSelf(Communicator &comm)
  {
    inicComm(5);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VelNormalFrcDep::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VelNormalFrcDep::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VelNormalFrcDep::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VelNormalFrcDep::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::VelNormalFrcDep::Print(std::ostream &s, int flag) const
  {
    s << "VelNormalFrcDep tag: " << this->getTag() << std::endl;
    s << "  aSlow: " << aSlow << "  nSlow: " << nSlow;
    s << "  aFast: " << aFast << "  nFast: " << nFast << std::endl;
    s << "  alpha0: " << alpha0 << "  alpha1: " << alpha1;
    s << "  alpha2: " << alpha2 << std::endl;
    s << "  maxMuFact: " << maxMuFact << std::endl;
  }
