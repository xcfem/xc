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
//MExpressionMatrix.cc

#include "MExpressionMatrix.h"
#include "MMatrix.h"

void MExpressionMatrix::eval(void)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval();
  }
void MExpressionMatrix::eval(const char *palabra,const ExprAlgebra &a)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,a);
  }
void MExpressionMatrix::eval(const char *palabra,const double &d)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,d);
  }

MExpressionMatrix MExpressionMatrix::Eval(void) const
  {
    MExpressionMatrix retval(*this);
    retval.eval();
    return retval;
  }
MExpressionMatrix MExpressionMatrix::Eval(const char *palabra,const ExprAlgebra &a)
  {
    MExpressionMatrix retval(*this);
    retval.eval(palabra,a);
    return retval;
  }
MExpressionMatrix MExpressionMatrix::Eval(const char *palabra,const double &d)
  {
    MExpressionMatrix retval(*this);
    retval.eval(palabra,d);
    return retval;
  }
bool MExpressionMatrix::Evaluable(void) const
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        if(!(*this)(i,j).Evaluable())
          return false;
    return true;
  }
MMatrix MExpressionMatrix::ToNum(void) const
  {
    MMatrix retval(n_rows,n_columns);
    if(!Evaluable())
      std::cerr << "ExpressionMatrix::" << __FUNCTION__
	        << "cant evaluate the expression matrix" << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum();
    return retval;
  }
MMatrix MExpressionMatrix::ToNum(const char *palabra,const double &d) const
  {
    MMatrix retval(n_rows,n_columns);
    if(!Evaluable())
      std::cerr << "ExpressionMatrix::" << __FUNCTION__
	        << "cant evaluate the expression matrix" << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum(palabra,d);
    return retval;
  }
MExpressionMatrix &MExpressionMatrix::operator*=(const ExpressionMatrix &m)
  {
    m_expression_matrix::operator*=(m);
    return *this;
  }
MExpressionMatrix &MExpressionMatrix::operator*=(const double &d)
  {
    size_type i,sz= size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
MExpressionMatrix &MExpressionMatrix::operator*=(const MExpressionMatrix &m)
  {
    m_expression_matrix::operator*=(m);
    return *this;
  }
MExpressionMatrix operator*(const MExpressionMatrix &m1,const MExpressionMatrix &m2)
  {
    MExpressionMatrix producto(m1);
    producto*=(m2);
    return producto;
  }

MExpressionMatrix operator+(const MExpressionMatrix &m1,const MExpressionMatrix &m2)
  {
    MExpressionMatrix retval(m1);
    retval+=m2;
    return retval;
  }

MExpressionMatrix operator-(const MExpressionMatrix &m1,const MExpressionMatrix &m2)
  {
    MExpressionMatrix retval(m1);
    retval-=m2;
    return retval;
  }

MExpressionMatrix operator*(const MExpressionMatrix &m,const double &d)
  {
    MExpressionMatrix retval(m);
    retval*=d;
    return retval;
  }

MExpressionMatrix operator*(const ExpressionMatrix &m,const MExpressionMatrix &mM)
  {
    MExpressionMatrix retval(mM); retval*=m;
    return retval;
  }

