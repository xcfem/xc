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
                                                                        
// $Revision: 1.13 $
// $Date: 2006/01/10 19:15:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/UniaxialMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/UniaxialMaterial.C
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class implementation for 
// UniaxialMaterial.
//
// What: "@(#) UniaxialMaterial.C, revA"

#include <material/uniaxial/UniaxialMaterial.h>
#include <cstring>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <cfloat>
#include <utility/matrix/Vector.h>

#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor.
//!
//! @param tag: object identifier (unique among materials).
//! @param classTag: class identifier (for persistence).
XC::UniaxialMaterial::UniaxialMaterial(int tag, int classTag)
  :Material(tag,classTag), rho(0.0) {}

int XC::UniaxialMaterial::setTrial(double strain, double &stress, double &tangent, double strainRate)
  {
    int res = this->setTrialStrain(strain, strainRate);
    if(res == 0)
      {
        stress = this->getStress();
        tangent = this->getTangent();
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; material failed in setTrialStrain().\n"; 
      }
    return res;
  }

//! @brief Return the initial strain.
double XC::UniaxialMaterial::getInitialStrain(void) const
  { return 0.0; }

//! @brief default operation for strain rate is zero
double XC::UniaxialMaterial::getStrainRate(void) const
  { return 0.0; }


//! @brief Return the generalized stress.
const XC::Vector &XC::UniaxialMaterial::getGeneralizedStress(void) const
  {
    static Vector retval(1);
    retval(0)= getStress();
    return retval;
  }

//! @brief Return the generalized strain.
const XC::Vector &XC::UniaxialMaterial::getGeneralizedStrain(void) const
  {
    static Vector retval(1);
    retval(0)= getStrain();
    return retval;
  }

const XC::Vector &XC::UniaxialMaterial::getInitialGeneralizedStrain(void) const
  {
    static Vector retval(1);
    retval(0)= getInitialStrain();
    return retval;
  }

//! @brief default operation for damping tangent is zero
double XC::UniaxialMaterial::getDampTangent(void) const
  { return 0.0; }

//! @brief Return secant secant stiffness of the material.
double XC::UniaxialMaterial::getSecant(void) const
  {
    double strain = this->getStrain();
    double stress = this->getStress();

    if(strain != 0.0)
      return stress/strain;
    else
      return this->getTangent();
  }

//! @brief Return the inverse of stiffness matrix.
double XC::UniaxialMaterial::getFlexibility(void) const
  {
    double retval;
    const double k= getTangent();
    if(k == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; singular material stiffness.\n";
	retval= 1.e14;
      }
    else
      retval= 1/k;
    return retval;
  }

//! @brief Return the inverse of stiffness matrix inicial.
double XC::UniaxialMaterial::getInitialFlexibility(void) const
  {
    double retval;
    const double k= getInitialTangent();
    if(k == 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; singular material stiffness.\n";
	retval= 1.e14;
      }
    else
      retval= 1/k;
    return retval;
  }

//! @brief Return the material density.
double XC::UniaxialMaterial::getRho(void) const
  { return rho; }

//! @brief Assigns la material density.
void XC::UniaxialMaterial::setRho(const double &r)
  { rho= r; }

//! @brief Virtual constructor.
XC::UniaxialMaterial* XC::UniaxialMaterial::getCopy(SectionForceDeformation *s) const
  { return getCopy(); }

//! @brief Sets the initial strain value.
int XC::UniaxialMaterial::setInitialStrain(double strain)
  {
    if(strain!=0.0)
      std::clog << "Material: " << getClassName() 
                << " can't handle initial strains." << std::endl;
    return 0;
  }

//! @brief Sets the initial generalized strain to the value being passed as parameter.
void XC::UniaxialMaterial::setInitialGeneralizedStrain(const Vector &iS)
  { setInitialStrain(iS[0]); }

XC::Response *XC::UniaxialMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    const size_t argc= argv.size();
    if(argc == 0) return 0;

    // stress
    if(argv[0] == "stress")
      return new MaterialResponse(this, 1, this->getStress());
    // tangent
    else if (argv[0] == "tangent")
      return new MaterialResponse(this, 2, this->getTangent());
    // strain
    else if (argv[0] == "strain")
      return new MaterialResponse(this, 3, this->getStrain());
    // strain
    else if ((argv[0] == "stressStrain") || 
             (argv[0] == "stressANDstrain"))
      { return new MaterialResponse(this, 4, XC::Vector(2)); }
    // otherwise unknown
    else
      { return nullptr; }
  }

int XC::UniaxialMaterial::getResponse(int responseID, Information &matInfo)
  {
    static XC::Vector stressStrain(2);
    // each subclass must implement its own stuff    
    switch(responseID)
      {
      case 1:
        matInfo.setDouble(this->getStress());
        return 0;
      case 2:
        matInfo.setDouble(this->getTangent());
        return 0;      
      case 3:
        matInfo.setDouble(this->getStrain());
        return 0;      
      case 4:
        stressStrain(0) = this->getStress();
        stressStrain(1) = this->getStrain();
        matInfo.setVector(stressStrain);
        return 0;
      default:      
        return -1;
      }
  }


// AddingSensitivity:BEGIN ////////////////////////////////////////
int XC::UniaxialMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::UniaxialMaterial::updateParameter(int parameterID, Information &info)
  { return -1; }

int XC::UniaxialMaterial::activateParameter(int parameterID)
  { return -1; }

double XC::UniaxialMaterial::getStressSensitivity(int gradNumber, bool conditional)
  { return 0.0; }

double XC::UniaxialMaterial::getStrainSensitivity(int gradNumber)
  { return 0.0; }

double XC::UniaxialMaterial::getInitialTangentSensitivity(int gradNumber)
  { return 0.0; }

double XC::UniaxialMaterial::getRhoSensitivity(int gradNumber)
  { return 0.0; }

double XC::UniaxialMaterial::getDampTangentSensitivity(int gradNumber)
  { return 0.0; }

int XC::UniaxialMaterial::commitSensitivity(double strainSensitivity, int gradNumber, int numGrads)
  { return -1; }

double XC::UniaxialMaterial::getInitialTangent(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this method " << std::endl
              << " is not implemented for the selected material. "
	      << std::endl;
    return 0.0;
  }

// AddingSensitivity:END //////////////////////////////////////////

//! @brief Send object members through the communicator argument.
int XC::UniaxialMaterial::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    int res= comm.sendDouble(rho,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::UniaxialMaterial::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(0));
    int res= comm.receiveDouble(rho,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives a pointer to uniaxial material through the communicator argument.
XC::UniaxialMaterial *XC::receiveUniaxialMaterialPtr(UniaxialMaterial *ptr,DbTagData &dt,const Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    UniaxialMaterial *retval= nullptr;
    Material *tmp= nullptr;
    tmp= receiveMaterialPtr(ptr,dt,comm,md);
    if(tmp)
      {
        retval= dynamic_cast<UniaxialMaterial *>(tmp);
        if(!retval)
          {
            std::cerr << __FUNCTION__
		      <<"; WARNING failed to get material."
		      << std::endl;
            delete tmp;
          }
      }
    return retval;
  }
