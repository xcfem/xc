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
//vcolM.h

#ifndef VCOLM_H
#define VCOLM_H

#include "MMatrix.h"


//! column vector with components are matrix.
class vcolM: public MMatrix
  {
  private:
    inline m_double &operator()(const size_t &iRow,const size_t &)
      { return MMatrix::operator()(iRow,1); }
    inline const m_double &operator()(const size_t &iRow,const size_t &) const
      { return MMatrix::operator()(iRow,1); }
  public:
    //! Constructor por defecto.
    vcolM(const size_type &n_rows= 1)
      : MMatrix(n_rows,1) {}
    //! Constructor.
    vcolM(const size_type &n_rows,const m_double &val)
      : MMatrix(n_rows,1,val) {}
    vcolM(const std::vector<size_t> &dim);
    vcolM(const MMatrix &m,const size_type &c);
    //! Constructor de copia.
    vcolM(const vcolM &other) 
      : MMatrix(other) {}
    //! Operador asignacion.
    vcolM &operator=(const vcolM &m)
      {
	MMatrix::operator=(m);
        return *this;
      }
    m_double &operator()(const size_t &iRow=1)
      { return MMatrix::operator()(iRow,1); }
    const m_double &operator()(const size_t &iRow=1) const
      { return MMatrix::operator()(iRow,1); }

    vcolM &operator*=(const m_double &m);
    vcolM &operator*=(const double &d);
    friend vcolM operator+(const vcolM &m1,const vcolM &m2);
    friend vcolM operator-(const vcolM &m1,const vcolM &m2);
    friend vcolM operator*(const vcolM &m,const double &d);
    friend vcolM operator*(const double &d,const vcolM &m)
      { return m*d; }
    friend vcolM operator*(const m_double &m,const vcolM &mM);
    friend vcolM operator*(const vcolM &mM, const m_double &m)
      { return m*mM; }
  };
#endif

vcolM operator*(const MMatrix &a,const vcolM &b);
