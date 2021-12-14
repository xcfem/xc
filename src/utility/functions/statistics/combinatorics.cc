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
//Combinatorics.cc

#include "combinatorics.h"

unsigned long long factorial(const unsigned int &n)
  {
    unsigned long long i,f=1;
    for(i= 2;i<=n;i++)
      f*=i;
    return f;
  }

unsigned long long coeficiente_binomial(const unsigned int &n,const unsigned int &k)
  //Devuelve el valor del coeficiente binomial C(n,k):
  //              n!
  //  C(n,k)= -----------
  //           k!.(n-k)!
  {
    if((n==k) || (k==0)) return 1;
    unsigned long long i,num=n,denom;
    denom= factorial(n-k);
    for(i=n-1;i>k;i--)
      num*=i;
    return num/denom;
  }

