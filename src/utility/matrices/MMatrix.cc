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
//MMatrix.cc

#include "MMatrix.h"
#include "vcolM.h"

//! @brief Constructor.
//!
//! @param dim: Dimensiones de rows and columns (square matrix).
MMatrix::MMatrix(const std::vector<size_t> &dim)
  :  m_matrix(dim) {}

//! @brief Constructor.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
MMatrix::MMatrix(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_columns)
  :  m_matrix(dim_rows,dim_columns) {}

//! @brief Operador *=
MMatrix &MMatrix::operator*=(const m_double &m)
  {
    m_matrix::operator*=(m);
    return *this;
  }

//! @brief Operador *=
MMatrix &MMatrix::operator*=(const double &d)
  {
    m_matrix::operator*=(d);
    return *this;
  }
//! @brief Operador *=
MMatrix &MMatrix::operator*=(const MMatrix &m)
  {
    m_matrix::operator*=(m);
    return *this;
  }

//! @brief Operador *=
MMatrix &MMatrix::operator*=(const vcolM &v)
  {
    m_matrix::operator*=((const MMatrix &)v);
    return *this;
  }

MMatrix operator*(const MMatrix &m1,const MMatrix &m2)
  {
    MMatrix producto(m1);
    producto*=m2;
    return producto;
  }

//! @brief Return the transposed matrix.
MMatrix MMatrix::GetTrn(void) const
  {
    MMatrix m= *this;
    m.Trn();
    return m;
  }

//! @brief Convert the argument matrix in a block matrix.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
//! @param a: matrix that will be converted.
MMatrix to_boxes(const std::vector<size_t> dim_rows,const std::vector<size_t> dim_columns,const m_double &a)
  {
    MMatrix retval(dim_rows,dim_columns);
    //Comprobamos dimensiones.
    assert(retval.getTotalNumberOfRows()==a.getNumberOfRows()); 
    assert(retval.getTotalNumberOfColumns()==a.getNumberOfColumns());

    const size_t n_rows= retval.getNumberOfRows();
    const size_t n_columns= retval.getNumberOfColumns();

    size_t row_box= 1; //Row where the box starts.
    size_t col_box= 1; //Column where the box starts.
    for(size_t i=1;i<=n_rows;i++)
      {
        col_box= 1;
        const size_t fbloque= dim_rows[i-1]; //Rows of i-th row.
        for(size_t j=1;j<=n_columns;j++)
          {
            const size_t cbloque= dim_columns[j-1]; //Columns of j-th column.
            retval(i,j)= a.getBox(row_box,col_box,row_box+fbloque-1,col_box+cbloque-1);
            col_box+=cbloque; //Next column.
          }
        row_box+=fbloque; //Next row.
      }
    return retval;
  }

//! @brief Convert the argument matrix in a block matrix.
//!
//! @param Sample: template matrix which blocks dimensions will be used
//! to define the blocks of the new matrix.
//! @param a: matrix that will be converted.
MMatrix to_boxes(const MMatrix &Sample,const m_double &a)
  {
    const std::vector<size_t> dim_rows= Sample.get_dim_rows();
    const std::vector<size_t> dim_columns= Sample.get_dim_columns();
    return to_boxes(dim_rows,dim_columns,a);
  }




