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

#include "NDAdaptorMaterial.h"
#include "utility/matrix/ID.h"
#include "material/nD/NDMaterialType.h"

void XC::NDAdaptorMaterial::free(void)
  {
    if(theMaterial)
      {
	delete theMaterial;
	theMaterial= nullptr;
      }
  }

void XC::NDAdaptorMaterial::alloc(const NDMaterial *other, const std::string &type)
  {
    free();
    if(type!="")
      theMaterial= other->getCopy(type); // Get a copy of the material
    else
      theMaterial= other->getCopy();
    if(!theMaterial)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to get copy of material.\n";
        exit(-1);
     }
  }

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int strain_size)
  : NDMaterial(0, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, int strain_size)
  : NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)
  {}

XC::NDAdaptorMaterial::NDAdaptorMaterial(int classTag, int tag, const NDMaterial &theMat, int strain_size)
  : NDMaterial(tag, classTag), Tstrain22(0.0), Cstrain22(0.0), theMaterial(nullptr), strain(strain_size)

  {
    alloc(&theMat, strTypeThreeDimensional);
  }

XC::NDAdaptorMaterial::NDAdaptorMaterial(const NDAdaptorMaterial &other)
  : NDMaterial(other), Tstrain22(other.Tstrain22), Cstrain22(other.Cstrain22), theMaterial(nullptr),
    strain(other.strain)
  {
    alloc(other.theMaterial, "");
  }

XC::NDAdaptorMaterial &XC::NDAdaptorMaterial::operator=(const NDAdaptorMaterial &other)
  {
    NDMaterial::operator=(other);
    Tstrain22= other.Tstrain22;
    Cstrain22= other.Cstrain22;
    alloc(other.theMaterial, "");
    strain= other.strain;
    return *this;
  }

XC::NDAdaptorMaterial::~NDAdaptorMaterial(void) 
  { free(); } 


//! @brief Commit the state of the material.
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

//! @brief Revert the material to its initial state.
int XC::NDAdaptorMaterial::revertToStart()
  {
    int retval= NDMaterial::revertToStart();
    Tstrain22 = 0.0;
    Cstrain22 = 0.0;
    strain.Zero();
    retval= theMaterial->revertToStart();
    return retval;
  }

double XC::NDAdaptorMaterial::getRho(void) const
  { return theMaterial->getRho(); }

const XC::Vector& XC::NDAdaptorMaterial::getStrain(void)
  { return strain; }

//! @brief Send object members through the communicator argument.
int XC::NDAdaptorMaterial::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(strain,getDbTagData(),CommMetaData(2));
    res+= comm.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::NDAdaptorMaterial::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveDoubles(Tstrain22,Cstrain22,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(strain,getDbTagData(),CommMetaData(2));
    if(theMaterial) delete theMaterial;
    theMaterial= comm.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(3,4,5));
    return res;
  }

void XC::NDAdaptorMaterial::Print(std::ostream &s, int flag) const
  {
	s << "NDAdaptorMaterial, tag: " << this->getTag() << std::endl;
	s << "\tWrapped material: "<< theMaterial->getTag() << std::endl;

	theMaterial->Print(s, flag);
  }

XC::Response *XC::NDAdaptorMaterial::setResponse(const std::vector<std::string> &argv, Information &matInformation)
  {
    // for strain, stress and tangent use the base class implementation
    // so that the output will be that of the adapter
    if((argv[0]=="Tangent") ||
        (argv[0]=="tangent") ||
        (argv[0]=="stress") ||
        (argv[0]=="stresses") ||
        (argv[0]=="strain") ||
        (argv[0]=="strains") )
      { return NDMaterial::setResponse(argv, matInformation); }
    // otherwise, for other custom results, forward the call to the adaptee
    return theMaterial->setResponse(argv, matInformation);
  }
