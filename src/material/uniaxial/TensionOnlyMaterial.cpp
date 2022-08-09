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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/TensionOnlyMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition for 
// TensionOnlyMaterial.  TensionOnlyMaterial wraps a UniaxialMaterial
// and imposes min and max strain limits.

#include "TensionOnlyMaterial.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::TensionOnlyMaterial::TensionOnlyMaterial(int tag, UniaxialMaterial &material)
  :EncapsulatedMaterial(tag,MAT_TAG_TensionOnly, material)
  {}

XC::TensionOnlyMaterial::TensionOnlyMaterial(int tag)
  :EncapsulatedMaterial(tag,MAT_TAG_TensionOnly)
  {}

int XC::TensionOnlyMaterial::setTrialStrain(double strain, double strainRate)
  {
    return theMaterial->setTrialStrain(strain, strainRate);
  }


// int XC::TensionOnlyMaterial::setTrialStrain(double strain, double temp, double strainRate)
//   {
//     return theMaterial->setTrialStrain(strain, temp, strainRate);
//   }

//! @brief Return the material stress.
double XC::TensionOnlyMaterial::getStress(void) const
  {
    const double f= theMaterial->getStress();
    if (f < 0.0)
      return factor*f;
    else
      return f;
  }

//! @brief Return the tangent stiffness.
double XC::TensionOnlyMaterial::getTangent(void) const
  {
    const double E= theMaterial->getTangent();
    const double f= theMaterial->getStress();
    if(f < 0.0)
      return factor*E;
    else
      return E;
  }

double XC::TensionOnlyMaterial::getDampTangent(void) const
  {
    const double D= theMaterial->getDampTangent();
    const double f= theMaterial->getStress();
    if (f < 0.0)
      return factor*D;
    else
      return D;
  }


int XC::TensionOnlyMaterial::commitState(void)
  {
    const double f= theMaterial->getStress();
    if(f >= 0.0)
      return theMaterial->commitState();
    else
      return 0;
  }

int XC::TensionOnlyMaterial::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

int XC::TensionOnlyMaterial::revertToStart(void)
  { return theMaterial->revertToStart(); }

XC::UniaxialMaterial *XC::TensionOnlyMaterial::getCopy(void) const
  { return new TensionOnlyMaterial(*this); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::TensionOnlyMaterial::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }


int XC::TensionOnlyMaterial::sendSelf(Communicator &comm)
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

int XC::TensionOnlyMaterial::recvSelf(const Communicator &comm)
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

void XC::TensionOnlyMaterial::Print(std::ostream &s, int flag) const
  {
    s << "TensionOnlyMaterial, tag: " << this->getTag() << std::endl
      << "  material: " << theMaterial->getTag() << std::endl;
  }

int XC::TensionOnlyMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(theMaterial)
      retval= theMaterial->setParameter(argv, param);
    return retval;
  }

int XC::TensionOnlyMaterial::updateParameter(int parameterID, Information &info)
  { return 0; }

double XC::TensionOnlyMaterial::getStressSensitivity(int gradIndex, bool conditional)
{
  double f = theMaterial->getStress();
  if (f < 0.0)
    return 0.0;
  else 
    return theMaterial->getStressSensitivity(gradIndex, conditional);
}

double
XC::TensionOnlyMaterial::getStrainSensitivity(int gradIndex)
{
  return theMaterial->getStrainSensitivity(gradIndex);
}

double
XC::TensionOnlyMaterial::getInitialTangentSensitivity(int gradIndex)
{
  return theMaterial->getInitialTangentSensitivity(gradIndex);
}

double
XC::TensionOnlyMaterial::getDampTangentSensitivity(int gradIndex)
{
  double f = theMaterial->getStress();
  if (f < 0.0)
    return 0.0;
  else
    return theMaterial->getDampTangentSensitivity(gradIndex);
}

double
XC::TensionOnlyMaterial::getRhoSensitivity(int gradIndex)
{
  return theMaterial->getRhoSensitivity(gradIndex);
}

int   
XC::TensionOnlyMaterial::commitSensitivity(double strainGradient,
				       int gradIndex, int numGrads)
{
  double f = theMaterial->getStress();
  if (f >= 0.0)
    return theMaterial->commitSensitivity(strainGradient, gradIndex, numGrads);
  else
    return 0;
}
