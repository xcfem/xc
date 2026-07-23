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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VelPressureDep.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// VelPressureDep friction model.

#include "VelPressureDep.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"


XC::VelPressureDep::VelPressureDep(int tag)
  : VelDependent(tag, FRN_TAG_VelPressureDep),
    A(0.0), deltaMu(0.0), alpha(0.0), DmuDn(0.0) {}


XC::VelPressureDep::VelPressureDep(int tag, double muslow,
    double mufast0, double a, double deltamu, double _alpha, double transrate)
    : VelDependent(tag,muslow,mufast0,transrate,FRN_TAG_VelPressureDep),
    A(a), deltaMu(deltamu), alpha(_alpha), DmuDn(0.0)
  {
    // check that A is positive and not zero
    if (A <= 0.0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "the nominal contact area has to be positive."
	          << std::endl;
        exit(-1);
      } 
  }

//! @brief Virtual constructor.
XC::FrictionModel* XC::VelPressureDep::getCopy(void) const
  { return new VelPressureDep(*this); }

int XC::VelPressureDep::setTrial(double normalForce, double velocity)
  {
    VelDependent::setTrial(normalForce,velocity);
    
    // determine COF at high velocities
    double muFst= muFast0();
    if(trialN>0.0)
      muFst-=deltaMu*tanh(alpha*trialN/A);

    // get COF for given trial velocity
    const double temp1= exp(-transRate*fabs(trialVel));
    const double temp2= (muFst-muSlow)*temp1;
    this->mu= muFst - temp2;
    
    // get derivative of COF wrt normal force
    this->DmuDn= deltaMu*alpha/A/pow(cosh(alpha*trialN/A),2)*(temp1-1.0);
    
    // get derivative of COF wrt velocity
    if (trialVel != 0.0)
      this->DmuDvel= transRate*trialVel/fabs(trialVel)*temp2;
    else
      this->DmuDvel= 0.0;

    return 0;
  }


double XC::VelPressureDep::getDFFrcDNFrc(void)
  {
    if (trialN >= 0.0)
        return mu + DmuDn*trialN;
    else
        return 0.0;
  }

int XC::VelPressureDep::revertToStart()
  {
    VelDependent::revertToStart();
    DmuDn= 0.0;
    
    return 0;
  }

//! @brief Send data through the communicator argument.
int XC::VelPressureDep::sendData(Communicator &comm)
  {
    int res= VelDependent::sendData(comm);
    res+= comm.sendDoubles(A,deltaMu,alpha, DmuDn,getDbTagData(),CommMetaData(4));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VelPressureDep::recvData(const Communicator &comm)
  {
    int res= VelDependent::recvData(comm);
    res+= comm.receiveDoubles(A,deltaMu,alpha, DmuDn, getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::VelPressureDep::sendSelf(Communicator &comm)
  {
    inicComm(5);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VelPressureDep::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VelPressureDep::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VelPressureDep::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VelPressureDep::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::VelPressureDep::Print(std::ostream &s, int flag) const
{
    s << "VelPressureDep tag: " << this->getTag() << std::endl;
    s << "  muSlow: " << muSlow << std::endl;
    s << "  muFast0: " << muFast0() << "  A: " << A << "  deltaMu: " << deltaMu;
    s << "  alpha: " << alpha << std::endl;
    s << "  transRate: " << transRate << std::endl;
}
