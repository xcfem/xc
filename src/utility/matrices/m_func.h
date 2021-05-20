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
//m_func.h
//Matrices of real functions of n variables, that is a function
//from Rn to Rn.

#ifndef M_FUNC_H
#define M_FUNC_H

#include "ZMatrix.h"
#include "m_double.h"

template<class F>
//! @brief Matrix of objects that have a () operator (::operator())
//! that receives a double matrix and return a double.
class FMatrix: public ZMatrix<F>
  {
  public:
    typedef ZMatrix<F> f_matrix;
    typedef typename f_matrix::size_type size_type;

    FMatrix(void) : f_matrix(1,1) {}
    FMatrix(size_type n_rows,size_type cols) : f_matrix(n_rows,cols) {}
    FMatrix(const FMatrix<F> &other) : f_matrix(other) {}
    m_double Eval(const m_double &d) const;
  };

template <class F>
m_double FMatrix<F>::Eval(const m_double &d) const
  {
    size_t sz= this->size();
    m_double retval(sz,1);
    for(size_t i= 1; i<=sz; i++)
      retval(i)= ((*this)(i))(d);
    return retval;
  }

typedef double (*tipo_m_func)(const m_double &);
typedef FMatrix<tipo_m_func> m_func;

#endif
