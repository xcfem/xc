//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/05/08 22:56:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber/UniaxialFiber3d.cpp,v $
                                                                        
                                                                        
// File: ~/section/UniaxialFiber3d.C
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the implementation for the
// UniaxialFiber3d class. UniaxialFiber3d provides the abstraction of a
// uniaxial fiber that forms a fiber section for 3d frame elements.
// The UniaxialFiber3d is subjected to a stress state with
// only one nonzero axial stress and corresponding axial strain.
//
// What: "@(#) UniaxialFiber3d.C, revA"

#include <cstdlib>
#include <cstdio>

#include "UniaxialFiber3d.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/SectionForceDeformation.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/FiberResponse.h>
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::Matrix XC::UniaxialFiber3d::ks(3,3); 
XC::Vector XC::UniaxialFiber3d::fs(3); 

void XC::UniaxialFiber3d::set_position(const Vector &position)
  {
    as[0]= -position(0); //Sign of Y coordinate changed.
    as[1]=  position(1);
  }

//! @brief Constructor.
XC::UniaxialFiber3d::UniaxialFiber3d(int classTag)
  : UniaxialFiber(classTag)
  {
    as[0]= 0.0;
    as[1]= 0.0;
  }

//! @brief Constructor.
XC::UniaxialFiber3d::UniaxialFiber3d(int tag,const UniaxialMaterial &theMat,const double &Area, const Vector &position,int classTag)
  : UniaxialFiber(tag, classTag,theMat,Area)
  { set_position(position); }


//! @brief Constructor.
XC::UniaxialFiber3d::UniaxialFiber3d(int tag,const MaterialHandler &ldr,const std::string &nmbMat, const double &Area,const Vector &position,int classTag)
  : UniaxialFiber(tag, classTag,ldr,nmbMat,Area)
  { set_position(position); }

//! @brief Sets the fiber trian strain.
int XC::UniaxialFiber3d::setTrialFiberStrain(const Vector &vs)
  {
    //Sign of Y coordinate is changed.
    const double strain= vs(0) + as[0]*vs(1) + as[1]*vs(2);
    if(theMaterial)
      return theMaterial->setTrialStrain(strain);
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no material!\n";
        return -1; // in case fatal does not exit
      }
  }

//! @brief Returns the stress resultant of the fiber. 
XC::Vector &XC::UniaxialFiber3d::getFiberStressResultants(void)
  {
    if(isAlive())
      {
        const double df= theMaterial->getStress() * area;
        // fs= as^ df;
        fs(0)= df;
        fs(1)= as[0]*df; //Sign of Y coordinate is changed.
        fs(2)= as[1]*df;
      }
    else
      fs.Zero();
    return fs;
  }

//! @brief Returns the fiber contribution to the section stiffness.
XC::Matrix &XC::UniaxialFiber3d::getFiberTangentStiffContr(void) 
  {
    if(isAlive())
      {
        // ks= (as^as) * area * Et;
        const double value= theMaterial->getTangent() * area;

        const double as1= as[0]; //Sign of Y coordinate is changed.
        const double as2= as[1];
        const double vas1= as1*value;
        const double vas2= as2*value;
        const double vas1as2= vas1*as2;

        ks(0,0)= value;
        ks(0,1)= vas1;
        ks(0,2)= vas2;
    
        ks(1,0)= vas1;
        ks(1,1)= vas1*as1;
        ks(1,2)= vas1as2;
    
        ks(2,0)= vas2;
        ks(2,1)= vas1as2;
        ks(2,2)= vas2*as2;
      }
    else
      ks.Zero();
    return ks;
  }

//! @brief Virtual constructor.
XC::Fiber *XC::UniaxialFiber3d::getCopy(void) const
  { return new UniaxialFiber3d(*this); }

//! @brief Returns the dimension of the stress resultant vector.
int XC::UniaxialFiber3d::getOrder(void) const
  { return 3; }

const XC::ResponseId &XC::UniaxialFiber3d::getType(void) const
  { return RespFiberSection3d; }

//! @brief Send data through the channel being passed as parameter.
int XC::UniaxialFiber3d::sendData(CommParameters &cp)
  {
    int res= UniaxialFiber::sendData(cp);
    res+= cp.sendDoubles(as[0],as[1],getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::UniaxialFiber3d::recvData(const CommParameters &cp)
  {    
    int res= UniaxialFiber::recvData(cp);
    res+= cp.receiveDoubles(as[0],as[1],getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::UniaxialFiber3d::sendSelf(CommParameters &cp)
   {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::UniaxialFiber3d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::UniaxialFiber3d::Print(std::ostream &s, int flag) const
  {
    s << "\nUniaxialFiber3d, tag: " << this->getTag() << std::endl;
    s << "\tArea: " << area << std::endl;
    //Sign of Y coordinate is changed.
    s << "\tMatrix as: " << 1.0 << " " << -as[0] << " " << as[1] << std::endl; 
    s << "\tMaterial, tag: " << theMaterial->getTag() << std::endl;
  }

XC::Response *XC::UniaxialFiber3d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    const int argc= argv.size();
    if(argc == 0)
      return 0;
    if(argv[0] == "force" || argv[0] == "forces")
      return new FiberResponse(this, 1, Vector(3));
    else
      return theMaterial->setResponse(argv, info);
  }

int XC::UniaxialFiber3d::getResponse(int responseID, Information &fibInfo)
  {
    switch(responseID)
      {
      case 1:
        return fibInfo.setVector(this->getFiberStressResultants());
      default:
        return -1;
      }
  }

//! @brief Returns fiber position.
void XC::UniaxialFiber3d::getFiberLocation(double &yLoc, double &zLoc) const
  {
    yLoc= -as[0]; //Sign of Y coordinate is changed.
    zLoc= as[1];
  }
