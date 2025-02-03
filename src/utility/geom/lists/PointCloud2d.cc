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
#include "utility/geom/coo_sys/ref_sys/Ref2d2d.h"
#include "utility/utils/misc_utils/colormod.h"
#include "utility/geom/d2/2d_polygons/Quadrilateral2d.h"

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

//! @brief Return the oriented bounding box that contains
//! all the points in the cloud and whose edges are parallel
//! to the given reference.
Quadrilateral2d PointCloud2d::getOrientedBoundingBox(const Ref2d2d &ref) const
  {
    Quadrilateral2d retval;
    if(!this->empty())
      {	  
	// Initialize maximum and minimum relative coordinates.
        Pos2d p= ref.getLocalPosition((*this)[0]);
	double x_min= p.x();
	double x_max= x_min;
	double y_min= p.y();
	double y_max= y_min;
	const size_t sz= this->size();
	for(size_t i= 1; i<sz; i++)
	  {
	    p= ref.getLocalPosition((*this)[i]);
	    x_min= std::min(p.x(), x_min);
	    x_max= std::max(p.x(), x_max);
	    y_min= std::min(p.y(), y_min);
	    y_max= std::max(p.y(), y_max);
	  }
	const GEOM_FT length= x_max-x_min;
	const GEOM_FT height= y_max-y_min;
	// const Vector2d i= Vector2d(1,0);
	// const Vector2d j= Vector2d(0,1);
	const Pos2d p1_local= Pos2d(x_min, y_min);
	const Pos2d p2_local= Pos2d(x_min+length, y_min); // p1_local+length* Vector2d(1,0);
	const Pos2d p3_local= Pos2d(p2_local.x(), p2_local.y()+height); //p2_local+height*Vector2d(0,1);
	const Pos2d p4_local= Pos2d(x_min, y_min+height); // p1_local+height**Vector2d(0,1);
	retval= Quadrilateral2d(ref.getGlobalPosition(p1_local),
				ref.getGlobalPosition(p2_local),
				ref.getGlobalPosition(p3_local),
				ref.getGlobalPosition(p4_local));
      }
    else
      std::cerr << Color::red << "PointCloud2d::" << __FUNCTION__
	        << "; point cloud is empty the returned bounding box has no meaning."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Return the oriented bounding box that contains
//! all the points in the cloud.
Quadrilateral2d PointCloud2d::getOrientedBoundingBox(void) const
  {
    Quadrilateral2d retval;
    if(!this->empty())
      {	  
	// Get principal axes reference system.
	PrincipalAxes2D p_axis= this->getPrincipalAxes();
	const Ref2d2d ref= p_axis.getAxis();
	retval= this->getOrientedBoundingBox(ref);
      }
    else
      std::cerr << Color::red << "PointCloud2d::" << __FUNCTION__
	        << "; point cloud is empty the returned bounding box has no meaning."
	        << Color::def << std::endl;
    return retval;
  }
