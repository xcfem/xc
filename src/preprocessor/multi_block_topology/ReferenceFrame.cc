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
//ReferenceFrame.cc

#include "ReferenceFrame.h"
#include "utility/geom/pos_vec/Vector3d.h"


//! @brief Return the $\vec{i}$ unit vector expressed in the global coordinate
//! system for the position passed as parameter.
Vector3d XC::ReferenceFrame::getIVector(const Pos3d &p) const
  { return getAxisVDir(1,p); }

//! @brief Return the $\vec{j}$ unit vector expressed in the global coordinate
//! system for the position being passed as parameter.
Vector3d XC::ReferenceFrame::getJVector(const Pos3d &p) const
  { return getAxisVDir(2,p); }

//! @brief Return the $\vec{k}$ unit vector expressed in the global coordinate
//! system for the position being passed as parameter.
Vector3d XC::ReferenceFrame::getKVector(const Pos3d &p) const
  { return getAxisVDir(3,p); }


