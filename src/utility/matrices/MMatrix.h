// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//MMatrix.h
//Matrix of matrices (or by boxes)
#ifndef MMATRIX_H
#define MMATRIX_H

#include "m_double.h"
#include "matrix_by_boxes.h"

typedef MatrixByBoxes<m_double> m_matrix;

class vcolM;

class MMatrix: public m_matrix
  {
  public:
    //! Constructor.
    MMatrix(void) : m_matrix(1,1) {}
    //! Constructor.
    MMatrix(size_type n_rows,size_type cols) : m_matrix(n_rows,cols) {}
    //! Constructor.
    MMatrix(size_type n_rows,size_type cols,const m_double &val)
      : m_matrix(n_rows,cols,val) {}
    MMatrix(const std::vector<size_t> &dim);
    MMatrix(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_cols);
    //! Constructor de copia.
    MMatrix(const MMatrix &other) : m_matrix(other) {}
    MMatrix& operator=(const MMatrix &m)
      {
	m_matrix::operator=(m);
        return *this;
      }
    MMatrix GetTrn(void) const;
    MMatrix &operator*=(const m_double &m);
    MMatrix &operator*=(const double &d);
    MMatrix &operator*=(const MMatrix &m);
    MMatrix &operator*=(const vcolM &M);
    friend MMatrix operator+(const MMatrix &m1,const MMatrix &m2)
      {
        MMatrix retval(m1);
        retval+=m2;
        return retval;
      }
    friend MMatrix operator-(const MMatrix &m1,const MMatrix &m2)
      {
        MMatrix retval(m1);
        retval-=m2;
        return retval;
      }
    friend MMatrix operator*(const MMatrix &m,const double &d)
      {
        MMatrix retval(m);
        retval*=d;
        return retval;
      }
    friend MMatrix operator*(const double &d,const MMatrix &m)
      { return m*d; }
    friend MMatrix operator*(const MMatrix &m1,const MMatrix &m2);
    friend MMatrix operator*(const m_double &m,const MMatrix &mM)
      {
        MMatrix retval(mM); retval*=m;
        return retval;
      }
    friend MMatrix operator*(const MMatrix &mM, const m_double &m)
      { return m*mM; }
    friend MMatrix to_boxes(const MMatrix &Sample,const m_double &a);
    friend MMatrix to_boxes(const std::vector<size_t> dim_n_rows,const std::vector<size_t> dim_n_columns,const m_double &a);
  };

#endif
