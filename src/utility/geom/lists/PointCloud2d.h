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
//PointCloud2d.h

#ifndef POINTCLOUD2D_H
#define POINTCLOUD2D_H

#include "utility/geom/lists/PolyPos.h"
#include "utility/geom/pos_vec/Pos2d.h"

class PrincipalAxes2D;

//! @ingroup GEOM
//
//! @brief Base class for position lists.
class PointCloud2d : public PolyPos<Pos2d>
  {
  public:
    PointCloud2d(void);
    explicit PointCloud2d(const std::deque<Pos2d> &);
    explicit PointCloud2d(const boost::python::list &);
    PrincipalAxes2D getPrincipalAxes(void) const;
  };

#endif






