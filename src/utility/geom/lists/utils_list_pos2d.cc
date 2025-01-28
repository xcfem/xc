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
//utils_list_pos2d.cc

#include "utils_list_pos2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include <plotter.h>
#include "utility/geom/trf/Trf2d.h"
#include "utility/geom/d2/BND2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes2D.h"
#include <eigen3/Eigen/Dense>
#include "utility/utils/misc_utils/matem.h"

void plot(Plotter &plt,const GeomObj::list_Pos2d &l)
  {
    for(GeomObj::list_Pos2d::const_iterator i= l.begin();i!=l.end();i++)
      (*i).Plot(plt);
  }


//! @brief Moves the points of the list.
void move(GeomObj::list_Pos2d &l,const Vector2d &v)
  {
    for(GeomObj::list_Pos2d::iterator j=l.begin();j != l.end();j++)
      (*j)= (*j) + v;
  }

//! @brief Applies the transformation to the vertices.
void transform(GeomObj::list_Pos2d &l,const Trf2d &t)
  { t.Transform(l.begin(),l.end()); }

//! @brief Return the upper right corner of the bounding rectangle. 
Pos2d getPMax(const GeomObj::list_Pos2d &l)
  { return Pos2d(l.GetMax(1),l.GetMax(2)); }

//! @brief Return the lower left corner of the bounding rectangle. 
Pos2d getPMin(const GeomObj::list_Pos2d &l)
  { return Pos2d(l.GetMin(1),l.GetMin(2)); }

//! @brief Return the bounding rectangle. 
BND2d getBnd(const GeomObj::list_Pos2d &l)
  { return BND2d(getPMin(l),getPMax(l)); }

//! @brief Compute the cover of the positions in the polygon.
std::deque<GEOM_FT> &getCovers(const GeomObj::list_Pos2d &l,const Polygon2d &plg)
  {
    const size_t sz= l.size();
    static std::deque<GEOM_FT> retval;
    retval.resize(sz);
    for(size_t i= 0;i<sz;i++)
      retval[i]= plg.getCover(l[i]);
    return retval;
  }

//! @brief Construct a Point2d list from the given Python list.
GeomObj::list_Pos2d python_to_list_pos2d(const boost::python::list &l)
  {
    GeomObj2d::list_Pos2d retval;
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      retval.push_back(boost::python::extract<Pos2d>(l[i]));
    return retval;
  }

//! @brief Return the principal axes of inertia of the point cloud.
PrincipalAxes2D get_principal_axes_of_inertia(const GeomObj::list_Pos2d &l)
  {
    const size_t sz= l.size();
    PrincipalAxes2D retval;
    if(sz>0)
      {
	Eigen::MatrixXd data(sz, 2);
	//Compute the covariance matrix of the point cloud.
	size_t row= 0;
	for(GeomObj::list_Pos2d::const_iterator i=l.begin(); i!=l.end(); i++, row++)
	  {
	    const Pos2d &p= (*i);
	    data(row,0)= p.x();
	    data(row,1)= p.y();
	  }
	const Pos2d centroid= l.getCenterOfMass();
	Eigen::MatrixXd covariance_matrix= compute_covariance_matrix(data);
	double A[2][2];
	A[0][0]= covariance_matrix(0,0); A[0][1]= covariance_matrix(0,1);
	A[1][0]= covariance_matrix(0,1); A[1][1]= covariance_matrix(1,1);
	
	retval= PrincipalAxes2D(centroid, A);
      }    
    return retval;
  }
