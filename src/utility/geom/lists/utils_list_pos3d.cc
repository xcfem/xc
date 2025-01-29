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
//utils_list_pos3d.cc

#include "utils_list_pos3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/trf/Trf3d.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes3D.h"
#include <eigen3/Eigen/Dense>
#include "utility/utils/misc_utils/matem.h"


//! @brief Moves the points of the list.
void move(GeomObj::list_Pos3d &l,const Vector3d &v)
  {
    for(GeomObj::list_Pos3d::iterator j=l.begin();j != l.end();j++)
      (*j)= (*j) + v;
  }

//! @brief Applies the transformation to the positions.
void transform(GeomObj::list_Pos3d &l,const Trf3d &t)
  { t.Transform(l.begin(),l.end()); }

//! @brief Return the upper right corner of the bounding rectangle.
Pos3d getPMax(const GeomObj::list_Pos3d &l)
  { return Pos3d(l.GetMax(1),l.GetMax(2),l.GetMax(3)); }

//! @brief Return the lower left cornerof the bounding rectangle.
Pos3d getPMin(const GeomObj::list_Pos3d &l)
  { return Pos3d(l.GetMin(1),l.GetMin(2),l.GetMin(3)); }

//! @brief Return the bounding rectangle. 
BND3d getBnd(const GeomObj::list_Pos3d &l)
  { return BND3d(getPMin(l),getPMax(l)); }

//! @brief Construct a Point3d list from the given Python list.
GeomObj::list_Pos3d python_to_list_pos3d(const boost::python::list &l)
  {
    GeomObj3d::list_Pos3d retval;
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      retval.push_back(boost::python::extract<Pos3d>(l[i]));
    return retval;
  }

//! @brief Return the principal axes of inertia of the point cloud.
PrincipalAxes3D get_principal_axes_of_inertia(const GeomObj::list_Pos3d &l)
  {
    const size_t sz= l.size();
    PrincipalAxes3D retval;
    if(sz>0)
      {
	Eigen::MatrixXd data(sz, 3);
	//Compute the covariance matrix of the point cloud.
	size_t row= 0;
	for(GeomObj::list_Pos3d::const_iterator i=l.begin(); i!=l.end(); i++, row++)
	  {
	    const Pos3d &p= (*i);
	    data(row,0)= p.x();
	    data(row,1)= p.y();
	    data(row,2)= p.z();
	  }
	const Pos3d centroid= l.getCenterOfMass();
	Eigen::MatrixXd covariance_matrix= compute_covariance_matrix(data);
	double A[3][3];
	A[0][0]= covariance_matrix(0,0); A[0][1]= covariance_matrix(0,1); A[0][2]= covariance_matrix(0,2);
	A[1][0]= covariance_matrix(0,1); A[1][1]= covariance_matrix(1,1); A[1][2]= covariance_matrix(1,2);
	A[2][0]= covariance_matrix(0,2); A[2][1]= covariance_matrix(1,2); A[2][2]= covariance_matrix(2,2);
	
	retval= PrincipalAxes3D(centroid, A);
      }    
    return retval;
  }
