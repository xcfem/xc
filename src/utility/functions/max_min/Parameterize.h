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
//Parameterize.h

#ifndef PARAMETERIZE_H
#define PARAMETERIZE_H

#include "utility/matrices/m_double.h"

//! Converts a Rn->R function in a R->R
//! function fixing all the variables except
//! the i-th one.
template<class F,size_t i>
class Parameterize
  {
    const F f_;
    const m_double &x;
  public:
    Parameterize(const F &f,const m_double &x0)
      : f_(f), x(x0) {}
    double operator()(const double &d) const
      {
        m_double nuevo(x);
        nuevo(i)= d;
        return f_(nuevo);
      }
  };

template <class F>
inline m_double matrix_argument(const F &f,const m_double &m)
  {
    m_double retval(m.getNumberOfRows(),m.getNumberOfColumns());
    for(size_t k= 1; k <= m.getNumberOfRows(); k++)
      for(size_t l= 1; l <= m.getNumberOfColumns(); l++)
        retval(k,l)= f(m(k,l));
    return retval;
  }

#endif
