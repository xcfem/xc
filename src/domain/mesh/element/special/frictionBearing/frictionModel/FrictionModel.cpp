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
    else if(argv[0]=="normalForce")
        return new FrictionResponse(this, 2, this->getNormalForce());
    
    // velocity
    if(argv[0]=="velocity")
        return new FrictionResponse(this, 1, this->getVelocity());
    
    // friction force
    else if(argv[0]=="frictionForce")
        return new FrictionResponse(this, 3, this->getFrictionForce());
    
    // friction coeff
    else if(argv[0]=="frictionCoeff")
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

//! @brief Envia los datos through the channel being passed as parameter.
int XC::FrictionModel::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendDoubles(trialN,trialVel,getDbTagData(),CommMetaData(1));
    return res;
  }


//! @brief Recibe los datos through the channel being passed as parameter.
int XC::FrictionModel::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveDoubles(trialN,trialVel,getDbTagData(),CommMetaData(1));
    return res;
  }
