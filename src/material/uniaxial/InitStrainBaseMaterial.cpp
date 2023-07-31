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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/InitStrainBaseMaterial.cpp,v $

#include <material/uniaxial/InitStrainBaseMaterial.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::InitStrainBaseMaterial::InitStrainBaseMaterial(int tag, int classTag, const UniaxialMaterial &material, double epsini)
  : EncapsulatedMaterial(tag, classTag, material),
   epsInit(epsini)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(!tmp)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to get copy of material" << std::endl;
	exit(-1);
      }
  }

XC::InitStrainBaseMaterial::InitStrainBaseMaterial(int tag, int classTag)
  :EncapsulatedMaterial(tag, classTag),
   epsInit(0.0) {}

int XC::InitStrainBaseMaterial::setInitialStrain(const double &initStrain)
  {
    epsInit= initStrain;
    return 0;
  }

int XC::InitStrainBaseMaterial::incrementInitialStrain(const double &incStrain)
  {
    return setInitialStrain(epsInit+incStrain);
  }

void XC::InitStrainBaseMaterial::zeroInitialStrain(void)
  {
    setInitialStrain(0.0);
  }

double XC::InitStrainBaseMaterial::getInitialStrain(void) const
  { return epsInit; }

double XC::InitStrainBaseMaterial::getStress(void) const
  {
    const UniaxialMaterial *tmp= theMaterial.getMaterial();
    if(tmp)
      return tmp->getStress();
    else
      return 0.0;
  }

//! @brief Return the material tangent stiffness.
double XC::InitStrainBaseMaterial::getTangent(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->getTangent();
    else
      return 0.0;
  }

double XC::InitStrainBaseMaterial::getDampTangent(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->getDampTangent();
    else
      return 0.0;
  }

double XC::InitStrainBaseMaterial::getStrainRate(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->getStrainRate();
    else
      return 0.0;
  }

//! @brief Commit the state of the material.
int XC::InitStrainBaseMaterial::commitState(void)
  {        
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->commitState();
    else
      return -1;
  }

int XC::InitStrainBaseMaterial::revertToLastCommit(void)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->revertToLastCommit();
    else
      return -1;
  }

int XC::InitStrainBaseMaterial::revertToStart(void)
  {
    int retval= -1;
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
        retval= tmp->revertToStart();
        retval+= tmp->setTrialStrain(epsInit);
        retval+= tmp->commitState();
      }
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::InitStrainBaseMaterial::sendData(Communicator &comm)
  {
    int res= EncapsulatedMaterial::sendData(comm);
    res+= comm.sendDouble(epsInit,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::InitStrainBaseMaterial::recvData(const Communicator &comm)
  {
    int res= EncapsulatedMaterial::recvData(comm);
    res+= comm.receiveDouble(epsInit, getDbTagData(),CommMetaData(4));
    return res;
  }

double XC::InitStrainBaseMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if (tmp)
      return tmp->getStressSensitivity(gradIndex, conditional);
    else
      return 0.0;
  }

double XC::InitStrainBaseMaterial::getInitialTangentSensitivity(int gradIndex)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->getInitialTangentSensitivity(gradIndex);
    else
      return 0.0;
  }

int XC::InitStrainBaseMaterial::commitSensitivity(double strainGradient,  int gradIndex, int numGrads)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->commitSensitivity(strainGradient, gradIndex, numGrads);
    else
      return -1;
  }
