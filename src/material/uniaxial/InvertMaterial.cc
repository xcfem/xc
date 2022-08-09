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
                                                                        
// $Revision: 1.6 $
// $Date: 2010-09-11 00:50:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/InvertMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition for 
// InvertMaterial.  InvertMaterial wraps a UniaxialMaterial
// and imposes min and max strain limits.

#include "InvertMaterial.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::InvertMaterial::InvertMaterial(int tag, UniaxialMaterial &material)
  :EncapsulatedMaterial(tag,MAT_TAG_Invert, material)
  {}

XC::InvertMaterial::InvertMaterial(int tag)
  :EncapsulatedMaterial(tag,MAT_TAG_Invert)
  {}

int XC::InvertMaterial::setTrialStrain(double strain, double strainRate)
  { return theMaterial->setTrialStrain(-strain, -strainRate); }


// int XC::InvertMaterial::setTrialStrain(double strain, double temp, double strainRate)
//   {
//     return theMaterial->setTrialStrain(strain, temp, strainRate);
//   }

//! @brief Return the material stress.
double XC::InvertMaterial::getStress(void) const
  { return -theMaterial->getStress(); }

//! @brief Return the tangent stiffness.
double XC::InvertMaterial::getTangent(void) const
  { return -theMaterial->getTangent();  }

double XC::InvertMaterial::getDampTangent(void) const
  { return -theMaterial->getDampTangent(); }

int XC::InvertMaterial::commitState(void)
  { return theMaterial->commitState(); }

int XC::InvertMaterial::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

int XC::InvertMaterial::revertToStart(void)
  { return theMaterial->revertToStart(); }

XC::UniaxialMaterial *XC::InvertMaterial::getCopy(void) const
  { return new InvertMaterial(*this); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::InvertMaterial::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }


int XC::InvertMaterial::sendSelf(Communicator &comm)
  {
    inicComm(4);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::InvertMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;    
  }

void XC::InvertMaterial::Print(std::ostream &s, int flag) const
  {
    s << "InvertMaterial, tag: " << this->getTag() << std::endl
      << "  material: " << theMaterial->getTag() << std::endl;
  }

int XC::InvertMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(theMaterial)
      retval= theMaterial->setParameter(argv, param);
    return retval;
  }

int XC::InvertMaterial::updateParameter(int parameterID, Information &info)
  { return 0; }

double XC::InvertMaterial::getStressSensitivity(int gradIndex, bool conditional)
  { return theMaterial->getStressSensitivity(gradIndex, conditional); }

double XC::InvertMaterial::getStrainSensitivity(int gradIndex)
  { return theMaterial->getStrainSensitivity(gradIndex); }

double XC::InvertMaterial::getInitialTangentSensitivity(int gradIndex)
  { return theMaterial->getInitialTangentSensitivity(gradIndex); }

double XC::InvertMaterial::getDampTangentSensitivity(int gradIndex)
  { return theMaterial->getDampTangentSensitivity(gradIndex); }

double XC::InvertMaterial::getRhoSensitivity(int gradIndex)
  { return theMaterial->getRhoSensitivity(gradIndex); }

int XC::InvertMaterial::commitSensitivity(double strainGradient, int gradIndex, int numGrads)
  {
    return theMaterial->commitSensitivity(strainGradient, gradIndex, numGrads);
  }
