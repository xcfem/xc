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
//ExpressionMatrix.cc

#include "ExpressionMatrix.h"

ExpressionMatrix::ExpressionMatrix(const m_double &m)
  : expression_matrix(m.getNumberOfRows(),m.getNumberOfColumns())
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= ExprAlgebra(m(i,j));
  }
void ExpressionMatrix::eval(void)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval();
  }
void ExpressionMatrix::eval(const char *palabra,const ExprAlgebra &a)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,a);
  }
void ExpressionMatrix::eval(const char *palabra,const double &d)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,d);
  }
ExpressionMatrix ExpressionMatrix::Eval(void) const
  {
    ExpressionMatrix retval(*this);
    retval.eval();
    return retval;
  }
ExpressionMatrix ExpressionMatrix::Eval(const char *palabra,const ExprAlgebra &a)
  {
    ExpressionMatrix retval(*this);
    retval.eval(palabra,a);
    return retval;
  }
ExpressionMatrix ExpressionMatrix::Eval(const char *palabra,const double &d)
  {
    ExpressionMatrix retval(*this);
    retval.eval(palabra,d);
    return retval;
  }
bool ExpressionMatrix::Evaluable(void) const
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        if(!(*this)(i,j).Evaluable())
          return false;
    return true;
  }
m_double ExpressionMatrix::ToNum(void) const
  {
    m_double retval(n_rows,n_columns,0.0);
    if(!Evaluable())
      std::cerr << "ExpressionMatrix::" << __FUNCTION__
	        << " can't eval the matrix expression." << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum();
    return retval;  
  }
m_double ExpressionMatrix::ToNum(const char *palabra,const double &d) const
  {
    m_double retval(n_rows,n_columns,0.0);
    if(!Evaluable())
      std::cerr << "ExpressionMatrix::" << __FUNCTION__
	        << " can't eval the matrix expression." << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum(palabra,d);
    return retval;  
  }
ExpressionMatrix &ExpressionMatrix::Trn(void)
  {
    expression_matrix::Trn();
    return *this;
  }
ExpressionMatrix ExpressionMatrix::GetTrn(void) const
  {
    ExpressionMatrix m= *this;
    m.Trn();
    return m;
  }
ExpressionMatrix &ExpressionMatrix::operator*=(const double &d)
  {
    size_type i,sz= size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*= ExprAlgebra(d);
    return *this;
  }
ExpressionMatrix &ExpressionMatrix::operator*=(const ExpressionMatrix &m)
  {
    *this= (*this)*m;
    return *this;
  }
ExpressionMatrix operator*(const ExpressionMatrix &m1,const ExpressionMatrix &m2)
  {
    ExpressionMatrix producto(m1.n_rows,m2.n_columns);
    if (m1.n_columns != m2.n_rows)
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
        std::cerr << "  m1= " << m1 << std::endl;
        std::cerr << "  m2= " << m2 << std::endl;
        return producto;
      }
    ExpressionMatrix::size_type i=1,j=1;
    for (i= 1;i<=m1.n_rows;i++)
      for (j= 1;j<=m2.n_columns;j++)
        producto(i,j)= dot(m1.getRow(i),m2.getColumn(j));
    return producto;
  }




//! @brief Converts a string (i.e.: [[1.0+x^2,0.0][0.0,1.0-x]]) into
//! a matrix.
ExpressionMatrix string_to_expression_matrix(const std::string &str)
  {
    ExpressionMatrix retval(1,1);
    retval.Input(str);
    return retval;
  }

