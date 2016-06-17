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
// $Source: /usr/local/cvs/OpenSees/SRC/element/frictionBearing/frictionModel/CoulombFriction.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// CoulombFriction friction model.

#include "CoulombFriction.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"


XC::CoulombFriction::CoulombFriction(int classTag)
  : FrictionModel(0, classTag),  mu(0.0) {}


XC::CoulombFriction::CoulombFriction (int tag, double _mu, int classTag)
  : FrictionModel(tag, classTag), mu(_mu)
  {
    if(mu <= 0.0)
      {
        std::cerr << "CoulombFriction::CoulombFriction - "
            << "the friction coefficient has to be positive\n";
        exit(-1);
      }
  }


int XC::CoulombFriction::setTrial(double normalForce, double velocity)
  {	
    trialN   = normalForce;
    trialVel = velocity;  
    return 0;
  }


double XC::CoulombFriction::getFrictionForce(void)
  {
    if(trialN > 0.0)
      return mu*trialN;
    else
      return 0.0;
  }


double XC::CoulombFriction::getFrictionCoeff(void)
  {
    if(trialN > 0.0)
      return mu;
    else
      return 0.0;
  }


double XC::CoulombFriction::getDFFrcDNFrc(void)
  {
    if(trialN > 0.0)
      return mu;
    else
      return 0.0;
  }


int XC::CoulombFriction::commitState(void)
  { return 0; }


int XC::CoulombFriction::revertToLastCommit(void)
  { return 0; }


XC::FrictionModel* XC::CoulombFriction::getCopy(void) const
  { return new CoulombFriction(*this); }


//! @brief Envia los datos through the channel being passed as parameter.
int XC::CoulombFriction::sendData(CommParameters &cp)
  {
    int res= FrictionModel::sendData(cp);
    res+= cp.sendDouble(mu,getDbTagData(),CommMetaData(2));
    return res;
  }


//! @brief Recibe los datos through the channel being passed as parameter.
int XC::CoulombFriction::recvData(const CommParameters &cp)
  {
    int res= FrictionModel::recvData(cp);
    res+= cp.receiveDouble(mu,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::CoulombFriction::sendSelf(CommParameters &cp)
  {
    inicComm(3);
  
    int res= sendData(cp);

    const int dbTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "CoulombFriction::sendSelf - failed to send data.\n";
    return res;
  }


int XC::CoulombFriction::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    
    const int dbTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "CoulombFriction::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
           std::cerr << "CoulombFriction::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::CoulombFriction::Print(std::ostream  &s, int flag)
  {
    s << "CoulombFriction tag: " <<  getTag() << std::endl;
    s << "  mu: " << mu << std::endl;
  }
