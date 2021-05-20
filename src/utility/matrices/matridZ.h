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
//MatridZ.h
//Matrices tridiagonales.
//The matrix is stored by rows the first vector component is returned when
// the indexes are outside the three diagonals.
#ifndef MATRIDZ_H
#define MATRIDZ_H

#include "ZMatrix.h"

template <class numero>
class matridZ : public ZMatrix<numero>
  {
  public:
    typedef ZMatrix<numero> ZMatrix_number;  
    typedef typename ZMatrix_number::reference reference;
    typedef typename ZMatrix_number::const_reference const_reference;
    typedef typename ZMatrix_number::size_type size_type;
  protected:
    inline virtual size_type Tam(size_type n_rows,size_type cols) const
      { return (3*n_rows-1); }
    inline virtual int se_guarda(size_type iRow,size_type col) const
      { return (labs(iRow-col) <= 1); }
    inline virtual size_type Indice( const size_type &iRow,
                                     const size_type &col) const
      {
        if(!se_guarda(iRow,col) || (iRow == 0) || (col == 0))
          return 0;
        else
          return ((iRow-1)*3+col-iRow+1);
      }
  public:
    matridZ(void) : ZMatrix_number (1,1)
      { vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const size_type &dim) : ZMatrix_number(1,Tam(dim,dim))
      { PutDim(dim,dim); vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const size_type &dim,const numero &val) : ZMatrix_number(1,Tam(dim,dim))
      { PutDim(dim,dim); vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const matridZ<numero> &other) : ZMatrix_number(other)
      { PutDim(other.n_rows,other.n_columns); vectorZ_numero::operator[](0)= 0.0; }
    matridZ<numero>& operator =(const matridZ<numero> &m)
      {
        ZMatrix_number::operator =(m);
        return *this;
      }
    matridZ<numero>& operator +=(const matridZ<numero> &m)
      {
        ZMatrix_number::Suma(m);
        return *this;
      }
    matridZ<numero>& operator -=(const matridZ<numero> &m)
      {
        ZMatrix_number::Resta(m);
        return *this;
      }
    inline void Trn(void) {}
    inline bool Cuadrada(void)
      { return 1; }
    friend matridZ<numero> operator *(const matridZ<numero> &m,const numero &p)
      {
        matridZ<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend matridZ<numero> operator *(const numero &p,const matridZ<numero> &m)
      { return m*p; }
    friend matridZ<numero> operator +(const matridZ<numero> &m1,const matridZ<numero> &m2)
      {
        if (!CompDim(m1,m2))
          ZMatrixError("Matrices de dimensiones distintas en operador +");
        matridZ<numero> suma(m1.n_rows);
        suma.Suma(m1,m2);
        return suma;
      }
    friend matridZ<numero> operator -(const matridZ<numero> &m1,const matridZ<numero> &m2)
      {
        if (!CompDim(m1,m2)) 
          ZMatrixError("Matrices de dimensiones distintas en operador -");
        matridZ<numero> resta(m1.n_rows,m1.n_columns);
        resta.Resta(m1,m2);
        return resta;
      }
    friend std::ostream &operator << (std::ostream &stream,const matridZ<numero> &m)
      { return ::operator <<(stream,(const ZMatrix_number &) m); }
    friend std::istream &operator >> (std::istream &stream,matridZ<numero> &m)
      { return ::operator >>(stream,(ZMatrix_number &) m); }
    friend bool operator ==(const matridZ<numero> &m1,const matridZ<numero> &m2)
      { return ((const ZMatrix_number &) m1 == (const ZMatrix_number &) m2); }
  };

#endif



