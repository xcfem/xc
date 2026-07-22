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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 04/12
// Revision: A
//
// Description: This file contains the class implementation for the
// VelDepMultiLinear friction model.

#include "VelDepMultiLinear.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"


XC::VelDepMultiLinear::VelDepMultiLinear(int tag)
    : Coulomb(tag, FRN_TAG_VelDepMultiLinear),
      velocityPoints(1), frictionPoints(1),
      trialID(0), trialIDmin(0), trialIDmax(0),
      DmuDvel(0.0)
  {
    // does nothing
  }


XC::VelDepMultiLinear::VelDepMultiLinear(int tag,
    const Vector &velPts, const Vector &frnPts)
    : Coulomb(tag, FRN_TAG_VelDepMultiLinear),
    velocityPoints(velPts), frictionPoints(frnPts),
    trialID(0), trialIDmin(0), trialIDmax(0),
    DmuDvel(0.0)
  {
    const int numDataPoints= velocityPoints.Size();
    if (numDataPoints != frictionPoints.Size())
      {
        std::cerr << "XC::VelDepMultiLinear::VelDepMultiLinear() "
            << "- velocity and friction arrays do not have same length.\n";
        exit(-1);
      }
    trialIDmax = numDataPoints - 2;
    
    // check that velocity and friction points are non-negative
    for (int i=0; i<numDataPoints; i++)
      {
        if (velocityPoints(i) < 0.0 || frictionPoints(i) < 0.0)
	  {
            std::cerr << "XC::VelDepMultiLinear::VelDepMultiLinear - "
		      << "the velocity and friction points have to be positive.\n";
            exit(-1);
	  }
      }
    
    // check that velocity points are monotonically increasing
    for (int i=0; i<numDataPoints-1; i++)
      {
        if (velocityPoints(i) >= velocityPoints(i+1))
	  {
            std::cerr << "XC::VelDepMultiLinear::VelDepMultiLinear - "
                << "the velocity points have to increase monotonically.\n";
            exit(-1);
	  }
      }
    
    // initialize variables
    this->revertToStart();
  }


int XC::VelDepMultiLinear::setTrial(double normalForce, double velocity)
  {
    Coulomb::setTrial(normalForce,velocity);

    const double absTrialVel = fabs(trialVel);
    
    // find the current interval
    double vel1 = velocityPoints(trialID);
    double vel2 = velocityPoints(trialID+1);
    if (absTrialVel >= vel2 && trialID < trialIDmax)
      {
        while (absTrialVel >= vel2 && trialID < trialIDmax)
	  {
            trialID++;
            vel1 = vel2;
            vel2 = velocityPoints(trialID+1);
	  }
      }
    else if (absTrialVel < vel1 && trialID > trialIDmin)
      {
        while (absTrialVel <= vel1 && trialID > trialIDmin)
	  {
            trialID--;
            vel2 = vel1;
            vel1 = velocityPoints(trialID);
	  }
      }
    const double mu1 = frictionPoints(trialID);
    const double mu2 = frictionPoints(trialID+1);
    
    // get derivative of COF wrt velocity for the selected interval
    DmuDvel = (mu2-mu1)/(vel2-vel1);
    
    // get the COF for the selected interval
    mu = mu1 + DmuDvel*(absTrialVel-vel1);
    
    return 0;
  }

boost::python::list XC::VelDepMultiLinear::getVelocityFrictionPoints() const
  {
    boost::python::list retval;
    const int sz= velocityPoints.Size();
    for(int i= 0; i<sz; i++)
      {
	const double &v= velocityPoints[i];
	const double &f= frictionPoints[i];
	const boost::python::tuple tpl= boost::python::make_tuple(v,f);
	retval.append(tpl);
      }
    return retval;
  }

void XC::VelDepMultiLinear::setVelocityFrictionPoints(const boost::python::list &l)
  {
    const int sz= boost::python::len(l);
    velocityPoints.resize(sz);
    frictionPoints.resize(sz);
    for(int i=0; i<sz; i++)
      {
	const boost::python::tuple tmp= boost::python::extract<boost::python::tuple>(l[i]);
	velocityPoints[i]= boost::python::extract<double>(tmp[0]);
	frictionPoints[i]= boost::python::extract<double>(tmp[1]);
      }
  }

double XC::VelDepMultiLinear::getDFFrcDVel() const
  {
    if (trialN > 0.0)
        return DmuDvel*trialN;
    else
        return 0.0;
  }


int XC::VelDepMultiLinear::revertToStart()
  {
    Coulomb::revertToStart();
    
    trialID  = 0;
    
    // find the current interval
    double vel1 = velocityPoints(trialID);
    double vel2 = velocityPoints(trialID+1);
    if (trialVel >= vel2 && trialID < trialIDmax)
      {
        while (trialVel >= vel2 && trialID < trialIDmax)
	  {
            trialID++;
            vel1 = vel2;
            vel2 = velocityPoints(trialID+1);
	  }
      }
    else if (trialVel < vel1 && trialID > trialIDmin)
      {
        while (trialVel <= vel1 && trialID > trialIDmin)
	  {
            trialID--;
            vel2 = vel1;
            vel1 = velocityPoints(trialID);
	  }
      }
    const double mu1 = frictionPoints(trialID);
    const double mu2 = frictionPoints(trialID+1);
    
    // get derivative of COF wrt velocity for the selected interval
    DmuDvel = (mu2-mu1)/(vel2-vel1);
    
    // get the COF for the selected interval
    mu = mu1 + DmuDvel*(trialVel-vel1);
    
    return 0;
  }


XC::FrictionModel* XC::VelDepMultiLinear::getCopy(void) const
  { return new VelDepMultiLinear(*this); }

//! @brief Send data through the communicator argument.
int XC::VelDepMultiLinear::sendData(Communicator &comm)
  {
    int res= Coulomb::sendData(comm);
    res+= comm.sendDouble(DmuDvel, getDbTagData(), CommMetaData(4));
    res+= comm.sendVector(velocityPoints, getDbTagData(), CommMetaData(5));
    res+= comm.sendVector(frictionPoints, getDbTagData(), CommMetaData(6));
    res+= comm.sendInts(trialID, trialIDmin, trialIDmax, getDbTagData(), CommMetaData(7));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VelDepMultiLinear::recvData(const Communicator &comm)
  {
    int res= Coulomb::recvData(comm);
    res+= comm.receiveDouble(DmuDvel, getDbTagData(),CommMetaData(4));
    res+= comm.receiveVector(velocityPoints, getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(frictionPoints, getDbTagData(),CommMetaData(6));
    res+= comm.receiveInts(trialID, trialIDmin, trialIDmax, getDbTagData(),CommMetaData(7));
    return res;
  }

int XC::VelDepMultiLinear::sendSelf(Communicator &comm)
  {
    inicComm(8);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VelDepMultiLinear::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VelDepMultiLinear::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VelDepMultiLinear::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VelDepMultiLinear::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::VelDepMultiLinear::Print(std::ostream &s, int flag) const
  {
    s << "VelDepMultiLinear tag: " << this->getTag() << std::endl;
    s << "  velocityPoints: " << velocityPoints << std::endl;
    s << "  frictionPoints: " << frictionPoints << std::endl;
  }
