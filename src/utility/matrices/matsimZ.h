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
//MatsimZ.h
//Symmetric and antysimmetric matrices.
//Lower triangle is stored.
#ifndef MATSIMZ_H
#define MATSIMZ_H

#include "ZMatrix.h"
#include <algorithm>

template <class numero>
class matsimZ : public ZMatrix<numero>
  {
  public:
    typedef ZMatrix<numero> ZMatrix_number;
    typedef typename ZMatrix_number::size_type size_type;
  protected:
    //Multiplies the elements of the lower triangle
    //can be used with antysimmetric matrices.
    int signo;
    inline virtual size_type Tam(size_type n_rows,size_type cols)
      { return (n_rows*(n_rows+1)/2); }
    inline virtual size_type Indice( const size_type &iRow,
                                     const size_type &iCol) const
      {
        size_type f= iRow, c= iCol;
        if (f < c) std::swap(f,c);
        return (c-1)*(2*this->n_rows-c)/2+f-1;
      }
  public:
    matsimZ(void) : ZMatrix_number (1,1) {}
    matsimZ(size_type dim) : ZMatrix_number(1,Tam(dim,dim))
      { PutDim(dim,dim); }
    matsimZ(size_type dim,numero val) : ZMatrix_number(1,Tam(dim,dim),val)
      { PutDim(dim,dim); }
    matsimZ(const matsimZ<numero> &other) : ZMatrix_number(other)
      { PutDim(other.n_rows,other.n_columns); }
    matsimZ<numero>& operator =(const matsimZ<numero> &m)
      {
        ZMatrix_number::operator =(m);
        return *this;
      }
    matsimZ<numero>& operator +=(const matsimZ<numero> &m)
      {
        ZMatrix_number::Suma(m);
        return *this;
      }
    matsimZ<numero>& operator -=(const matsimZ<numero> &m)
      {
        ZMatrix_number::Resta(m);
        return *this;
      }
    inline void Trn(void) {}
    inline bool Cuadrada(void) const
      { return 1; }
    ZMatrix_number GetCompleta(void) const
      {
        ZMatrix_number retval(this->n_rows,this->n_columns);
        for(size_t i=1;i<=this->n_rows;i++)
          for(size_t j=1;j<=this->n_columns;j++)
            retval(i,j)= (*this)(i,j);
        return retval;
      }
    friend matsimZ<numero> operator *(const matsimZ<numero> &m,const numero &p)
      {
        matsimZ<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend matsimZ<numero> operator *(const numero &p,const matsimZ<numero> &m)
      { return m*p; }
    friend matsimZ<numero> operator +(const matsimZ<numero> &m1,const matsimZ<numero> &m2)
      {
        check_sum(m1,m2);
        matsimZ<numero> suma(m1.n_rows);
        suma.Suma(m1,m2);
        return suma;
      }
    friend matsimZ<numero> operator -(const matsimZ<numero> &m1,const matsimZ<numero> &m2)
      {
        check_dif(m1,m2);
        matsimZ<numero> resta(m1.n_rows,m1.n_columns);
        resta.Resta(m1,m2);
        return resta;
      }
    friend std::ostream &operator << (std::ostream &stream,const matsimZ<numero> &m)
      { return ::operator <<(stream,(const ZMatrix_number &) m); }
    friend std::istream &operator >> (std::istream &stream,matsimZ<numero> &m)
      { return ::operator >>(stream,(ZMatrix_number &) m); }
    friend bool operator ==(const matsimZ<numero> &m1,const matsimZ<numero> &m2)
      { return ((const ZMatrix_number &) m1 == (const ZMatrix_number &) m2); }
  };

#endif



