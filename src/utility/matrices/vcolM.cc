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
//vcolM.cc

#include "vcolM.h"

//! @brief Constructor.
//!
//! @param dim: Dimensiones de rows and columns (square matrix).
vcolM::vcolM(const std::vector<size_t> &dim)
  : MMatrix(dim.size(),1)
  {
    const size_t num_gdl= dim.size();
    vcolM retval(num_gdl,m_double(1,1,0.0));
    for(size_t i=0;i<num_gdl;i++) //Iteramos sobre los grados de libertad.
      (*this)(i+1)= m_double(dim[i],1,0.0); //Inicializamos el vector.
  }
//! @brief Constructor.
vcolM::vcolM(const MMatrix &m,const size_type &c)
  : MMatrix(m.getNumberOfRows(),1)
  {
    const size_type nf= m.getNumberOfRows();
    for(size_type i=1;i<=nf;i++)
      (*this)(i)= m(i,1);
  }
vcolM &vcolM::operator*=(const m_double &m)
  {
    Prod(m);
    return *this;
  }
vcolM &vcolM::operator*=(const double &d)
  {
    MMatrix::operator*=(d);
    return *this;
  }
vcolM operator*(const MMatrix &a,const vcolM &b)
  {
    MMatrix tmp(a);
    tmp*=b;
    return vcolM(tmp,1);
  }

vcolM operator+(const vcolM &m1,const vcolM &m2)
  {
    vcolM retval(m1);
    retval+=m2;
    return retval;
  }

vcolM operator-(const vcolM &m1,const vcolM &m2)
  {
    vcolM retval(m1);
    retval-=m2;
    return retval;
  }

vcolM operator*(const vcolM &m,const double &d)
  {
    vcolM retval(m);
    retval*=d;
    return retval;
  }
vcolM operator*(const m_double &m,const vcolM &mM)
  {
    vcolM retval(mM); retval*=m;
    return retval;
  }
