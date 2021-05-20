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
//Orto.h
// Orthogonalization procedure for the solution
// of a system of linear equations

#ifndef ORTO_H
#define ORTO_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/gj_aux.h"

template <class treal>
treal escalar(const size_t &i,const size_t &j,const ZMatrix<treal> &a)
  {
    treal s=0.0,n= a.getNumberOfRows();
    for(size_t k=1;k<=n;k++) s+=a(i,k)*a(j,k);
    return s;
  }

template <class treal>
ZMatrix<treal> calcularx( const ZMatrix<treal> &a,
                          ZMatrix<treal> &b,
                          const ZMatrix<treal> &r)
  {
    size_t i,n= a.getNumberOfRows();
    ZMatrix<treal> x(n,1,0.0);
    for(i=1;i<=n;i++) b(i,1)*=r(i,1);
    for(i=1;i<=n;i++)
      for(size_t k=1;k<=n;k++) x(i)+= a(k,i)*b(k,1);
    return x;
  }

template <class treal>
ZMatrix<treal> ortoresol(ZMatrix<treal> &a,ZMatrix<treal> &b,int &regular)
  {
    size_t   i=0,l=0,n= a.getNumberOfRows();
    ZMatrix<treal> r(n,1,0.0),landa(n,1,0.0);
    regular=1;
    while( regular && (l<n))
      {
        l++; i=1;
        while(i<l)
          {
            landa(i,1)= r(i,1)*escalar(i,l,a);
            i++;
          }
        i=1;
        while(i<l)
          {
            for(size_t k=1;k<=n;k++) a(l,k)-= landa(i,1)*a(i,k);
            b(l,1)-= landa(i,1)*b(i,1);
            i++;
          }
        r(l,1)= escalar(l,l,a);
        regular= (gj_abs(r(l,1)) > zero);
        if(regular) r(l,1)= gj_inv(r(l,1));
      }
    ZMatrix<treal> x;
    if(regular) x= calcularx(a,b,r);
    return x;
  }
#endif
