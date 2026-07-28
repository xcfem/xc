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

// $Revision: 1.1 $
// $Date: 2009/04/17 23:02:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/Coulomb.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for the
// Coulomb friction model.

#include "Coulomb.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"


XC::Coulomb::Coulomb(int tag, int classTag)
  : FrictionModel(tag, classTag),  mu(0.0) {}


XC::Coulomb::Coulomb(int tag, double _mu, int classTag)
  : FrictionModel(tag, classTag), mu(_mu)
  {
    if(mu <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the friction coefficient has to be positive."
	          << std::endl;
        exit(-1);
      }
     
    // initialize variables
    this->revertToStart();
  }


int XC::Coulomb::setTrial(double normalForce, double velocity)
  {	
    trialN   = normalForce;
    trialVel = velocity;  
    return 0;
  }


double XC::Coulomb::getFrictionForce(void) const
  {
    if(trialN > 0.0)
      return mu*trialN;
    else
      return 0.0;
  }


double XC::Coulomb::getFrictionCoeff(void) const
  { return mu; }


double XC::Coulomb::getDFFrcDNFrc(void) const
  {
    if(trialN > 0.0)
      return mu;
    else
      return 0.0;
  }

double XC::Coulomb::getDFFrcDVel(void) const
  { return 0.0; }


int XC::Coulomb::commitState(void)
  { return 0; }


int XC::Coulomb::revertToLastCommit(void)
  { return 0; }

XC::FrictionModel* XC::Coulomb::getCopy(void) const
  { return new Coulomb(*this); }


//! @brief Send data through the communicator argument.
int XC::Coulomb::sendData(Communicator &comm)
  {
    int res= FrictionModel::sendData(comm);
    res+= comm.sendDouble(mu,getDbTagData(),CommMetaData(2));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::Coulomb::recvData(const Communicator &comm)
  {
    int res= FrictionModel::recvData(comm);
    res+= comm.receiveDouble(mu,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::Coulomb::sendSelf(Communicator &comm)
  {
    inicComm(3);
  
    int res= sendData(comm);

    const int dbTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dbTag);
    if(res < 0)
      std::cerr << "Coulomb::sendSelf - failed to send data.\n";
    return res;
  }


int XC::Coulomb::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "Coulomb::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << "Coulomb::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::Coulomb::Print(std::ostream  &s, int flag) const
  {
    s << "Coulomb tag: " <<  getTag() << std::endl;
    s << "  mu: " << mu << std::endl;
  }
