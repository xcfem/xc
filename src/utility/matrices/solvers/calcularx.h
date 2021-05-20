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
//calcularx.h
// Backward substitution procedure for the Gauss and Crout methods
// for the solution of linear equations systems.

#ifndef CALCULARX_H
#define CALCULARX_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include <set>
#include "utility/matrices/solvers/gj_aux.h"

template <class treal>
ZMatrix<treal> calcularx( const ZMatrix<treal> &a,
                          const ZMatrix<treal> &b,
                          size_t *pivot)
  {
    size_t j,l,k,n= a.getNumberOfRows(),m= b.getNumberOfColumns();
    ZMatrix<treal> x(n,m,0.0);
    for(j=n;j>=1;j--)
      {
        l=pivot[j-1];
        for(size_t c=1;c<=m;c++) x(j,c)=b(l,c);
        k=j;
        while(k<n)
          {
            k++;
            size_t c;
            for(c=1;c<=m;c++) x(j,c)-= a(l,k)*x(k,c);
          }
      }
    return x;
  }
#endif
