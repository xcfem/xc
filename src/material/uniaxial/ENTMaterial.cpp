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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ENTMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ENTMaterial.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ENTMaterial. 
//
// What: "@(#) ENTMaterial.C, revA"

#include <material/uniaxial/ENTMaterial.h>
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/utils/Information.h>


XC::ENTMaterial::ENTMaterial(int tag, const double &e,const double &A,const double &B)
  :ElasticBaseMaterial(tag,MAT_TAG_ENTMaterial,e), a(A), b(B), parameterID(0) {}

int XC::ENTMaterial::setTrialStrain(double strain, double strainRate)
  {
    trialStrain = strain;
    return 0;
  }

//! @brief Return stress.
double XC::ENTMaterial::getStress(void) const
  {
    if(trialStrain<0.0)
      return E*trialStrain;
    else if (a == 0.0)
      return 0.0;
    else
      return a*E*tanh(trialStrain*b);
  }

//! @brief Returns elastic modulus.
double XC::ENTMaterial::getTangent(void) const
  {
    if(trialStrain<=0.0)
      return E;
    else if(a == 0.0)
      return 0.0;
    else
      {
        const double tanhB = tanh(trialStrain*b);
        return a*E*(1.0-tanhB*tanhB);
      }
  }

XC::UniaxialMaterial *XC::ENTMaterial::getCopy(void) const
  { return new ENTMaterial(*this); }

//! @brief Send object members through the communicator argument.
int XC::ENTMaterial::sendData(Communicator &comm)
  {
    int res= ElasticBaseMaterial::sendData(comm);
    res+= comm.sendDoubles(a,b,getDbTagData(),CommMetaData(3));
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ENTMaterial::recvData(const Communicator &comm)
  {
    int res= ElasticBaseMaterial::recvData(comm);
    res+= comm.receiveDoubles(a,b,getDbTagData(),CommMetaData(3));
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ENTMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5); 
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ENTMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ENTMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ENTMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
           std::cerr << "ENTMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::ENTMaterial::Print(std::ostream &os, int flag) const
  {
    os << getClassName() << ", tag: " << this->getTag() << std::endl
       << "  E: " << E << std::endl;
  }

int XC::ENTMaterial::setParameter(const std::vector<std::string > &argv, Parameter &param)
  {
    int retval= -1;
    if(argv[0] == "E")
      {
        param.setValue(E);
        retval= param.addObject(1, this);
      }
    return retval;
  }

int XC::ENTMaterial::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
	case -1:
	  return -1;
	case 1:
	  E = info.theDouble;
	  return 0;
	default:
	  return -1;
      }
  }

int XC::ENTMaterial::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

double XC::ENTMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    if(parameterID == 1 && trialStrain < 0.0)
      return trialStrain;
    else
      return 0.0;
  }

double XC::ENTMaterial::getInitialTangentSensitivity(int gradIndex)
  { return 0.0; }

int XC::ENTMaterial::commitSensitivity(double strainGradient,int gradIndex, int numGrads)
  {
    // Nothing to commit ... path independent
    return 0.0;
  }
