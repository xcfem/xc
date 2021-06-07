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
//CartesianReferenceFrame3d.cc

#include "CartesianReferenceFrame3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "utility/geom/pos_vec/Vector3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Origin of the 3D cartesian coordinate system
Pos3d XC::CartesianReferenceFrame3d::getOrg(void) const
  { return ref.Org(); }

//! @brief Origin of the 3D cartesian coordinate system
void XC::CartesianReferenceFrame3d::setOrg(const Pos3d &p)
  { ref.Org()= p; }

//! @brief 3D cartesian coordinate system by three points
void XC::CartesianReferenceFrame3d::ThreePoints(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  { ref= Ref3d3d(p1,p2,p3); }

//! @brief Return the i-th axis unit vector expressed in the global coordinate
//! system for the position passed as parameter.
Vector3d XC::CartesianReferenceFrame3d::getAxisVDir(const size_t &i,const Pos3d &) const
  { return ref.getAxisVDir(i); }

//! @brief Return the global coordinates of point p. 
Pos3d XC::CartesianReferenceFrame3d::getGlobalPosition(const Pos3d &p) const
  { return ref.getGlobalPosition(p); }
//! @brief Return the global coordinates of vector v.
Vector3d XC::CartesianReferenceFrame3d::getGlobalCoordinates(const Vector3d &v) const
  { return ref.getGlobalCoordinates(v); }
//! @brief Return the local coordinates of point p.
Pos3d XC::CartesianReferenceFrame3d::getLocalPosition(const Pos3d &p) const
  { return ref.getLocalPosition(p); }
//! @brief Return the local coordinates of vector v.
Vector3d XC::CartesianReferenceFrame3d::getLocalCoordinates(const Vector3d &v) const
  { return ref.getLocalCoordinates(v); }
