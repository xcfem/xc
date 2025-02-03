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
#include "utility/geom/coo_sys/ref_sys/Ref3d3d.h"
#include "utility/utils/misc_utils/colormod.h"
#include "utility/geom/d3/3d_polyhedrons/Hexahedron3d.h"

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

//! @brief Return the oriented bounding box that contains
//! all the points in the cloud and whose edges are parallel
//! to the given reference.
Hexahedron3d PointCloud3d::getOrientedBoundingBox(const Ref3d3d &ref) const
  {
    Hexahedron3d retval;
    if(!this->empty())
      {	  
	// Initialize maximum and minimum relative coordinates.
        Pos3d p= ref.getLocalPosition((*this)[0]);
	double x_min= p.x();
	double x_max= x_min;
	double y_min= p.y();
	double y_max= y_min;
	double z_min= p.z();
	double z_max= z_min;
	const size_t sz= this->size();
	for(size_t i= 1; i<sz; i++)
	  {
	    p= ref.getLocalPosition((*this)[i]);
	    x_min= std::min(p.x(), x_min);
	    x_max= std::max(p.x(), x_max);
	    y_min= std::min(p.y(), y_min);
	    y_max= std::max(p.y(), y_max);
	    z_min= std::min(p.z(), z_min);
	    z_max= std::max(p.z(), z_max);
	  }
	const GEOM_FT length= x_max-x_min;
	const GEOM_FT width= y_max-y_min;
	const GEOM_FT height= z_max-z_min;
	// const Vector3d i= Vector3d(1,0,0);
	// const Vector3d j= Vector3d(0,1,0);
	// const Vector3d k= Vector3d(0,0,1);
	
	const Pos3d p0_local= Pos3d(x_min, y_min, z_min);
	const Pos3d p1_local= Pos3d(p0_local.x()+length, p0_local.y(), p0_local.z()); // p0_local+length*Vector3d(1,0,0);
	const Pos3d p2_local= Pos3d(p1_local.x(), p1_local.y()+width, p1_local.z()); // p1_local+width*Vector3d(0,1,0);
	const Pos3d p3_local= Pos3d(p0_local.x(), p0_local.y()+width, p0_local.z()); // p0_local+width*Vector3d(0,1,0);
	
	const Pos3d p4_local= Pos3d(p0_local.x(), p0_local.y(), p0_local.z()+height); // p0_local+height*Vector3d(0,0,1);
	const Pos3d p5_local= Pos3d(p1_local.x(), p1_local.y(), p1_local.z()+height); // p1_local+height*Vector3d(0,0,1);
	const Pos3d p6_local= Pos3d(p2_local.x(), p2_local.y(), p2_local.z()+height); // p2_local+height*Vector3d(0,0,1);
	const Pos3d p7_local= Pos3d(p3_local.x(), p3_local.y(), p3_local.z()+height); // p3_local+height*Vector3d(0,0,1);
	retval= Hexahedron3d(ref.getGlobalPosition(p0_local),
			     ref.getGlobalPosition(p1_local),
			     ref.getGlobalPosition(p2_local),
			     ref.getGlobalPosition(p3_local),
			     ref.getGlobalPosition(p4_local),
			     ref.getGlobalPosition(p5_local),
			     ref.getGlobalPosition(p6_local),
			     ref.getGlobalPosition(p7_local));
      }
    else
      std::cerr << Color::red << "PointCloud3d::" << __FUNCTION__
	        << "; point cloud is empty the returned bounding box has no meaning."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Return the oriented bounding box that contains
//! all the points in the cloud.
Hexahedron3d PointCloud3d::getOrientedBoundingBox(void) const
  {
    Hexahedron3d retval;
    if(!this->empty())
      {	  
	// Get principal axes reference system.
	PrincipalAxes3D p_axis= this->getPrincipalAxes();
	const Ref3d3d ref= p_axis.getAxis();
	retval= this->getOrientedBoundingBox(ref);
      }
    else
      std::cerr << Color::red << "PointCloud3d::" << __FUNCTION__
	        << "; point cloud is empty the returned bounding box has no meaning."
	        << Color::def << std::endl;
    return retval;
  }
