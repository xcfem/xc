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
//MExpressionMatrix.h
//Matrix of matrices of expressions (or by boxes)
#ifndef MEXPRESSIONMATRIX_H
#define MEXPRESSIONMATRIX_H

#include "ExpressionMatrix.h"
#include "matrix_by_boxes.h"

class MMatrix;
class ExprAlgebra;

typedef MatrixByBoxes<ExpressionMatrix> m_expression_matrix;

class MExpressionMatrix: public m_expression_matrix
  {
    void eval(void);
    void eval(const char *palabra,const ExprAlgebra &a);
    void eval(const char *palabra,const double &d);
  public:
    MExpressionMatrix(void) : m_expression_matrix(1,1) {}
    MExpressionMatrix(size_type n_rows,size_type cols) : m_expression_matrix(n_rows,cols) {}
    MExpressionMatrix(size_type n_rows,size_type cols,const ExpressionMatrix &val)
      : m_expression_matrix(n_rows,cols,val) {}
    MExpressionMatrix(const MExpressionMatrix &other) : m_expression_matrix(other) {}
    MExpressionMatrix& operator=(const MExpressionMatrix &m)
      {
	m_expression_matrix::operator=(m);
        return *this;
      }
    bool Evaluable(void) const;
    MExpressionMatrix Eval(void) const;
    MExpressionMatrix Eval(const char *palabra,const ExprAlgebra &a);
    MExpressionMatrix Eval(const char *palabra,const double &d);
    MMatrix ToNum(void) const;
    MMatrix ToNum(const char *palabra,const double &d) const;
    MExpressionMatrix &operator*=(const ExpressionMatrix &m);
    MExpressionMatrix &operator*=(const double &d);
    MExpressionMatrix &operator*=(const MExpressionMatrix &m);

    friend MExpressionMatrix operator+(const MExpressionMatrix &m1,const MExpressionMatrix &m2);
    friend MExpressionMatrix operator-(const MExpressionMatrix &m1,const MExpressionMatrix &m2);
    friend MExpressionMatrix operator*(const MExpressionMatrix &m,const double &d)
;
    friend MExpressionMatrix operator*(const double &d,const MExpressionMatrix &m)
      { return m*d; }
    friend MExpressionMatrix operator*(const MExpressionMatrix &m1,const MExpressionMatrix &m2);
    friend MExpressionMatrix operator*(const ExpressionMatrix &m,const MExpressionMatrix &mM);
    friend MExpressionMatrix operator*(const MExpressionMatrix &mM, const ExpressionMatrix &m)
      { return m*mM; }
  };


#endif
