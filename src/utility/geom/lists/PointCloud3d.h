// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//PointCloud3d.h

#ifndef POINTCLOUD3D_H
#define POINTCLOUD3D_H

#include "utility/geom/lists/PolyPos.h"
#include "utility/geom/pos_vec/Pos3d.h"

class PrincipalAxes3D;

//! @ingroup GEOM
//
//! @brief Base class for position lists.
class PointCloud3d : public PolyPos<Pos3d>
  {
  public:
    PointCloud3d(void);
    explicit PointCloud3d(const std::deque<Pos3d> &);
    explicit PointCloud3d(const boost::python::list &);
    PrincipalAxes3D getPrincipalAxes(void) const;
  };

#endif






