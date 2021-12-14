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
//Pos3dArray.cc

#include "Pos3dArray.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d2/Triangle3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d3/BND3d.h"

#include "utility/geom/trf/Trf3d.h"
#include "utility/geom/trf/Revolution3d.h"

//! @brief Constructor.
Pos3dArray::Pos3dArray(const PosArray<Pos3d> &mp3d)
  : PosArray<Pos3d>(mp3d) {}
//! @brief Constructor.
Pos3dArray::Pos3dArray(const size_t &f,const size_t &c,const Pos3d &p)
  : PosArray<Pos3d>(f,c,p) {}
//! @brief Constructor.
Pos3dArray::Pos3dArray(const Pos3d &p1,const Pos3d &p2,const size_t &ndiv)
  : PosArray<Pos3d>(p1,p2,ndiv) {}

//! @brief Ver el constructor correspondiente en PosArray<POS>.
Pos3dArray::Pos3dArray(const Pos3d &p1,const Pos3d &p2,const std::vector<GEOM_FT> &longs)
  : PosArray<Pos3d>(p1,p2,longs) {}

//! @brief Constructor.
Pos3dArray::Pos3dArray(const Pos3d &p1,const Pos3d &p2,const size_t &num,const GEOM_FT &ratio)
  : PosArray<Pos3d>(p1,p2,num,ratio) {}
//! @brief Constructor.
Pos3dArray::Pos3dArray(const Pos3d &p0,const Pos3d &p1,const Pos3d &p2,const size_t &ndiv1,const size_t &ndiv2)
  : PosArray<Pos3d>(p0,p1,p2,ndiv1,ndiv2) {}

Pos3dArray::Pos3dArray(const Pos3dArray &l1_points,const Pos3dArray &l2_points,const Pos3dArray &l3_points,const Pos3dArray &l4_points)
  : PosArray<Pos3d>(l1_points,l2_points,l3_points,l4_points) {}

Pos3dArray::Pos3dArray(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4,const size_t &ndiv1,const size_t &ndiv2)
  : PosArray<Pos3d>(Quadrilateral<Pos3d>(p1,p2,p3,p4,ndiv1,ndiv2)) {}


Pos3d Pos3dArray::getCenter(void) const
  { return get_centro(*this,Segment3d()); }

Pos3d Pos3dArray::pos_lagrangiana(const size_t &i,const size_t &j) const
  {
    assert(interior(i,j));
    const GEOM_FT x= ((*this)(i-1,j).x()+(*this)(i+1,j).x()+(*this)(i,j-1).x()+(*this)(i,j+1).x())/GEOM_FT(4);
    const GEOM_FT y= ((*this)(i-1,j).y()+(*this)(i+1,j).y()+(*this)(i,j-1).y()+(*this)(i,j+1).y())/GEOM_FT(4);
    const GEOM_FT z= ((*this)(i-1,j).z()+(*this)(i+1,j).z()+(*this)(i,j-1).z()+(*this)(i,j+1).z())/GEOM_FT(4);
    return Pos3d(x,y,z);
  }

//! @brief Return the maximum of the distances between the mesh points
//! y corresponding to the Lagrange interpolation (see page IX-19 of
//1 the SAP90 manual).
GEOM_FT Pos3dArray::dist_lagrange(void) const
  {
    GEOM_FT retval(0.0);
    for(size_t i=2;i<n_rows;i++) //interior points.
      for(size_t j=2;j<n_columns;j++)
        retval= std::max(retval,dist((*this)(i,j),pos_lagrangiana(i,j)));
    return retval;
  }

//! @brief Set the interior points of the mesh.
//! corresponding to the Lagrange interpolation (see page IX-19 of the SAP90 manual).
//! Return the distance máxima obtenida.
GEOM_FT Pos3dArray::ciclo_lagrange(void)
  {
    for(size_t i=2;i<n_rows;i++) //interior points.
      for(size_t j=2;j<n_columns;j++)
        (*this)(i,j)= pos_lagrangiana(i,j);
    return dist_lagrange();
  }

//! @brief Set the interior points of the mesh
//! corresponding to the Lagrange interpolation (see page IX-19 of the SAP90 manual).
GEOM_FT Pos3dArray::Lagrange(const GEOM_FT &tol)
  {
    GEOM_FT err= dist_lagrange();
    size_t conta= 0;
    while(err>tol)
      {
        if(conta<10)
          err= ciclo_lagrange();
        else
          {
	    std::cerr << "Pos3dArray::" << __FUNCTION__
		      << "; doesn't converge after: "
                      << conta << " iterations." << std::endl;
            break;
          }
      }
    return err;
  }

//! @brief Applies the transformation argument to the matrix.
void Pos3dArray::Transform(const Trf3d &trf)
  { trf.Transform(*this); }

//! @brief Return the revolution surface obtained by applying to the matrix
//! the revolution transformation argument.
Pos3dArray create_revolution_surface(const Revolution3d &r,const Pos3dArray &m)
  { return r(m); }

Pos3dArray Quadrilateral(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3,const Pos3d &p4,const size_t &ndiv1,const size_t &ndiv2)
  { return Pos3dArray(p1,p2,p3,p4,ndiv1,ndiv2); }


//! @brief Return the triangle inscribed in the cell of the mesh the has
//! the (i,j) vertex as lower left corner and remains under the diagonal
//! between the (i,j) and the (i+1,j+1) vertices.
//
//         i+1,j +---+ i+1,j+1
//               |2 /|
//               | / |
//               |/ 1|
//           i,j +---+ i,j+1
Triangle3d Pos3dArray::getTriangle1(const size_t &i,const size_t &j) const
  { return Triangle3d((*this)(i,j),(*this)(i,j+1),(*this)(i+1,j+1)); }

//! @brief Return the triangle inscribed in the cell of the mesh the has
//! the (i,j) vertex as lower left corner and remains over the diagonal
//! between the (i,j) and the (i+1,j+1) vertices.
//
//         i+1,j +---+ i+1,j+1
//               |2 /|
//               | / |
//               |/ 1|
//           i,j +---+ i,j+1
Triangle3d Pos3dArray::getTriangle2(const size_t &i,const size_t &j) const
  { return Triangle3d((*this)(i,j),(*this)(i+1,j+1),(*this)(i+1,j)); }


//! @brief Distance from the point to the surface defined by the
//! inscribed triangles.
//!
//  Each mesh cell is filled with two triangles:
//         i+1,j +---+ i+1,j+1
//               |2 /|
//               | / |
//               |/ 1|
//           i,j +---+ i,j+1
GEOM_FT dist2(const Pos3dArray &ptos,const Pos3d &pt)
  {
    if(ptos.size()<1) return NAN; //Set is empty.
    GEOM_FT d= dist2(ptos(1,1),pt);
    if(ptos.size()==1) return d; //Degenerated mesh (only a point)
    //Distance to the triangles.
    const size_t n_rows= ptos.getNumberOfRows();
    const size_t n_columns= ptos.getNumberOfColumns();
    if(n_rows<2) //There is only a point row.
      {
        for(size_t j=1;j<n_columns;j++) //To the last but one column.
          {
            Segment3d s(ptos(1,j),ptos(1,j+1));
            d= std::min(d,s.dist2(pt));
          }
      }
    if(n_columns<2) //There is only a point column.
      {
        for(size_t i=1;i<n_rows;i++) //To the last but one row.
          {
            Segment3d s(ptos(i,1),ptos(i+1,1));
            d= std::min(d,s.dist2(pt));
          }
      }
    for(size_t i=1;i<n_rows;i++) //To the last but one row.
      for(size_t j=1;j<n_columns;j++) //To the last but one column.
        {
          Triangle3d t1=ptos.getTriangle1(i,j); //First triangle.
          d= std::min(d,t1.dist2(pt));
          Triangle3d t2=ptos.getTriangle2(i,j); //Second triangle.
          d= std::min(d,t2.dist2(pt));
        }
    return d;
  }

//! @brief Return true if the distance from the point to the surface
//! defined by the point matrix is less than the argument.
bool dist_menor(const Pos3dArray &ptos,const Pos3d &pt,const GEOM_FT &d_max)
  {
    const GEOM_FT d_max2(d_max*d_max);
    if(ptos.size()<1) return false; //El conjunto is empty.
    GEOM_FT d= dist2(ptos(1,1),pt);
    if(ptos.size()==1) //Degenerated mesh (only a point).
      return (d<d_max2);
    if(d<d_max2) //It's already OK, the first point is near enough..
      return true;
    const size_t n_rows= ptos.getNumberOfRows();
    const size_t n_columns= ptos.getNumberOfColumns();
    if(n_rows<2) //There is only a point row.
      {
        for(size_t j=1;j<n_columns;j++) //To the last but one column.
          {
            Segment3d s(ptos(1,j),ptos(1,j+1));
            d= std::min(d,s.dist2(pt));
            if(d<d_max2)
              return true;
          }
      }
    if(n_columns<2) //There is only a point column.
      {
        for(size_t i=1;i<n_rows;i++) //To the last but one row.
          {
            Segment3d s(ptos(i,1),ptos(i+1,1));
            d= std::min(d,s.dist2(pt));
            if(d<d_max2)
              return true;
          }
      }
    //Distance to the triangles.
    for(size_t i=1;i<n_rows;i++) //To the last but one row.
      for(size_t j=1;j<n_columns;j++) //To the last but one column.
        {
          Triangle3d t1=ptos.getTriangle1(i,j); //first triangle.
          d= std::min(d,t1.dist2(pt));
          if(d<d_max2)
            return true;
          Triangle3d t2=ptos.getTriangle2(i,j); //Second triangle.
          d= std::min(d,t2.dist2(pt));
          if(d<d_max2)
            return true;
        }
    return (d<d_max2);
  }
//! ! @brief Ver dist2.
GEOM_FT dist(const Pos3dArray &ptos,const Pos3d &pt)
  { return sqrt_FT(dist2(ptos,pt)); }


//! Maximum of the distances from the point to each of the planes
//! defined by the triangles inscribed in the mesh cells
//! i+1,j +---+ i+1,j+1
//!                                                                                    |2 /|
//!                                                                                    | / |
//!                                                                                    |/ 1|
//!                                                                                i,j +---+ i,j+1
GEOM_FT pseudo_dist2(const Pos3dArray &ptos,const Pos3d &pt)
  {
    if(ptos.size()<1) return NAN; //El conjunto is empty.
    GEOM_FT d= dist2(ptos(1,1),pt);
    if(ptos.size()==1) return d; //Degenerated mesh.
    //Distance to the triangles.
    const size_t n_rows= ptos.getNumberOfRows();
    const size_t n_columns= ptos.getNumberOfColumns();
    if(n_rows<2) //There is only a point row.
      {
        for(size_t j=1;j<n_columns;j++) //To the last but one column.
          {
            Segment3d s(ptos(1,j),ptos(1,j+1));
            d= std::max(d,s.dist2(pt));
          }
      }
    if(n_columns<2) //There is only a point column.
      {
        for(size_t i=1;i<n_rows;i++) //To the last but one row.
          {
            Segment3d s(ptos(i,1),ptos(i+1,1));
            d= std::max(d,s.dist2(pt));
          }
      }
    d= Plane(ptos.getTriangle1(1,1)).PseudoDist2(pt);
    for(size_t i=1;i<n_rows;i++) //To the last but one row.
      for(size_t j=1;j<n_columns;j++) //To the last but one column.
        {
          Plane p1(ptos.getTriangle1(i,j)); //Plane from first triangle.
          d= std::max(d,p1.PseudoDist2(pt));
          Plane p2(ptos.getTriangle2(i,j)); //Plane from second triangle.
          d= std::max(d,p2.PseudoDist2(pt));
        }
    return d;
  }

//! @brief Ver pseudo_dist2.
GEOM_FT pseudo_dist(const Pos3dArray &ptos,const Pos3d &pt)
  { return sqrt_FT(::Abs(pseudo_dist2(ptos,pt))); }

BND3d get_bnd(const Pos3dArray &ptos)
  {
    BND3d retval;
    if(ptos.size()<1) //Empty set.
      {
	std::cerr << __FUNCTION__
		  << "; the point matrix is empty." << std::endl;
        return retval;
      }
    if(ptos.size()<2)
      {
	std::cerr << __FUNCTION__
		  << "; the matrix has only a point."
		  << std::endl;
        retval= BND3d(ptos(1,1),ptos(1,1));
        return retval;
      }
    retval= BND3d(ptos.front(),ptos.back());
    const size_t n_rows= ptos.getNumberOfRows();
    const size_t n_columns= ptos.getNumberOfColumns();
    retval+= ptos(1,1);
    retval+= ptos(n_rows,1);
    retval+= ptos(1,n_columns);
    retval+= ptos(n_rows,n_columns);
    return retval;
  }
