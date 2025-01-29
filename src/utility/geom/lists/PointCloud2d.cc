//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
// PointCloud2d.cc

#include "utility/geom/lists/PointCloud2d.h"
#include "utility/geom/lists/utils_list_pos2d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes2D.h"

//! @brief Constructor.
PointCloud2d::PointCloud2d(void)
  : PolyPos<Pos2d>() {}

//! @brief Constructor.
PointCloud2d::PointCloud2d(const std::deque<Pos2d> &dq_pos)
  : PolyPos<Pos2d>(dq_pos) {}

//! @brief Constructor.
PointCloud2d::PointCloud2d(const boost::python::list &l)
  : PolyPos<Pos2d>(python_to_list_pos2d(l)) {}

//! @brief Return the principal axes oriented according
//! to the distribution of the points in space.
PrincipalAxes2D PointCloud2d::getPrincipalAxes(void) const
  {
    PrincipalAxes2D retval= get_principal_axes_of_inertia(*this);
    return retval;
  }
