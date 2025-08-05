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
//DiscretBase.cpp

#include "material/section/repres/DiscretBase.h" 
#include "material/Material.h" 

//! @brief Constructor.
XC::DiscretBase::DiscretBase(Material *m)
  : mat(m) {}

//! @brief Set the material.
void XC::DiscretBase::setMaterialPtr(Material *m)
  { mat = m; }

//! @brief Get a pointer to the material.
XC::Material *XC::DiscretBase::getMaterialPtr(void) const
  { return mat; }

//! @brief Set the material pointer to null.
void XC::DiscretBase::clear(void)
  {
    mat= nullptr;
    NamedEntity::clearPyProps();
  }

