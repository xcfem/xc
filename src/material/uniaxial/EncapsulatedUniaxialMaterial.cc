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

#include "EncapsulatedUniaxialMaterial.h"
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"

//! @brief Sets the encapsulated material.
void XC::EncapsulatedUniaxialMaterial::setMaterial(const UniaxialMaterial &material)
  { theMaterial.setMaterial(material); }

//! @brief Sets the encapsulated material.
void XC::EncapsulatedUniaxialMaterial::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= getMaterialByName(matName);
    if(ptr_mat)
      {
	const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
	if(tmp)
	  theMaterial.setMaterial(*tmp);
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		    << "material identified by: '" << matName
		    << "' is not an uniaxial material." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
		<< "material identified by: '" << matName
		<< "' not found." << std::endl;
  }

//! @brief Constructor.
XC::EncapsulatedUniaxialMaterial::EncapsulatedUniaxialMaterial(int tag, int classTag, const UniaxialMaterial &material)
  :UniaxialMaterial(tag,classTag), theMaterial(material)
  {}

//! @brief Constructor.
XC::EncapsulatedUniaxialMaterial::EncapsulatedUniaxialMaterial(int tag, int classTag)
  : UniaxialMaterial(tag,classTag), theMaterial() {}

double XC::EncapsulatedUniaxialMaterial::getStrain(void) const
  { return theMaterial.getStrain(); }

double XC::EncapsulatedUniaxialMaterial::getStrainRate(void) const
  { return theMaterial.getStrainRate(); }

int XC::EncapsulatedUniaxialMaterial::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    int res= comm.sendMovable(theMaterial, getDbTagData(), CommMetaData(1));
    return res;
  }

int XC::EncapsulatedUniaxialMaterial::recvData(const Communicator &comm)
  {
    int res= UniaxialMaterial::recvData(comm);
    res+= comm.receiveMovable(theMaterial, getDbTagData(),CommMetaData(1));
    return res;
  }
