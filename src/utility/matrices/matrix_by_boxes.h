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
//matrix_by_boxes.h

#ifndef MATRIX_BY_BOXES_H
#define MATRIX_BY_BOXES_H

#include "ZMatrix.h"

//! @brief Matrix made of boxes.
//! E is the template class for the boxes.
template <class TBOX>
class MatrixByBoxes: public ZMatrix<TBOX>
  {
  protected:
    size_t get_rows_row(const size_t &f) const;
    size_t get_columns_column(const size_t f) const;
    std::vector<size_t> get_dim_rows(void) const;
    std::vector<size_t> get_dim_columns(void) const;
    
  public:
    typedef ZMatrix<TBOX> box_matrix;
    typedef typename box_matrix::size_type size_type;
    typedef typename TBOX::value_type value_type;
    typedef typename box_matrix::iterator iterator;
    typedef typename box_matrix::const_iterator const_iterator;

    MatrixByBoxes(void);
    MatrixByBoxes(size_type n_rows,size_type n_columns);
    MatrixByBoxes(size_type n_rows,size_type n_columns,const TBOX &val);
    MatrixByBoxes(const std::vector<size_t> &);
    MatrixByBoxes(const std::vector<size_t> &,const std::vector<size_t> &);
    MatrixByBoxes(const MatrixByBoxes &);
    MatrixByBoxes& operator=(const MatrixByBoxes &m);

    size_t getTotalNumberOfRows(void) const;
    size_t getTotalNumberOfColumns(void) const;
    void Con(const value_type &d);
    MatrixByBoxes<TBOX> &Trn(void);
    MatrixByBoxes<TBOX> GetTrn(void) const;
    TBOX QuitaBloques(void);
    MatrixByBoxes<TBOX> &operator*=(const TBOX &m);
    MatrixByBoxes<TBOX> &operator*=(const value_type &d);
    MatrixByBoxes<TBOX> &operator*=(const MatrixByBoxes<TBOX> &m);
  };

//! @brief  Constructor.
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(void)
  : box_matrix(1,1) {}

//! @brief  Constructor.
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(size_type n_rows,size_type n_columns)
  : box_matrix(n_rows,n_columns) {}

//! @brief  Constructor.
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(size_type n_rows,size_type n_columns,const TBOX &val)
  : box_matrix(n_rows,n_columns,val) {}

//! @brief Constructor.
//!
//! @param dim: Dimensiones de rows and columns (square matrix).
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(const std::vector<size_t> &dim)
  :  box_matrix(dim.size(),dim.size())
  {
    for(size_t i=1;i<=this->n_rows;i++)
      for(size_t j=1;j<=this->n_columns;j++)
        (*this)(i,j)= m_double(dim[i-1],dim[j-1]);
  }

//! @brief Constructor.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_columns)
  :  box_matrix(dim_rows.size(),dim_columns.size())
  {
    for(size_t i=1;i<=this->n_rows;i++)
      for(size_t j=1;j<=this->n_columns;j++)
        (*this)(i,j)= m_double(dim_rows[i-1],dim_columns[j-1]);
  }

//! Constructor de copia.
template <class TBOX>
MatrixByBoxes<TBOX>::MatrixByBoxes(const MatrixByBoxes &other)
  : box_matrix(other) {}

//! @brief Assignment operator.
template <class TBOX>
MatrixByBoxes<TBOX> &MatrixByBoxes<TBOX>::operator=(const MatrixByBoxes<TBOX> &m)
  {
    box_matrix::operator=(m);
    return *this;
  }


//! @brief Return el number of rows of the iRow-th row matrices.
template <class TBOX>
size_t MatrixByBoxes<TBOX>::get_rows_row(const size_t &iRow) const
  {
    size_t retval= 0;
    const size_t ncls= this->getNumberOfColumns();
    for(size_t j=1;j<=ncls;j++)
      {
        const size_t nrows_j= (*this)(iRow,j).getNumberOfRows();
        if(nrows_j>retval) retval= nrows_j;
      }
    return retval;
  }

//! @brief Return el number of columns de las matrices de the column c.
template <class TBOX>
size_t MatrixByBoxes<TBOX>::get_columns_column(const size_t c) const
  {
    size_t retval= 0;
    const size_t nrows= this->getNumberOfRows();
    for(size_t i=1;i<=nrows;i++)
      {
        const size_t ncls_i= (*this)(i,c).getNumberOfRows();
        if(ncls_i>retval) retval= ncls_i;
      }
    return retval;
  }

//! @brief Return el number of rows of each row.
template <class TBOX>
std::vector<size_t> MatrixByBoxes<TBOX>::get_dim_rows(void) const
  {
    const size_t nrows= this->getNumberOfRows();
    std::vector<size_t> retval(nrows,0);
    for(size_t i=1;i<=nrows;i++)
      retval[i-1]= get_rows_row(i);
    return retval;
  }
//! @brief Return el number of rows of each column.
template <class TBOX>
std::vector<size_t> MatrixByBoxes<TBOX>::get_dim_columns(void) const
  {
    const size_t ncls= this->getNumberOfColumns();
    std::vector<size_t> retval(ncls,0);
    for(size_t j=1;j<=ncls;j++)
      retval[j-1]= get_columns_column(j);
    return retval;
  }
//! @brief Return the sum of the rows of boxes.
template <class TBOX>
size_t MatrixByBoxes<TBOX>::getTotalNumberOfRows(void) const
  {
    std::vector<size_t> dim_rows= get_dim_rows();
    size_t tot_rows= dim_rows[0];
    for(size_t i=1;i<this->n_rows;i++)
      tot_rows+= dim_rows[i];
    return tot_rows;
  }
//! @brief Return the sum of the columns of boxes.
template <class TBOX>
size_t MatrixByBoxes<TBOX>::getTotalNumberOfColumns(void) const
  {
    std::vector<size_t> dim_columns= get_dim_columns();
    size_t tot_n_columns= dim_columns[0];
    for(size_t j=1;j<=this->n_columns;j++)
      tot_n_columns+= dim_columns[j];
    return tot_n_columns;
  }

//! @brief Assigns the argument to all the boxes.
template <class TBOX>
void MatrixByBoxes<TBOX>::Con(const value_type &d)
  {
    for(iterator i= this->begin();i!=this->end();i++)
      (*i).Con(d);
  }
//! @brief Traspose the matrix.
template <class TBOX>
MatrixByBoxes<TBOX> &MatrixByBoxes<TBOX>::Trn(void)
  {
    box_matrix::Trn();
    for(iterator i= this->begin();i!=this->end();i++)
      (*i).Trn();
    return *this;
  }
//! @brief Return transposed.
template <class TBOX>
MatrixByBoxes<TBOX> MatrixByBoxes<TBOX>::GetTrn(void) const
  {
    MatrixByBoxes<TBOX> m= *this;
    m.Trn();
    return m;
  }

//! @brief Return the matrix that has the same number of boxes that this has
//! blocks. We suppose that the number of rows of the blocks of a row is the
//! same and that the number of columns of the blocks are also the same.
template <class TBOX>
TBOX MatrixByBoxes<TBOX>::QuitaBloques(void)
  {
    TBOX retval(getTotalNumberOfRows(),getTotalNumberOfColumns());
    size_t f= 1,c= 1;
    TBOX box;
    for(size_t i= 1;i<=this->getNumberOfRows();i++)
      {
        for(size_t j= 1;j<=this->getNumberOfColumns();j++)
          {
            box= (*this)(i,j);
            retval.putBox(f,c,box);
            c+= box.getNumberOfColumns();
	      }
        f+= box.getNumberOfRows();
        c= 1;
      }
    return retval;
  }
//! @brief Operador *=
template <class TBOX>
MatrixByBoxes<TBOX> &MatrixByBoxes<TBOX>::operator*=(const TBOX &m)
  {
    this->Prod(m);
    return *this;
  }
//! @brief Operador *=
template <class TBOX>
MatrixByBoxes<TBOX> &MatrixByBoxes<TBOX>::operator*=(const value_type &d)
  {
    size_type i,sz= this->size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
//! @brief Operador *=
template <class TBOX>
MatrixByBoxes<TBOX> &MatrixByBoxes<TBOX>::operator*=(const MatrixByBoxes<TBOX> &m)
  {
    box_matrix::operator*=(m);
    return *this;
  }

#endif
