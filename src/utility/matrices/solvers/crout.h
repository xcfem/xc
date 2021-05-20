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
//Crout.h
//Crout procedure for the solution of a linear system of equations.

#ifndef CROUT_H
#define CROUT_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/pivote.h"
#include "utility/matrices/solvers/calcularx.h"

template <class treal>
void row( const size_t &j,
           ZMatrix<treal> &a,
           ZMatrix<treal> &b,
           const set_szt &fp,
           size_t *pivot)
  {
    size_t i,k,l=pivot[j],n=a.getNumberOfRows();
    treal s,r= gj_inv(a(l,j));
    for(k=j+1;k<=n;k++)
      {
        s=a(l,k); i=1;
        while(i<j)
          {
            s-=a(l,i)*a(pivot[i],k);
            i++;
          }
        a(l,k)=s*r;
      }
    s=b(l,1); i=1;
    while(i<j)
      {
        s-=a(l,i)*b(pivot[i],1);
        i++;
      }
    b(l,1)=s*r;
  }

template <class treal>
void column( const size_t &j,
            ZMatrix<treal> &a,
            ZMatrix<treal> &b,
            const set_szt &fp,
            size_t *pivot)
  {
    size_t l,i,n= a.getNumberOfRows();
    for(i=1;i<=n;i++)
    if (!In(fp,i))
      if(j<=n)
        for(l=1;l<=j-1;l++)
          a(i,j)-=a(i,l)*a(pivot[l],j);
      else
        for(l=1;l<=j-1;l++)
          b(i,1)-=a(i,l)*b(pivot[l],1);      
  }

template <class treal>
ZMatrix<treal> crout(ZMatrix<treal> &a,ZMatrix<treal> &b,int &regular)
  {
    //Dimensions of the pivot indices matrix.
    typedef typename ZMatrix<treal>::size_type size_type;
    const size_type n= a.getNumberOfRows();
    size_type *pivot= new size_type[n];
    set_szt fp;
    regular=1; size_t j=0;
    while(regular && (j<n))
      {
        j++;
        pivot[j]=pivote(j,a,fp);
        regular=(pivot[j]>0);
        if(regular)
          {
            fp.insert(pivot[j]);
            row(j,a,b,fp,pivot); column(j+1,a,b,fp,pivot);
          }
      }
    ZMatrix<treal> x;
    if(regular) x= calcularx(a,b,pivot);
    delete[] pivot;
    return x;
  }
#endif




