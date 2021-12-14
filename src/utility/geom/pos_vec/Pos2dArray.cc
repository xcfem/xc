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
//Pos2dArray.cc

#include "Pos2dArray.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d2/2d_polygons/Triangle2d.h"
#include "utility/geom/d2/2d_polygons/Quadrilateral2d.h"
#include "utility/geom/FT_matrix.h"
#include "utility/geom/trf/Trf2d.h"

//! @brief Ver el constructor correspondiente en PosArray<POS>.
Pos2dArray::Pos2dArray(const Pos2d &p1,const Pos2d &p2,const size_t &num,const GEOM_FT &ratio)
  : m_pos(p1,p2,num,ratio) {}

//! @brief Ver el constructor correspondiente en PosArray<POS>.
Pos2dArray::Pos2dArray(const Pos2d &p1,const Pos2d &p2,const size_t &ndiv)
  : m_pos(p1,p2,ndiv) {}

//! @brief Ver el constructor correspondiente en PosArray<POS>.
Pos2dArray::Pos2dArray(const Pos2d &p1,const Pos2d &p2,const std::vector<GEOM_FT> &longs)
  : m_pos(p1,p2,longs) {}

//! @brief Constructor.
Pos2dArray::Pos2dArray(const Pos2d &p0,const Pos2d &p1,const Pos2d &p2,const size_t &ndiv1,const size_t &ndiv2)
  : PosArray<Pos2d>(p0,p1,p2,ndiv1,ndiv2) {}

//! @brief Constructor.
Pos2dArray::Pos2dArray(const Pos2dArray &l1_points,const Pos2dArray &l2_points,const Pos2dArray &l3_points,const Pos2dArray &l4_points)
  : m_pos(l1_points,l2_points,l3_points,l4_points) {}

Pos2d Pos2dArray::pos_lagrangiana(const size_t &i,const size_t &j) const
  {
    assert(interior(i,j));
    const GEOM_FT x= ((*this)(i-1,j).x()+(*this)(i+1,j).x()+(*this)(i,j-1).x()+(*this)(i,j+1).x())/GEOM_FT(4);
    const GEOM_FT y= ((*this)(i-1,j).y()+(*this)(i+1,j).y()+(*this)(i,j-1).y()+(*this)(i,j+1).y())/GEOM_FT(4);
    return Pos2d(x,y);
  }

//! @brief Return the maximum distance between the mesh points
//! and the corresponding ones in the Lagrange interpolation
//! (see page IX-19 of the SAP90 manual).
GEOM_FT Pos2dArray::dist_lagrange(void) const
  {
    GEOM_FT retval(0.0);
    for(size_t i=2;i<n_rows;i++) //Interior points.
      for(size_t j=2;j<n_columns;j++)
        retval= std::max(retval,dist((*this)(i,j),pos_lagrangiana(i,j)));
    return retval;
  }

//! @brief Return el número de cuadriláteross
size_t Pos2dArray::GetNumQuads(void) const
  { return (n_rows-1)*(n_columns-1); }

//! @brief Set the interior points of the mesh
//! corresponding to Lagrange interpolation (see page IX-19 from SAP90 manual).
//! Return the maximum computed distance.
GEOM_FT Pos2dArray::ciclo_lagrange(void)
  {
    for(size_t i=2;i<n_rows;i++) //Interior points.
      for(size_t j=2;j<n_columns;j++)
        (*this)(i,j)= pos_lagrangiana(i,j);
    return dist_lagrange();
  }

//! @brief Set the interior points of the mesh
//! corresponding to Lagrange interpolation (see page IX-19 from SAP90 manual).
GEOM_FT Pos2dArray::Lagrange(const GEOM_FT &tol)
  {
    GEOM_FT err= dist_lagrange();
    size_t conta= 0;
    while(err>tol)
      {
        if(conta<10)
          err= ciclo_lagrange();
        else
          {
	    std::cerr << "Pos2dArray::" << __FUNCTION__
	              << "; convergence not reached after: "
                      << conta << " iterations." << std::endl;
            break;
          }
      }
    return err;
  }

//! @brief Return the position of the midpoint.
Pos2d Pos2dArray::getCenter(void) const
  { return get_centro(*this,Segment2d()); }

//! @brief Return the center of mass.
Pos2d Pos2dArray::getCenterOfMass(void) const
  {
    GEOM_FT areaQuad= 0.0;
    GEOM_FT Atot= 0.0;
    GEOM_FT Qy= 0.0;
    GEOM_FT Qx= 0.0;
    Pos2d c;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        {
          areaQuad= GetAreaQuad(i,j);
          c= getQuadCentroid(i,j);
          Atot+= areaQuad;
          Qx+= c.x()*areaQuad;
          Qy+= c.y()*areaQuad;
        }
    Qx/=Atot;
    Qy/=Atot;
    return Pos2d(Qx,Qy);
  }

//! @brief Return the moment of inertia with respect to the
//! axis which is parallel to the y axis through the origin.
GEOM_FT Pos2dArray::getIy(void) const
  {
    GEOM_FT retval= 0.0;
    Pos2d c;
    Quadrilateral2d q;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        {
          q= GetQuad(i,j);
          c= q.Centroide();
          retval+= q.Iy()+q.getArea()*sqr(c.x());
        }
    return retval;
  }

//! @brief Return el moment of inertia with respect to axis parallel to x por el origen
GEOM_FT Pos2dArray::getIx(void) const
  {
    GEOM_FT retval= 0.0;
    Pos2d c;
    Quadrilateral2d q;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        {
          q= GetQuad(i,j);
          c= q.Centroide();
          retval+= q.Ix()+q.getArea()*sqr(c.y());
        }
    return retval;
  }

//! @brief Return el product of inertia with respect to the axis that
//! pass through the origin.
GEOM_FT Pos2dArray::GetPxy(void) const
  {
    GEOM_FT retval= 0.0;
    Pos2d c;
    Quadrilateral2d q;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        {
          q= GetQuad(i,j);
          c= q.Centroide();
          retval+= q.Pxy()+q.getArea()*c.x()*c.y();
        }
    return retval;
  }

GEOM_FT Pos2dArray::GetArea(void) const
  {
    GEOM_FT retval= 0.0;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        retval+= GetAreaQuad(i,j);
    return retval;
  }

//! @brief Moment of inertia with respect to axis parallel to x through the center of mass.
GEOM_FT Pos2dArray::Ix(void) const
  { return getIx()-GetArea()*sqr(getCenterOfMass().y()); }

//! @brief Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Pos2dArray::Iy(void) const
  { return getIy()-GetArea()*sqr(getCenterOfMass().x()); }

//! @brief product of inertia with respect to the center of mass in local coordinates.
GEOM_FT Pos2dArray::Pxy(void) const
  {
    const GEOM_FT d2= getCenterOfMass().x()*getCenterOfMass().y();
    return GetPxy()-GetArea()*d2;
  }

//! @brief Return the triangle inscribed in the cell of the mesh the has
//! the (i,j) vertex as lower left corner and remains under the diagonal
//! between the (i,j) and the (i+1,j+1) vertices.
//
//         i+1,j +---+ i+1,j+1
//               |2 /|
//               | / |
//               |/ 1|
//           i,j +---+ i,j+1
Triangle2d Pos2dArray::getTriangle1(const size_t &i,const size_t &j) const
  { return Triangle2d((*this)(i,j),(*this)(i,j+1),(*this)(i+1,j+1)); }

//! @brief Return the triangle inscribed in the cell of the mesh the has
//! the (i,j) vertex as lower left corner and remains over the diagonal
//! between the (i,j) and the (i+1,j+1) vertices.
//
//         i+1,j +---+ i+1,j+1
//               |2 /|
//               | / |
//               |/ 1|
//           i,j +---+ i,j+1
Triangle2d Pos2dArray::getTriangle2(const size_t &i,const size_t &j) const
  { return Triangle2d((*this)(i,j),(*this)(i+1,j+1),(*this)(i+1,j)); }


//! @brief Return the x coordinate of point i,j.
GEOM_FT Pos2dArray::GetX(const size_t &i,const size_t &j) const
  { return (*this)(i,j).x(); }

//! @brief Return the y coordinate of point i,j.
GEOM_FT Pos2dArray::GetY(const size_t &i,const size_t &j) const
  { return (*this)(i,j).y(); }

//! @brief Return the coordinates of the i,j point.
const FT_matrix &Pos2dArray::GetVertCoords(const size_t &i,const size_t &j) const
  {
    const Pos2d tmp= getPoint(i,j);
    static FT_matrix tmp_m(1,2);
    tmp_m(1,1)= tmp.x();
    tmp_m(1,2)= tmp.y();
    return tmp_m;
  }

//! @brief Return the point i,j.
Pos2d Pos2dArray::getPoint(const size_t &i,const size_t &j) const
{ return (*this)(i,j); }


//! @brief Return the el cuadrilátero i,j:
//                                                                             i+1,j +---+ i+1,j+1
//                                                                                   |   |
//                                                                                   |   |
//                                                                                   |   |
//                                                                               i,j +---+ i,j+1
Quadrilateral2d Pos2dArray::GetQuad(const size_t &i,const size_t &j) const
  { return Quadrilateral2d((*this)(i,j),(*this)(i,j+1),(*this)(i+1,j+1),(*this)(i+1,j)); }


bool Pos2dArray::In(const Pos2d &p, const double &tol) const
  {
    bool retval= false;
    for(size_t i=1;i<n_rows;i++)
      for(size_t j=1;j<n_columns;j++)
        if(GetQuad(i,j).In(p,tol))
          {
            retval= true;
            break;
          }
    return retval;
  }

//! @brief Return the area of the quad at i,j:
//                                                                             i+1,j +---+ i+1,j+1
//                                                                                   |   |
//                                                                                   |   |
//                                                                                   |   |
//                                                                               i,j +---+ i,j+1
GEOM_FT Pos2dArray::GetAreaQuad(const size_t &i,const size_t &j) const
  { return GetQuad(i,j).getArea(); }

//! @brief Return the centroid of the quad at i,j:
//                                                                             i+1,j +---+ i+1,j+1
//                                                                                   |   |
//                                                                                   |   |
//                                                                                   |   |
//                                                                               i,j +---+ i,j+1
Pos2d Pos2dArray::getQuadCentroid(const size_t &i,const size_t &j) const
  { return GetQuad(i,j).Centroide(); }

//! @brief Return the maximum value of the k coordinate.
GEOM_FT Pos2dArray::GetMax(unsigned short int k) const
  {
    GEOM_FT mx= (*this)(1,1)(k);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        mx= std::max((*this)(i,j)(k),mx);
    return mx;
  }

//! @brief Return the minimum value of the k coordinate.
GEOM_FT Pos2dArray::GetMin(unsigned short int k) const
  {
    GEOM_FT mn= (*this)(1,1)(k);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        mn= std::min((*this)(i,j)(k),mn);
    return mn;
  }

//! @brief Applies the transformation to the points.
void Pos2dArray::Transform(const Trf2d &trf2d)
  {
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        (*this)(i,j)= trf2d.Transform((*this)(i,j));
  }
