//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDFieldInfo

#include "MEDFieldInfo.h"
#include "MEDGaussModel.h"

#include "xc_basic/src/med_xc/MEDMEM_Group.hxx"
#include "xc_basic/src/med_xc/MEDMEM_Field.hxx"

#include "domain/mesh/element/utils/gauss_models/GaussModel.h"

//! @brief Constructor.
XC::MEDFieldInfo::MEDFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : field_info(fi) 
  { set_owner(grp); }

//! @brief Returns the grupo sobre el que está definido el campo.
const XC::MEDGroupInfo &XC::MEDFieldInfo::getGrupo(void) const
  { 
    const MEDGroupInfo *retval= dynamic_cast<const MEDGroupInfo *>(Owner());
    return *retval;
  }

MEDMEM::GROUP *XC::MEDFieldInfo::getGrupoMED(void) const
  { return getGrupo().getGrupoMED(); }
