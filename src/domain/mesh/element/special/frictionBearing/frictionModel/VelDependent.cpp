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

// $Revision: 1.1 $
// $Date: 2009/04/17 23:02:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VelDependent.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// VelDependent friction model.

#include "VelDependent.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"

XC::VelDependent::VelDependent(int tag, int classTag)
  : Coulomb(tag, classTag),
    muSlow(0.0), muFast(0.0), transRate(0.0), DmuDvel(0.0) {}


XC::VelDependent::VelDependent(int tag, double muslow, double mufast, double transrate, int classTag)
    : Coulomb(tag, classTag),
      muSlow(muslow), muFast(mufast), transRate(transrate), DmuDvel(0.0)
  {
    // check that COF are positive and not zero
    if(muSlow <= 0.0  || muFast <= 0.0)
      {
        std::cerr << "VelDependent::VelDependent - "
		  << "the friction coefficients have to be positive."
	        << std::endl;
        exit(-1);
      }
    // check that transRate is positive
    if (transRate < 0.0)
      {
	std::cerr << "VelDependent::VelDependent - "
		  << "the transition rate has to be positive."
		  << std::endl;
        exit(-1);
      }
  }


int XC::VelDependent::setTrial(double normalForce, double velocity)
  {	
    Coulomb::setTrial(normalForce,velocity);    
    // get COF for given trial velocity
    const double temp = (muFast-muSlow)*exp(-transRate*fabs(trialVel));
    mu= muFast - temp;
    // get derivative of COF wrt velocity
    if(trialVel != 0.0)
        DmuDvel = transRate*trialVel/fabs(trialVel)*temp;
    else
        DmuDvel = 0.0;

    return 0;
  }


double XC::VelDependent::getDFFrcDNFrc(void) const
  {
    if (trialN > 0.0)
        return mu;
    else
        return 0.0;
  }

double XC::VelDependent::getDFFrcDVel(void) const
  {
    if (trialN > 0.0)
        return DmuDvel*trialN;
    else
        return 0.0;
  }


int XC::VelDependent::revertToStart(void)
  {
    Coulomb::revertToStart();
    this->mu= muSlow;
    this->DmuDvel= 0.0;
    return 0;
  }


XC::FrictionModel *XC::VelDependent::getCopy(void) const
  { return new VelDependent(*this); }

//! @brief Send data through the communicator argument.
int XC::VelDependent::sendData(Communicator &comm)
  {
    int res= Coulomb::sendData(comm);
    res+= comm.sendDoubles(muSlow,muFast,transRate, DmuDvel, getDbTagData(),CommMetaData(3));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VelDependent::recvData(const Communicator &comm)
  {
    int res= Coulomb::recvData(comm);
    res+= comm.receiveDoubles(muSlow,muFast,transRate, DmuDvel, getDbTagData(),CommMetaData(3));
    return res;
  }


int XC::VelDependent::sendSelf(Communicator &comm)
  {
    inicComm(4);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VelDependent::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VelDependent::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VelDependent::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VelDependent::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::VelDependent::Print(std::ostream &s, int flag) const
  {
    s << "VelDependent tag: " << this->getTag() << std::endl;
    s << "  muSlow: " << muSlow << std::endl;
    s << "  muFast: " << muFast << std::endl;
    s << "  transRate: " << transRate << std::endl;
  }
