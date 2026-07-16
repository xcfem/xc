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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/FrictionModel.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class implementation for FrictionModel.
//
// What: "@(#) FrictionModel.cpp, revA"

#include "FrictionModel.h"
#include "FrictionResponse.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"

XC::FrictionModel::FrictionModel(int tag, int classTag)
  : TaggedObject(tag), MovableObject(classTag), trialN(0.0), trialVel(0.0) {}


double XC::FrictionModel::getNormalForce(void)
  { return trialN; }


double XC::FrictionModel::getVelocity(void)
  { return trialVel; }


XC::Response *XC::FrictionModel::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    const size_t argc= argv.size();
    if(argc == 0) 
      return 0;
    // normal force
    else if((argv[0]=="normalForce") ||
	    (argv[0]=="N") ||
	    (argv[0]=="normalFrc"))
        return new FrictionResponse(this, 2, this->getNormalForce());
    
    // velocity
    else if((argv[0]=="velocity") || (argv[0]=="vel"))
        return new FrictionResponse(this, 1, this->getVelocity());
    
    // friction force
    else if((argv[0]=="frictionForce") || (argv[0]=="Ff") ||
	    (argv[0]=="frnForce") || (argv[0]=="frnFrc"))
        return new FrictionResponse(this, 3, this->getFrictionForce());
    
    // friction coeff
    else if((argv[0]=="frictionCoeff") || (argv[0]=="mu") ||
	    (argv[0]=="frnCoeff") || (argv[0]=="COF"))
        return new FrictionResponse(this, 4, this->getFrictionCoeff());
    // otherwise unknown
    else
        return 0;
}


int XC::FrictionModel::getResponse(int responseID, Information &info)
{
    // each subclass must implement its own stuff    
    switch (responseID)  {
    case 1:
        info.setDouble(trialN);
        return 0;
        
    case 2:
        info.setDouble(trialVel);
        return 0;      
        
    case 3:
        info.setDouble(this->getFrictionForce());
        return 0;      
                
    case 4:
        info.setDouble(this->getFrictionCoeff());
        return 0;      

    default:      
        return -1;
    }
}

int XC::FrictionModel::revertToStart(void)
  {
    trialN= 0.0;
    trialVel= 0.0;
    return 0;
  }

//! @brief Send data through the communicator argument.
int XC::FrictionModel::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    int res= comm.sendDoubles(trialN,trialVel,getDbTagData(),CommMetaData(1));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::FrictionModel::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(0));
    int res= comm.receiveDoubles(trialN,trialVel,getDbTagData(),CommMetaData(1));
    return res;
  }
