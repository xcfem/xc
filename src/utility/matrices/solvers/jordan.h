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
//Jordan.h
// Jordan procedure for the solution of a linear system of equations.

#ifndef JORDAN_H
#define JORDAN_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/pivote.h"

template <class treal>
void eliminar( ZMatrix<treal> &a,
               ZMatrix<treal> &b,
               const size_t &j,size_t *pivot)
  {
    size_t i,k,l,n= a.getNumberOfRows(),m= b.getNumberOfColumns();
    l=pivot[j]; treal r=gj_inv(a(l,j));
    for(k=j+1;k<= n;k++) a(l,k)*=r;
    for(k=1;k<=m;k++) b(l,k)*=r;
    for(i=1;i<=n;i++)
      if ((i!=l) && (gj_abs(a(i,j))>zero))
	{
          for(k=j+1;k<=n;k++) a(i,k)-= a(i,j)*a(l,k);
          for(k=1;k<=m;k++) b(i,k)-= a(i,j)*b(l,k);
        }
  }

template <class treal>
ZMatrix<treal> calcularx( const ZMatrix<treal> &a,
                          const ZMatrix<treal> &b,
                          size_t *pivot)
  {
    size_t n= a.getNumberOfRows(),m=b.getNumberOfColumns();
    ZMatrix<treal> x(n,m,0.0);
    for(size_t j=1;j<= n;j++)
      for(size_t k=1;k<=m;k++) x(j,k)= b(pivot[j],k);
    return x;
  }

template <class treal>
ZMatrix<treal> jordan(ZMatrix<treal> &a,ZMatrix<treal> &b,int &regular)
  {
    size_t j;
    //Dimension of the pivot indices matrix.
    typedef typename ZMatrix<treal>::size_type size_type;
    const size_type n= a.getNumberOfRows();
    size_type *pivot= new size_type[n];
    set_szt fp;
    regular= 1; j=0;
    while (regular && (j<n))
      {
        j++;
        pivot[j]= pivote(j,a,fp);
        regular= (pivot[j]>0);
        if(regular)
          {
            fp.insert(pivot[j]);
            eliminar(a,b,j,pivot);
          }
      }
     ZMatrix<treal> x;
     if(regular) x= calcularx(a,b,pivot);
     delete[] pivot;
     return x;
  }

template <class treal>
ZMatrix<treal> jordan_const(const ZMatrix<treal> &a,const ZMatrix<treal> &b,int &regular)
  {
    ZMatrix<treal> c=a,d=b;
    return jordan(c,d,regular);
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
    ZMatrix<treal> x= jordan_const(a,b,regular);
    if (!regular) std::cerr << " singular matrix" << std::endl;
    return x;
  }

template <class treal>
ZMatrix<treal> operator /(const ZMatrix<treal> &m,const treal &d)
  { return m*gj_inv(d); }

#endif
