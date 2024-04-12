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
//NDMaterialPhysicalProperties.cc

#include "Contact2D.h"
#include "material/nD/uw_materials/ContactMaterial2D.h"

//! @brief Constructor.
XC::Contact2D::Contact2D(const size_t &nMat,const ContactMaterial2D *ptr_mat)
  :NDMaterialPhysicalProperties(nMat,ptr_mat)
  {}

//! @brief Constructor.
XC::Contact2D::Contact2D(const size_t &nMat, ContactMaterial2D &theMat)
  : NDMaterialPhysicalProperties(nMat,&theMat)
  {}


const XC::ContactMaterial2D *XC::Contact2D::getContactMaterial(const int &i) const
  {
    const ContactMaterial2D *retval= nullptr;
    if(this->size()>0)
      retval= dynamic_cast< const ContactMaterial2D *>(theMaterial[i]);
    return retval;
  }


XC::ContactMaterial2D *XC::Contact2D::getContactMaterial(const int &i)
  {
    ContactMaterial2D *retval= nullptr;
    if(this->size()>0)
      retval= dynamic_cast<ContactMaterial2D *>(theMaterial[i]);
    return retval;
  }
