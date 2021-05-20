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
//Cholesky.h
//Cholesky procedure to solve a linear system of equations with a symmetric
//and positive definite coefficient matrix.

#ifndef CHOLESKY_H
#define CHOLESKY_H

#include <cmath>
#include "utility/matrices/matsimZ.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/gj_aux.h"

template <class treal>
treal prod( const size_t &i,
            const size_t &j,
            const matsimZ<treal> &a)
  {
    size_t k;
    treal s;
    s= a(i,j); k=1;   // j<i 
    while(k<j)
      {
        s-= a(i,k)*a(j,k);
        k++;
      }
    return s;
  }

template <class treal>
void colum(const size_t &j,matsimZ<treal> &a)
  {
    size_t i,n= a.getNumberOfRows();
    treal s= sqrt(a(j,j)); a(j,j)=s; i=j+1;
    while(i<=n)
      { a(i,j)= prod(i,j,a)/s; i++; }
  }

template <class treal>
ZMatrix<treal> calcularx(const matsimZ<treal> &a, const ZMatrix<treal> &b)
  {
    size_t i,k,n= a.getNumberOfRows();
    ZMatrix<treal> x(n,1,0.0);
    for(i= 1;i<=n;i++)          //first system
      {
        x(i,1)= b(i,1); k=1;
        while(k<i)
          {
            x(i,1)-= x(k,1)*a(i,k);
            k++;
          }
        x(i,1)/= a(i,i);
      }
    for(i=n;i>=1;i--)      //second system
      {
        k=n;
        while(k>i)
          {
            x(i,1)-= x(k,1)*a(k,i);
            k--;
          }
        x(i,1)/= a(i,i);
      }
    return x;
  }

template <class treal>
ZMatrix<treal> cholesky(matsimZ<treal> &a,ZMatrix<treal> &b,int &defpos)
  {
    defpos= 1; size_t j=0,n= a.getNumberOfRows();
    while(defpos && (j<n))
      {
        j++; a(j,j)= prod(j,j,a);
        defpos= (a(j,j)>zero);
        if(defpos) colum(j,a);
      }
     ZMatrix<treal> x;
     if(defpos) x= calcularx(a,b);
     return x;
  }
#endif
