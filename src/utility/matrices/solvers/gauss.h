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
//Gauss.h
// Gaussian procedure for the solution of a linear system of equations.

#ifndef GAUSS_H
#define GAUSS_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/pivote.h"
#include "utility/matrices/solvers/calcularx.h"

template <class treal>
void eliminar( ZMatrix<treal> &a,
               ZMatrix<treal> &b,
               const size_t &j,
               size_t *pivot,
               const set_szt &fp)
  {
    size_t i,k,l,n= a.getNumberOfRows(),m=b.getNumberOfColumns();
    l=pivot[j-1]; treal r=gj_inv(a(l,j));
    for(k=j+1;k<= n;k++) a(l,k)*=r;
    for(k=1;k<=m;k++) b(l,k)*=r;
    for(i=1;i<=n;i++)
      if (!In(fp,i) && (gj_abs(a(i,j))>zero))
	{
          for(k=j+1;k<=n;k++) a(i,k)-= a(i,j)*a(l,k);
          for(k=1;k<=m;k++) b(i,k)-= a(i,j)*b(l,k);
        }
  }

template <class treal>
ZMatrix<treal> gauss(ZMatrix<treal> &a,ZMatrix<treal> &b,int &regular)
  {
    size_t j;
    //Dimension of the pivot indices matrix.
    typename ZMatrix<treal>::size_type n= a.getNumberOfRows();
    typename ZMatrix<treal>::size_type *pivot= new typename ZMatrix<treal>::size_type[n];
    set_szt fp;
    regular=1; j=0;
    while (regular && (j<n))
      {
        j++;
        pivot[j-1]= pivote(j,a,fp);
        regular=(pivot[j-1]>0);
        if(regular)
          {
            fp.insert(pivot[j-1]);
            eliminar(a,b,j,pivot,fp);
          }
      }
    ZMatrix<treal> x;
    if(regular) x= calcularx(a,b,pivot);
    delete[] pivot;
    return x;
  }

template <class treal>
ZMatrix<treal> gauss_const(const ZMatrix<treal> &a,const ZMatrix<treal> &b,int &regular)
  {
    ZMatrix<treal> c=a,d=b;
    return gauss(c,d,regular);
  }

template <class treal>  
ZMatrix<treal> operator /(const ZMatrix<treal> &b, const ZMatrix<treal> &a)
//Se le pasan copias de los valores de b y a.
  {
    if (b.getNumberOfRows() != a.getNumberOfRows())
      {
	std::cerr << "Matrices de dimensiones incompatibles en operador /" << std::endl;
        abort();      
      }
    int regular;
    ZMatrix<treal> x= gauss_const(a,b,regular);
    if (!regular) std::cerr << " singular matrix" << std::endl;
    return x;
  }

template <class treal>
ZMatrix<treal> operator /(const ZMatrix<treal> &m,const treal &d)
  { return m*gj_inv(d); }

//! @brief Return inverse matrix.
template <class treal>
ZMatrix<treal> gauss_invierte(const ZMatrix<treal> &a,int &regular)
  {
    ZMatrix<treal> b(a.getNumberOfRows(),a.getNumberOfColumns());
    b.Idn();
    ZMatrix<treal> x= gauss_const(a,b,regular);
    return x;
  }

#endif


