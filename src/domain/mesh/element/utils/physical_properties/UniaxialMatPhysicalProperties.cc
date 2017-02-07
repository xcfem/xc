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
//UniaxialMatPhysicalProperties.cpp

#include "UniaxialMatPhysicalProperties.h"
#include <material/uniaxial/UniaxialMaterial.h>

//! @brief Constructor
XC::UniaxialMatPhysicalProperties::UniaxialMatPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : PhysicalProperties<UniaxialMaterial>(nMat,ptr_mat)
  { }

//! @brief Constructor
XC::UniaxialMatPhysicalProperties::UniaxialMatPhysicalProperties(const std::vector<UniaxialMaterial *> &ptr_mats)
  : PhysicalProperties<UniaxialMaterial>(ptr_mats.size(),nullptr)
  {
    const size_t sz= ptr_mats.size();
    for(size_t i= 0;i<sz;i++)
      theMaterial.setMaterial(i,ptr_mats[i]->getCopy());
  }

XC::Vector XC::UniaxialMatPhysicalProperties::getStresses(void) const
  {
    const size_t sz= size();
    Vector retval(sz);
    for(size_t i=0;i<sz;i++)
      if(theMaterial[i]!=nullptr)
        retval[i]= theMaterial[i]->getStress();
    return retval;
  }

//! @brief print out element data
void XC::UniaxialMatPhysicalProperties::Print(std::ostream &s)
  {
    std::cerr << "UniaxialMatPhysicalProperties\t" << theMaterial.size() << std::endl;
  }

