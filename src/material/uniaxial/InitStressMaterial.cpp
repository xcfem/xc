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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/InitStressMaterial.cpp,v $

#include <material/uniaxial/InitStressMaterial.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Determine the initial strain.
int XC::InitStressMaterial::findInitialStrain(void)
  {
    // determine the initial strain
    double tol= 1e-12;
    double dSig= sigInit;
    double tStrain = 0.0, tStress = 0.0;
    int count = 0;

    UniaxialMaterial *tmp= this->getMaterial();
    double K= tmp->getTangent();
    double dStrain= dSig/K;
    do
      {
        count++;
        tStrain+= dStrain;
        tmp->setTrialStrain(tStrain);
        tStress = tmp->getStress();
        dSig= sigInit-tStress; // residual.
        K = tmp->getTangent();
        dStrain = dSig/K;
      }
    while ((fabs(dSig) > tol) && (count <= 100));

    epsInit = tStrain;

    if ((fabs(tStress-sigInit) < tol)) 
      tmp->setTrialStrain(epsInit);
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: could not find initStrain to within tol for material: "
		  << tmp->getTag()
		  << " wanted sigInit: " << sigInit
		  << " using tStress: " << tmp->getStress()
		  << Color::def << std::endl;
        return -1;
      }
    return 0;
  }

//! @brief Sets the encapsulated material.
void XC::InitStressMaterial::setMaterial(const UniaxialMaterial &material)
  {
    InitStrainBaseMaterial::setMaterial(material);
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	if(this->findInitialStrain() == 0)
	  tmp->commitState();
      }    
  }

XC::InitStressMaterial::InitStressMaterial(int tag, const UniaxialMaterial &material, double sigini)
  : InitStrainBaseMaterial(tag,MAT_TAG_InitStress, material, 0.0),
    sigInit(sigini)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	if(this->findInitialStrain() == 0)
	  tmp->commitState();
      }
  }

XC::InitStressMaterial::InitStressMaterial(int tag)
  :InitStrainBaseMaterial(tag,MAT_TAG_InitStress),
   sigInit(0.0) {}

int XC::InitStressMaterial::setInitialStrain(const double &initStrain)
  {
    InitStrainBaseMaterial::setInitialStrain(initStrain);
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; the initial strain for this material is controlled"
              << " by the initial stress. Use InitStrainMaterial instead."
              << Color::def << std::endl;
    return -1;
  }

//! @brief Assigns the initial stress.
int XC::InitStressMaterial::setInitialStress(const double &d)
  {
    int retval= -1;
    this->sigInit= d;
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	if(this->findInitialStrain() == 0)
	  {
	    tmp->commitState();
	    retval= 0;
	  }
	else
	  retval= -1;
      }
    return retval;
  }

//! @brief Return the initial stress.
double XC::InitStressMaterial::getInitialStress(void) const
  { return sigInit; }

int XC::InitStressMaterial::setTrialStrain(double strain, double strainRate)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->setTrialStrain(strain+epsInit, strainRate);
    else
      return -1;
  }

double XC::InitStressMaterial::getStrain(void) const
  { return this->getMaterial()->getStrain(); }

XC::UniaxialMaterial *XC::InitStressMaterial::getCopy(void) const
  { return new InitStressMaterial(*this); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::InitStressMaterial::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::InitStressMaterial::sendData(Communicator &comm)
  {
    int res= InitStrainBaseMaterial::sendData(comm);
    res+= comm.sendDouble(sigInit,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::InitStressMaterial::recvData(const Communicator &comm)
  {
    int res= InitStrainBaseMaterial::recvData(comm);
    res+= comm.receiveDouble(sigInit, getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::InitStressMaterial::sendSelf(Communicator &comm)
  {
    inicComm(6);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;    
    return res;
  }

int XC::InitStressMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received."
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

void XC::InitStressMaterial::Print(std::ostream &s, int flag) const
  {
    s << "InitStressMaterial tag: " << this->getTag() << std::endl;
    s << "\tMaterial: " << this->getMaterial()->getTag() << std::endl;
    s << "\tinitial strain: " << epsInit << std::endl;
    s << "\tinitial stress: " << sigInit << std::endl;
  }

int XC::InitStressMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(argv[0]=="sig0" || argv[0]=="f0" || argv[0]=="F0")
      {
        param.setValue(sigInit);
        retval= param.addObject(1, this);
      }
    else //Otherwise, pass it on to the wrapped material
      {
        UniaxialMaterial *tmp= this->getMaterial();
        if(tmp)
	  retval= tmp->setParameter(argv, param);
      }
    return retval;
  }

int XC::InitStressMaterial::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      case 1:
        this->sigInit = info.theDouble;
        this->findInitialStrain();
        break;
      }

    return 0;
  }

