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

#include "NDMaterialWrapper.h"

//! @brief Default constructor.
XC::NDMaterialWrapper::NDMaterialWrapper(void)
  : MaterialWrapper<NDMaterial, MAT_TAG_NDMaterialWrapper>() {}

//! @brief Constructor.
XC::NDMaterialWrapper::NDMaterialWrapper(const NDMaterial &material)
  : MaterialWrapper<NDMaterial, MAT_TAG_NDMaterialWrapper>(material)
  {}

double XC::NDMaterialWrapper::getRho(void) const
// this function gets the mass density from the main material
  {
    return theMaterial->getRho();
  }

const XC::Vector &XC::NDMaterialWrapper::getStrain(void) const
// this function sends the strain back to the element
  { return theMaterial->getStrain(); }

const XC::Vector &XC::NDMaterialWrapper::getStress(void) const
// this function sends the stress back to the element
  {
    return theMaterial->getStress();
  }

int XC::NDMaterialWrapper::setTrialStrain(const Vector &strain_from_element)
// this function receives the strain from the element and sends strain to material
  {
    return theMaterial->setTrialStrain(strain_from_element);
  }

//! brief this function sends the tangent back to the element
const XC::Matrix &XC::NDMaterialWrapper::getTangent(void) const
  { return theMaterial->getTangent(); }

const XC::Matrix &XC::NDMaterialWrapper::getInitialTangent(void) const
// this function sends the initial tangent back to the element
  { return theMaterial->getInitialTangent(); }

const std::string &XC::NDMaterialWrapper::getType(void) const
  { return theMaterial->getType(); }
int XC::NDMaterialWrapper::getOrder(void) const
  { return theMaterial->getOrder(); }

int XC::NDMaterialWrapper::commitState(void)
  {
    return theMaterial->commitState();
  }

int XC::NDMaterialWrapper::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

int XC::NDMaterialWrapper::revertToStart(void)
  {
    return theMaterial->revertToStart();
  }
  
XC::Response *XC::NDMaterialWrapper::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    return theMaterial->setResponse(argv, info);
  }

int XC::NDMaterialWrapper::getResponse(int responseID, Information &matInfo)
  {
    return theMaterial->getResponse(responseID, matInfo);
  }
