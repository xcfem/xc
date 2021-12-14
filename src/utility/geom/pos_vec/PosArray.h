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
//PosArray.h
//Matrix of points

#ifndef POSARRAY_H
#define POSARRAY_H

#include "utility/matrices/TMatrix.h"
#include "../cgal_types.h"
#include "boost/lexical_cast.hpp"
#include "utility/utils/misc_utils/matem.h"


//! @ingroup GEOM
//
//! @brief Base class for position matrices used to represent grids of points.
template <class POS>
class PosArray: public TMatrix<POS,std::vector<POS> >
  {
  public:
    typedef TMatrix<POS,std::vector<POS> > m_pos;
    typedef typename POS::vector vector;
  protected:
    PosArray(const PosArray<POS> &mp, size_t &f1, size_t &c1, size_t &f2, size_t &c2)
      : m_pos(mp,f1,c1,f2,c2) {}
  public:
    PosArray(const size_t &f=1,const size_t &c=1,const POS &p= POS()): m_pos(f,c,p) {}
    PosArray(const POS &p1,const POS &p2,const size_t &num,const GEOM_FT &ratio);
    PosArray(const POS &p1,const POS &p2,const size_t &ndiv);
    PosArray(const POS &p1,const POS &p2,const std::vector<GEOM_FT> &longs);
    PosArray(const POS &p0,const POS &p1,const POS &p2,const size_t &ndiv1,const size_t &ndiv2);
    PosArray(const PosArray &l1_points,const PosArray &l2_points,const PosArray &l3_points,const PosArray &l4_points);
    inline PosArray<POS> getBox(size_t f1, size_t c1, size_t f2, size_t c2) const
      { return PosArray(*this,f1,c1,f2,c2); }
    inline PosArray<POS> getRow(size_t iRow) const
      { return getBox(iRow,1,iRow,this->n_columns); }
    inline PosArray<POS> getColumn(size_t col) const
      { return getBox(1,col,this->n_rows,col); }
  };

//! Build the point matrix between p1 and p2 in such way that, the firs point
//! is p1, the number of point is num and each point is obtained by summing
//! the product of ratio by the p1p2 vector to the previous one.
template <class POS>
PosArray<POS>::PosArray(const POS &p1,const POS &p2,const size_t &num,const GEOM_FT &ratio)
  : m_pos(num,1)
  {
    POS p(p1);
    const vector v_ratio= ratio * (p2-p1);
    for(size_t i=1;i<=num;i++)
      {
        p= p + v_ratio;
        (*this)(i,1)= p;
      }
  }

//! Build the point matrix between p1 and p2 in such way that, the firs point
//! is p1, the last one is p2 and the intermediate points result from dividing
//! the segment p1p2 in ndiv equal parts. So if ndiv= 1 only the p1 and p2
//! points are created. If ndiv= 2 the midpoint of the segment is created
//! in addition to p1 and p2, and so on...
template <class POS>
PosArray<POS>::PosArray(const POS &p1,const POS &p2,const size_t &ndiv)
  : m_pos(ndiv+1,1)
  {
    POS p(p1);
    if(ndiv==0)
      (*this)(1,1)= p;
    else
      {
        const GEOM_FT ratio= double_to_FT(1.0/boost::lexical_cast<double>(ndiv));
        const vector v_ratio= ratio * (p2-p1);
        for(size_t i=1;i<=ndiv+1;i++)
          {
            (*this)(i,1)= p;
            p= p + v_ratio;
          }
      }
  }

//! Build the point matrix between p1 and p2 in such way that, the firs point
//! is p1 the last one is p2, the number of points is longs.size()+1 and the
//! lengths of interior segments are proportional to those in longs.
//!
//!       l[0]     l[1]     l[2]     l[3]     l[4]
//!    *-------+----------+-----+-----------+-----+
//!   p1                                          p2
//!
template <class POS>
PosArray<POS>::PosArray(const POS &p1,const POS &p2,const std::vector<GEOM_FT> &longs)
  : m_pos(longs.size()+1,1)
  {
    const size_t sz= longs.size();
    const size_t num= sz+1;
    if(num<1)
      {
	std::cerr << "PosArray::" << __FUNCTION__
		  << "; list of lengths is empty." << std::endl;
        return;
      }
    else
      {
        (*this)(1,1)= p1;
        (*this)(num,1)= p2;
        if(num>1)
          {
            //Normalize the list.
            GEOM_FT denom(0);
            for(std::vector<GEOM_FT>::const_iterator i=longs.begin();i!=longs.end();i++)
              denom+= *i;
            POS p(p1);        
            const vector v= p2-p1;
            for(size_t i=0;i<sz;i++)
              {
                p= p + (longs[i]/denom)*v;
                (*this)(i+2,1)= p;
              }
          }
      }
  }

//! @brief Constructor.
template <class POS>
PosArray<POS>::PosArray(const POS &p0,const POS &p1,const POS &p2,const size_t &ndiv1,const size_t &ndiv2)
  : m_pos(ndiv1+1,ndiv2+1)
  {
    const GEOM_FT dn1= double_to_FT(1.0/boost::lexical_cast<double>(ndiv1));
    const GEOM_FT dn2= double_to_FT(1.0/boost::lexical_cast<double>(ndiv2));
    const vector v01= (p1-p0)*dn1;
    const vector v02= (p2-p0)*dn2;
    const size_t n_rows= this->getNumberOfRows();
    const size_t n_columns= this->getNumberOfColumns();
    for(size_t i=1;i<=n_rows;i++)
      {
        const vector vi= boost::lexical_cast<int>(i-1)*v02;
        for(size_t j=1;j<=n_columns;j++)
          (*this)(i,j)= p0+vi+boost::lexical_cast<int>(j-1)*v01;
      }
  }

template <class POS>
PosArray<POS>::PosArray(const PosArray &l1_points,const PosArray &l2_points,const PosArray &l3_points,const PosArray &l4_points)
  : m_pos(l1_points.getNumberOfRows(),l2_points.getNumberOfRows())
//The arguments are the points (X) in the following order (see rows).
//                 l3_points
//                  --->
//              X---X---X---X
//            ^ |           | ^
//  l4_points   | X           X | l2_points
//              |           |
//              X---X---X---X
//                  --->
//                 l1_points
//
// and return those points:
//
//              X---X---X---X
//            ^ |   |   |   | 
//            | X---X---X---X
//              |   |   |   |
//              X---X---X---X
//                  --->
//
//The rows are quasi-parallel to the lines l2 and l4
//The columns are quasi-parallel to 11 y l3
  {
    const size_t row_number= l1_points.getNumberOfRows();
    const size_t num_cols= l2_points.getNumberOfRows();
    PosArray row_points= l4_points;
    for(size_t i=1;i<=row_number;i++)
      {
        if(i>1)
          {
            if(i==row_number)
              row_points= l2_points;
            else
              row_points= PosArray(l1_points(i,1),l3_points(i,1),num_cols-1);
          }
        for(size_t j=1;j<=num_cols;j++)
          (*this)(i,j)= row_points(j,1);
      }
  }

template <class POS,class SEG>
POS get_centro(const PosArray<POS> &m,const SEG &sg)
  {
    POS retval;
    const size_t n_rows= m.getNumberOfRows();
    const size_t n_columns= m.getNumberOfColumns();
    const size_t iRow= n_rows/2;
    const size_t iColumn= n_columns/2;
    if(impar(n_rows) && impar(n_columns))
      retval= m(iRow+1,iColumn+1);
    else
      if(par(n_rows) && par(n_columns))
        {
          SEG s(m(iRow,iColumn),m(iRow+1,iColumn+1));
          retval= s.getCenterOfMass();
        }
      else
        if(impar(n_rows))
          {
            SEG s(m(iRow,iColumn),m(iRow,iColumn+1));
            retval= s.getCenterOfMass();
          }
        else
          {
            SEG s(m(iRow,iColumn),m(iRow+1,iColumn));
            retval= s.getCenterOfMass();
          }
    return retval;
  }

//! @brief Generate point in a quadrilateral (see page IX-16 of the SAP90 manual).
// The arguments are the points (X):
//                        
//            q3             q4
//              X-----------X
//              |           |
//              |           |          
//              |           |
//              X-----------X
//            q1             q2
//                 
// and the number of divisions in directions q1->q2 (ndiv1) and q1->q3 (ndiv2)
// and returns the points:
//
//              X---X---X---X
//            ^ |   |   |   | 
//            | X---X---X---X
//              |   |   |   |
//              X---X---X---X
//                  --->
//
template <class POS>
PosArray<POS> Quadrilateral(const POS &q1,const POS &q2,const POS &q3,const POS &q4,const size_t &ndiv1,const size_t &ndiv2)
  {
    const PosArray<POS> l1(q1,q2,ndiv1);
    const PosArray<POS> l2(q2,q4,ndiv2);
    const PosArray<POS> l3(q3,q4,ndiv1);
    const PosArray<POS> l4(q1,q3,ndiv2);
    return PosArray<POS>(l1,l2,l3,l4);
  }

//! @brief Generate the points by means of the algorithm that SAP90
//! names "frontal generation" (see page IX-18 of the SAP90 manual).
//! The first point of both lists is supposed to be the same.
template <class POS>
PosArray<POS> generacion_frontal(const PosArray<POS> &l1_points,const PosArray<POS> &l2_points)
  {
    const size_t nptos1= l1_points.getNumberOfRows();
    const size_t nptos2= l2_points.getNumberOfRows();
    const size_t ntot= nptos1*nptos2;
    PosArray<POS> retval(nptos1,nptos2);

    //Points of the first row.
    for(size_t j=1;j<=nptos1;j++)
      retval(1,j)= l1_points(j);
    //Points of the first column.
    for(size_t i=2;i<=nptos1;i++)
      retval(i,1)= l2_points(i);
    
    for(size_t i=2;i<=nptos1;i++)
      {
        const typename PosArray<POS>::vector v= retval(i,1)-retval(i-1,1);
        for(size_t j=2;j<=nptos1;j++)
          retval(i,j)= retval(i-1,j)+v;
      }
    return retval;
  }

#endif
