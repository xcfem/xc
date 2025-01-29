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
// PointCloud3d.cc

#include "utility/geom/lists/PointCloud3d.h"
#include "utility/geom/lists/utils_list_pos3d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes3D.h"

//! @brief Constructor.
PointCloud3d::PointCloud3d(void)
  : PolyPos<Pos3d>() {}

//! @brief Constructor.
PointCloud3d::PointCloud3d(const std::deque<Pos3d> &dq_pos)
  : PolyPos<Pos3d>(dq_pos) {}

//! @brief Constructor.
PointCloud3d::PointCloud3d(const boost::python::list &l)
  : PolyPos<Pos3d>(python_to_list_pos3d(l)) {}

//! @brief Return the principal axes oriented according
//! to the distribution of the points in space.
PrincipalAxes3D PointCloud3d::getPrincipalAxes(void) const
  {
    PrincipalAxes3D retval= get_principal_axes_of_inertia(*this);
    return retval;
  }
