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
//SisRef.cc

#include "SisRef.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"


//! @brief Returns the $\vec{i}$ unit vector expressed in global coordinates
//! for the position being passed as parameter.
Vector3d XC::SisRef::GetI(const Pos3d &p) const
  { return GetVDirEje(1,p); }

//! @brief Returns the $\vec{j}$ unit vector expressed in global coordinates
//! for the position being passed as parameter.
Vector3d XC::SisRef::GetJ(const Pos3d &p) const
  { return GetVDirEje(2,p); }

//! @brief Returns the $\vec{k}$ unit vector expressed in global coordinates
//! for the position being passed as parameter.
Vector3d XC::SisRef::GetK(const Pos3d &p) const
  { return GetVDirEje(3,p); }


