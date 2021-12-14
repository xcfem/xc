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
//ExpressionMatrix.h
//Matrix of expressions

#ifndef EXPRESSIONMATRIX_H
#define EXPRESSIONMATRIX_H

#include "ZMatrix.h"
#include "utility/functions/algebra/ExprAlgebra.h"
#include "m_double.h"

typedef ZMatrix<ExprAlgebra> expression_matrix;

class ExpressionMatrix: public expression_matrix
  {
    friend class MExpressionMatrix;

    void eval(void);
    void eval(const char *palabra,const ExprAlgebra &a);
    void eval(const char *palabra,const double &d);
  public:
    ExpressionMatrix(size_type n_rows=1,size_type cols=1,const ExprAlgebra &val=ExprAlgebra())
      : expression_matrix(n_rows,cols,val) {}
    ExpressionMatrix(const ExpressionMatrix &other) : expression_matrix(other) {}
    ExpressionMatrix &operator=(const ExpressionMatrix &m)
      {
	expression_matrix::operator=(m);
        return *this;
      }
    explicit ExpressionMatrix(const m_double &m);
    static LexAlgebra &LexA(void)
      { return ExprAlgebra::LexA(); }
    ExpressionMatrix &Trn(void);
    ExpressionMatrix GetTrn(void) const;
    bool Evaluable(void) const;
    ExpressionMatrix Eval(void) const;
    ExpressionMatrix Eval(const char *palabra,const ExprAlgebra &a);
    ExpressionMatrix Eval(const char *palabra,const double &d);
    m_double ToNum(void) const;
    m_double ToNum(const char *palabra,const double &d) const;
    ExpressionMatrix &operator*=(const double &d);
    ExpressionMatrix &operator*=(const ExpressionMatrix &m);
    friend ExpressionMatrix operator*(const ExpressionMatrix &m1,const ExpressionMatrix &m2);
    friend ExpressionMatrix operator*(const m_double &m,const ExpressionMatrix &mE)
      {
        ExpressionMatrix retval= mE*ExpressionMatrix(m);
        return retval;
      }
    friend ExpressionMatrix operator*(const ExpressionMatrix &mE, const m_double &m)
      { return m*mE; }
  };


ExpressionMatrix string_to_expression_matrix(const std::string &str);

#endif
