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
//MeshComponentContainer.cpp


#include "MeshComponentContainer.h"
#include "domain/domain/Domain.h"


//! @brief Constructor.
XC::MeshComponentContainer::MeshComponentContainer(EntCmd *owr,int classTag, int dbTag)
  :EntCmd(owr), MovableObject(classTag, dbTag) {}

//! @brief Returns a pointer to the mesh domain.
const XC::Domain *XC::MeshComponentContainer::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<const Domain *>(owr);
    return retval;
  }

//! @brief Returns a pointer to the mesh domain.
XC::Domain *XC::MeshComponentContainer::getDomain(void)
  {
    Domain *retval= nullptr;
    EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<Domain *>(owr);
    return retval;
  }

