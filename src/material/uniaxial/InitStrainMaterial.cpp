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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/InitStrainMaterial.cpp,v $

#include "InitStrainMaterial.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/MaterialResponse.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Sets the encapsulated material.
void XC::InitStrainMaterial::setMaterial(const UniaxialMaterial &material)
  {
    InitStrainBaseMaterial::setMaterial(material);
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	tmp->setTrialStrain(epsInit);
        tmp->commitState();
      }    
  }

XC::InitStrainMaterial::InitStrainMaterial(int tag, const UniaxialMaterial &material, double epsini)
  : InitStrainBaseMaterial(tag,MAT_TAG_InitStrain, material, epsini),
    localStrain(0.0)
  {
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	tmp->setTrialStrain(epsInit);
        tmp->commitState();
      }
  }

XC::InitStrainMaterial::InitStrainMaterial(int tag)
  :InitStrainBaseMaterial(tag,MAT_TAG_InitStrain),
   localStrain(0.0) {}

int XC::InitStrainMaterial::setInitialStrain(const double &initStrain)
  {
    InitStrainBaseMaterial::setInitialStrain(initStrain);
    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      {
	tmp->setTrialStrain(epsInit);
        tmp->commitState();
      }
    return 0;
  }

int XC::InitStrainMaterial::setTrialStrain(double strain, double strainRate)
  {
    localStrain = strain;

    UniaxialMaterial *tmp= this->getMaterial();
    if(tmp)
      return tmp->setTrialStrain(strain+epsInit, strainRate);
    else
      return -1;
  }

double XC::InitStrainMaterial::getStrain(void) const
  { return localStrain; }

XC::UniaxialMaterial *XC::InitStrainMaterial::getCopy(void) const
  { return new InitStrainMaterial(*this); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::InitStrainMaterial::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::InitStrainMaterial::sendData(Communicator &comm)
  {
    int res= InitStrainBaseMaterial::sendData(comm);
    res+= comm.sendDouble(localStrain,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::InitStrainMaterial::recvData(const Communicator &comm)
  {
    int res= InitStrainBaseMaterial::recvData(comm);
    res+= comm.receiveDouble(localStrain, getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::InitStrainMaterial::sendSelf(Communicator &comm)
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

int XC::InitStrainMaterial::recvSelf(const Communicator &comm)
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

XC::Response *XC::InitStrainMaterial::setResponse(const std::vector<std::string> &argv, Information &info)
  {

      Response *theResponse = 0;

      if(argv[0]=="strain")
	{ theResponse = new MaterialResponse(this, 100, 0.0); }
      else if(argv[0]=="material")
	{
	  const std::vector<std::string> newArgv(argv.begin()+1, argv.end());
	  theResponse = this->getMaterial()->setResponse(newArgv, info);
	}
      else
        theResponse= InitStrainBaseMaterial::setResponse(argv, info);
      return theResponse;
  }

int XC::InitStrainMaterial::getResponse(int responseID, Information &matInformation)
  {
    if (responseID == 100)
      return matInformation.setDouble(localStrain+epsInit);

    return InitStrainBaseMaterial::getResponse(responseID, matInformation);
  }

//! @brief Print stuff.
void XC::InitStrainMaterial::Print(std::ostream &s, int flag) const
  {
    s << "InitStrainMaterial tag: " << this->getTag()
      << std::endl
      << "\tMaterial: " << this->getMaterial()->getTag()
      << std::endl
      << "\tinitial strain: " << epsInit
      << std::endl
      << "\tlocal strain: " << localStrain
      << std::endl;
  }

int XC::InitStrainMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(argv[0]=="epsInit")
      {
        param.setValue(epsInit);
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

int XC::InitStrainMaterial::updateParameter(int parameterID, Information &info)
  {
    int retval= -1;
    if(parameterID==1)
      {
        epsInit = info.theDouble;
	UniaxialMaterial *tmp= this->getMaterial();
	if(tmp)
	  {
	    tmp->setTrialStrain(localStrain+epsInit);
	    tmp->commitState();
	    retval= 0;
	  }
      }
    return retval;
  }

