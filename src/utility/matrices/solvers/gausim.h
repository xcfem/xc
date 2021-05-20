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
//Gauss procedure to solve a linear system of equations with a symmetric
//coefficient matrix.

#ifndef GAUSIM_H
#define GAUSIM_H

#include <cmath>
#include "utility/matrices/matsimZ.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/gj_aux.h"

template <class treal>
void elim( matsimZ<treal> &a,
           ZMatrix<treal> &b,
           const size_t &j)
  {
    size_t i,n= a.getNumberOfRows();
    i=j;
    while(i<n)
      {
        i++; treal s= a(j,i)/a(j,j);
        if( gj_abs(s)> zero)
          {
            for(size_t k=i;k<=n;k++) a(i,k)-= s*a(j,k);
            b(i)-= s*b(j);
            a(j,i)= s;
          }
      }
    b(j)/= a(j,j);
  }

template <class treal>
ZMatrix<treal> calcularx( const matsimZ<treal> &a,
                          const ZMatrix<treal> &b)
  {
    size_t l,k,n= a.getNumberOfRows();
    ZMatrix<treal> x(n,1,0.0);
    for(l=n;l>=1;l--)
      {
        k= l; x(l)=b(l);
        while(k<n)
          {
            k++;
            x(l)-= a(l,k)*x(k);
          }
      }
    return x;
  }

template <class treal>
ZMatrix<treal> gausim(matsimZ<treal> &a,ZMatrix<treal> &b,int &simreg)
  {
    size_t j= 0,n= a.getNumberOfRows();
    simreg=1;
    while(simreg && (j<n))
      {
        j++;
        simreg= (gj_abs(a(j,j))>zero);
        if(simreg) elim(a,b,j);
      }
    ZMatrix<treal> x;
    if(simreg) x= calcularx(a,b);
    return x;
  }
#endif


