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
//SectionCartesianReferenceFrame.cc

#include "SectionCartesianReferenceFrame.h"
#include "Spot.h"
#include "utility/geom/pos_vec/Vector2d.h"


//! @brief Return the i-th axis unit vector expressed in global coordinates
//! for the position being passed as parameter.
Vector2d XC::SectionCartesianReferenceFrame::getAxisVDir(const size_t &i,const Pos2d &) const
  { return ref.getAxisVDir(i); }

//! @brief Return the position of point p expressed in local coordinates
//! expresada en global coordinates.
Pos2d XC::SectionCartesianReferenceFrame::getGlobalPosition(const Pos2d &p) const
  { return ref.getGlobalPosition(p); }
//! @brief Return the vector v expressed in local coordinates
//! expressed in global coordinates.
Vector2d XC::SectionCartesianReferenceFrame::getGlobalCoordinates(const Vector2d &v) const
  { return ref.getGlobalCoordinates(v); }
//! @brief Return the position of point p expressed in global coordinates
//! expresada en local coordinates.
Pos2d XC::SectionCartesianReferenceFrame::getLocalPosition(const Pos2d &p) const
  { return ref.getLocalPosition(p); }
//! @brief Return the vector v expressed in global coordinates
//! expressed in local coordinates.
Vector2d XC::SectionCartesianReferenceFrame::getLocalCoordinates(const Vector2d &v) const
  { return ref.getLocalCoordinates(v); }
