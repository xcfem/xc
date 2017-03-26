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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ViscousMaterial.cpp,v $
                                                                        
// Written: Mehrdad Sasani 
// Created: June 2000
// Revision: A
//
// Description: This file contains the class implementation for 
// ViscousMaterial. 

#include <cmath>

#include "ViscousMaterial.h"
#include "utility/matrix/Vector.h"

XC::ViscousMaterial::ViscousMaterial(int tag, double c, double a)
:UniaxialMaterial(tag,MAT_TAG_Viscous), trialRate(0.0), C(c), Alpha(a)
  {
    if (Alpha < 0.0) {
      std::cerr << "ViscousMaterial::ViscousMaterial -- Alpha < 0.0, setting to 1.0\n";
      Alpha = 1.0;
    }
  }

XC::ViscousMaterial::ViscousMaterial(int tag)
  :UniaxialMaterial(tag,MAT_TAG_Viscous),  trialRate(0.0), C(0.0), Alpha(0.0)
  {}


int XC::ViscousMaterial::setTrialStrain(double strain, double strainRate)
  {
    trialRate = strainRate;
    return 0;
  }

double XC::ViscousMaterial::getStress(void) const
  {
    double stress = C*pow(fabs(trialRate),Alpha);
    if(trialRate < 0.0)
      return -stress;
    else
      return  stress;
  }

double XC::ViscousMaterial::getTangent(void) const
  { return 0.0; }

double XC::ViscousMaterial::getInitialTangent(void) const
  { return 0.0; }

double XC::ViscousMaterial::getDampTangent(void) const
  {
    static const double minvel = 1.e-11;
    double absRate = fabs(trialRate);
    if(absRate < minvel)
      return Alpha*C*pow(minvel,Alpha-1.0);
    else
      return Alpha*C*pow(absRate,Alpha-1.0);        
  }


double XC::ViscousMaterial::getStrain(void) const
  { return 0.0; }

double XC::ViscousMaterial::getStrainRate(void) const
  { return trialRate; }

int XC::ViscousMaterial::commitState(void)
  { return 0; }

int XC::ViscousMaterial::revertToLastCommit(void)
  { return 0; }

int XC::ViscousMaterial::revertToStart(void)
  {
    trialRate = 0.0;
    return 0;
  }

XC::UniaxialMaterial *XC::ViscousMaterial::getCopy(void) const
  { return new ViscousMaterial(*this); }


//! @brief Send object members through the channel being passed as parameter.
int XC::ViscousMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(trialRate,C,Alpha,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ViscousMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialRate,C,Alpha,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ViscousMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ViscousMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::ViscousMaterial::Print(std::ostream &s, int flag)
  {
    s << "Viscous tag: " << this->getTag() << std::endl;
    s << "  C: " << C << std::endl;
    s << "  Alpha: " << Alpha << std::endl;
  }


