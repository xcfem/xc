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
// $Revision: 1.19 $
// $Date: 2004/06/15 18:58:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/SoilMaterialBase.cpp,v $
                                                                        
// Written: ZHY

//
// SoilMaterialBase.cpp
// -------------------
//
#include <cmath>
#include <cstdlib>
#include <material/nD/soil/SoilMaterialBase.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <utility/matrix/ID.h>

#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include "material/nD/NDMaterialType.h"
#include "utility/utils/misc_utils/colormod.h"

std::vector<int> XC::SoilMaterialBase::loadStagex;
std::vector<int> XC::SoilMaterialBase::ndmx;
int XC::SoilMaterialBase::matCount = 0;

void XC::SoilMaterialBase::resizeIfNeeded(void)
  {
    const int sz= loadStagex.size();
    if(matCount>=sz)
      {
	loadStagex.resize(matCount+20);
	ndmx.resize(matCount+20);
      }
  }

XC::SoilMaterialBase::SoilMaterialBase(int tag, int classTag) 
  : NDMaterial(tag, classTag), matN(0), e2p(0)
  {
    matCount++;
    resizeIfNeeded();
    matN= matCount;
  }

//! @brief return the material stage (0:elastic 1:plastic).
int XC::SoilMaterialBase::getMaterialStage(void) const
  { return loadStagex[matN]; }

//! @brief Update material stage.
//! @param stage: stage number if stage==0 running linear elastic for soil
//!               elements, so excess pore pressure should be zero. If stage==1
//!               running plastic soil element behavior, so this marks the end
//!               of the "consol" gravity loading.
void XC::SoilMaterialBase::updateMaterialStage(int stage)
  {
    if(stage !=0 && stage !=1)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; WARNING stage must be 0 or 1"
		  << Color::def << std::endl;
        exit(-1);
      }
    loadStagex[matN]= stage;
  }

void XC::SoilMaterialBase::setMaterialStage(const int &i)
  { updateMaterialStage(i); }

void XC::SoilMaterialBase::setDimension(const int &i)
  {
    if(i !=2 && i !=3)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; FATAL dimension error: "
		  << " dimension has to be 2 or 3, you give nd= "
		  << i
		  << Color::def << std::endl;
        exit(-1);
      }
    else
      ndmx[matN]= i;
  }

const std::string &XC::SoilMaterialBase::getType(void) const
  {
    const int ndm = ndmx[matN];
    return (ndm == 2) ? strTypePlaneStrain : strTypeThreeDimensional;
  }


int XC::SoilMaterialBase::getOrder(void) const
  {
    const int ndm = ndmx[matN];
    return (ndm == 2) ? 3 : 6;
  }


//! @brief Send object members through the communicator argument.
int XC::SoilMaterialBase::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendInts(matN,e2p,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SoilMaterialBase::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveInts(matN,e2p,getDbTagData(),CommMetaData(1));
    return res;
  }



void XC::SoilMaterialBase::Print(std::ostream &s, int flag ) const
  { s << "SoilMaterialBase" << std::endl; }










