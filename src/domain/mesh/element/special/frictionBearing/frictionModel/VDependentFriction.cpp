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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VDependentFriction.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// VDependentFriction friction model.

#include "VDependentFriction.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"

XC::VDependentFriction::VDependentFriction(int classTag)
  : CoulombFriction(0,classTag), muSlow(0.0), muFast(0.0), transRate(0.0) {}


XC::VDependentFriction::VDependentFriction(int tag, double muslow, double mufast, double transrate, int classTag)
    : CoulombFriction(tag, classTag), muSlow(muslow), muFast(mufast), transRate(transrate)
  {
    if(muSlow <= 0.0  || muFast <= 0.0)
      {
        std::cerr << "VDependentFriction::VDependentFriction - "
            << "the friction coefficients have to be positive\n";
        exit(-1);
      }
  }


int XC::VDependentFriction::setTrial(double normalForce, double velocity)
  {	
    CoulombFriction::setTrial(normalForce,velocity);    
    mu = muFast - (muFast-muSlow)*exp(-transRate*fabs(trialVel));
    return 0;
  }


double XC::VDependentFriction::getDFFrcDNFrc(void)
  {
    if (trialN > 0.0)
        return mu;
    else
        return 0.0;
  }


int XC::VDependentFriction::revertToStart(void)
  {
    CoulombFriction::revertToStart();
    mu= 0.0;    
    return 0;
  }


XC::FrictionModel *XC::VDependentFriction::getCopy(void) const
  { return new VDependentFriction(*this); }

//! @brief Send data through the communicator argument.
int XC::VDependentFriction::sendData(Communicator &comm)
  {
    int res= CoulombFriction::sendData(comm);
    res+= comm.sendDoubles(muSlow,muFast,transRate,getDbTagData(),CommMetaData(3));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VDependentFriction::recvData(const Communicator &comm)
  {
    int res= CoulombFriction::recvData(comm);
    res+= comm.receiveDoubles(muSlow,muFast,transRate,getDbTagData(),CommMetaData(3));
    return res;
  }


int XC::VDependentFriction::sendSelf(Communicator &comm)
  {
    inicComm(4);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VDependentFriction::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VDependentFriction::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VDependentFriction::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VDependentFriction::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::VDependentFriction::Print(std::ostream &s, int flag) const
  {
    s << "VDependentFriction tag: " << this->getTag() << std::endl;
    s << "  muSlow: " << muSlow << std::endl;
    s << "  muFast: " << muFast << std::endl;
    s << "  transRate: " << transRate << std::endl;
  }
