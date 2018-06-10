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
//NDAdaptorMaterial.cc

#include <material/nD/NDAdaptorMaterial.h>
#include "utility/matrix/ID.h"
#include <material/nD/NDMaterialType.h>


XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int strain_size)
  : NDMaterial(0, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, int strain_size)
  : NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, XC::NDMaterial &theMat, int strain_size)
: XC::NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)

  {
    // Get a copy of the material
    theMaterial = theMat.getCopy(strTypeThreeDimensional);
  
    if(!theMaterial)
      {
        std::cerr << "NDAdaptorMaterial::NDAdaptorMaterial -- failed to get copy of material\n";
        exit(-1);
     }
  }

XC::NDAdaptorMaterial::~NDAdaptorMaterial(void) 
  { if(theMaterial) delete theMaterial; } 


int XC::NDAdaptorMaterial::commitState(void)
  {
    Cstrain22 = Tstrain22;
    return theMaterial->commitState();
  }

int XC::NDAdaptorMaterial::revertToLastCommit(void)
  {
    Tstrain22 = Cstrain22;
    return theMaterial->revertToLastCommit();
  }

int XC::NDAdaptorMaterial::revertToStart()
  {
    Tstrain22 = 0.0;
    Cstrain22 = 0.0;
    strain.Zero();
    return theMaterial->revertToStart();
  }

double XC::NDAdaptorMaterial::getRho(void) const
  { return theMaterial->getRho(); }

const XC::Vector& XC::NDAdaptorMaterial::getStrain(void)
  { return strain; }

//! @brief Send object members through the channel being passed as parameter.
int XC::NDAdaptorMaterial::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(strain,getDbTagData(),CommMetaData(2));
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::NDAdaptorMaterial::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(strain,getDbTagData(),CommMetaData(2));
    if(theMaterial) delete theMaterial;
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

void XC::NDAdaptorMaterial::Print(std::ostream &s, int flag)
  {
	s << "NDAdaptorMaterial, tag: " << this->getTag() << std::endl;
	s << "\tWrapped material: "<< theMaterial->getTag() << std::endl;

	theMaterial->Print(s, flag);
  }

