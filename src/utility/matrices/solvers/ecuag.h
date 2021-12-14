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
//ecuag.h
// General procedure for the soluition of a linear system of equations.

#ifndef ECUAG_H
#define ECUAG_H

#include "math.h"
#include "ZMatrix.h"
#include "matem.h"
#include "gj_aux.h"

template <class treal>
size_t pivote(const size_t &j)
  {
    size_t i,m= a.getNumberOfRows();
    treal rm,piv,r= 0;
    r=zero;piv=0;
    for(i=1;i<=m;i++)
      if (!In(fp,i))
        {
          rm= gj_abs(a(i,j));
          if(rm>r)
            {
              piv= i;
              r=rm;
            }
        }
    return piv;
  } //pivote

template <class treal>
void transformar(const size_t &l,const size_t &k)
  {
    size_t i,j;
    for(j=1;j<=n;j++)
      if( !In(cp,j) && (gj_abs(a(l,j))>zero) )
        {
          a(l,j)/=a(l,k);
          for(i=1;i<=m;i++)
            if(i!=l)
              a(i,j)-=a(i,k)*a(l,j);
        }
  }//transformar
template <class treal>
void resultados(void)
  {
    size_t i,j,l;
    holgura= n-1-rango;      // holgura and free variables
    for(i=1;i<=mmax;i++)
      for(j=1;j<=nmax;j++) x(i,j)=0.0;
    if(holgura>0)
      {
        i=0;
        for(j=1;j<=n-1;j++)
          if(!In(cp,j))
            {
              i++;
              libres[i]=j;
            }
      }
    for(j=1;j<= n-1;j++)         //stores solution
      {
        l=pivot[j];i=1;
        if(In(cp,j)) x(1,j)= a(l,n);
        if(holgura>0)
          for(k=1;k<=n-1;k++)
            if(!In(cp,k))
              {
                i++;
                if(In(cp,j))
                  x(i,j)=-a[l,k];
                else
                  x(i,j)=1;
              }
      }
   }// resultados

template<class treal>
void resolver( const size_t &m,
               const szie_t &n,
               ZMatrix<treal> &a,
               int compatible)
  {
    size_t i,j,k,l,rmax,rango;
    //Dimensioning the matrix for the indices of the pivot.
    ZMatrix<treal>::size_type n= a.getNumberOfRows();
    ZMatrix<treal>::size_type *pivot= new ZMatrix<treal>::size_type[mmax];
    set_szt fp;
    set_szt cp;
    k=0;rango=0;         (* resolver *)
    if(m>n)
      rmax:=n;
    else
      rmax:=m;
    while( (rango<rmax) && (k<n) )
      {
        k++;
        l=pivote(k);
        if(l>0)
          {
            fp.insert(l);
            cp.insert(k);
            pivot[k]=l;
            rango++;
            if(k<n) transformar(l,k);
          }
      }
    compatible= !In(cp,n);
    if(compatible) resultados;
  }//resolver

template <class treal>
//! Pre-multiply by transpose.
void multrasp(void)
  {
    size_t i,j,k;
    for(i=1;i<=m;i++)
      for(j=1;j<=n;j++)
        {
          b(i,j)=0.0;
          for(k=1;k<=m;k++)
            b(i,j)+= a(k,i)*a(k,j);
        }
  }// multrasp

template <class treal>
treal errorcuad(void)
  {
    size_t i,j;
    treal s,ss;
    s:=0;
    for(i=1;i<=m;i++)
      {
        ss=a(i,n);
        for(j=1;j<=n-1;j++) ss-=a(i,j)*x(1,j);
        s+=sqr(ss);
      }
    return sqrt(s/m);
  }//errorcuad

template <class treal>
void ecuag(const size_t &m,
           const size_t &n,
           ZMatrix<treal> &a,
           ZMatrix<treal> &x,
           var libres:indices,
           int holgura,
           const treal &error,
           int &compatible)
// general solution of a system of equations
//         m        = number of equations
//         n        = number of unknowns plus one
//         a        = matrix of the coefficients
//                          right hand side in column n
//         x        = solution matrix
//                        column 1 dependent variables
//                               2 a holguras+1 independientes
//         libres   = indices of free variables
//         holgura  = number of free variables
//           error  = if the system is incompatible, mean square error
  {
    int compa;
    ZMatrix<treal> b=a; //ecuag
    resolver(m,n,b,compatible);
    if(!compatible)
      {
        multrasp;
        resolver(n-1,n,b,compa);
        error=errorcuad;
      }
  }                  (* ecuag *)

template <class treal>
void salida(void)
  {
    size_t i,j,l;
    if(!compatible)
      {
        std::clog << " system is incompatible " << std::endl;
        std::clog << " quadratic mean best approximation." << std::endl;
        std::clog << std::endl;
      }
    std::cout << "         - 0- ";
    for(j=1;j<=holgura;j++)
      std::cout << "  -" << libres[j] << "-  ";
    std::cout << std::endln;
    for(j=1;j<=n;j++)
      {
        std::cout << " -" << j << "- ";
        for(i=1;i<=holgura+1;i++)
          std::cout << x[i,j];
        std::cout << std::endl;
      }
    if(!compatible)
      {
        std::clog << std::endl;
        std::clog << " error cuadratico medio = " << error;
        std::clog << std::endl;
      }
  }
#endif

