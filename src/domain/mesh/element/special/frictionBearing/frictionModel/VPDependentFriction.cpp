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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/VPDependentFriction.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// VPDependentFriction friction model.

#include "VPDependentFriction.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"


XC::VPDependentFriction::VPDependentFriction()
  : VDependentFriction(FRN_TAG_VPDependentFriction),
    A(0.0), deltaMu(0.0), alpha(0.0) {}


XC::VPDependentFriction::VPDependentFriction (int tag, double muslow,
    double mufast0, double a, double deltamu, double _alpha, double transrate)
    : VDependentFriction(tag,muslow,mufast0,transrate,FRN_TAG_VPDependentFriction),
    A(a), deltaMu(deltamu), alpha(_alpha) {}


int XC::VPDependentFriction::setTrial(double normalForce, double velocity)
  {
    VDependentFriction::setTrial(normalForce,velocity);
    const double muFst= muFast0() - deltaMu*tanh(alpha*trialN/A);
    mu= muFst - (muFst-muSlow)*exp(-transRate*fabs(trialVel));
    return 0;
  }


double XC::VPDependentFriction::getDFFrcDNFrc(void)
  {
    if(trialN > 0.0)
      {
        double dFFdFN = mu + deltaMu*alpha*trialN/A/
          pow(cosh(alpha*trialN/A),2)*
          (exp(-transRate*fabs(trialVel))-1.0);
        return dFFdFN;
      }
    else
      return 0.0;
  }


XC::FrictionModel* XC::VPDependentFriction::getCopy(void) const
  { return new VPDependentFriction(*this); }

//! @brief Send data through the communicator argument.
int XC::VPDependentFriction::sendData(Communicator &comm)
  {
    int res= VDependentFriction::sendData(comm);
    res+= comm.sendDoubles(A,deltaMu,alpha,getDbTagData(),CommMetaData(4));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::VPDependentFriction::recvData(const Communicator &comm)
  {
    int res= VDependentFriction::recvData(comm);
    res+= comm.receiveDoubles(A,deltaMu,alpha,getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::VPDependentFriction::sendSelf(Communicator &comm)
  {
    inicComm(5);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "VPDependentFriction::sendSelf - failed to send data.\n";
    return res;
  }


int XC::VPDependentFriction::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "VPDependentFriction::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "VPDependentFriction::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::VPDependentFriction::Print(std::ostream &s, int flag) const
{
    s << "VPDependentFriction tag: " << this->getTag() << std::endl;
    s << "  muSlow: " << muSlow << std::endl;
    s << "  muFast0: " << muFast0() << "  A: " << A << "  deltaMu: " << deltaMu;
    s << "  alpha: " << alpha << std::endl;
    s << "  transRate: " << transRate << std::endl;
}
