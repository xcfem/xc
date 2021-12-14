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
//compos.h
//Computing the product of two triangular matrices.

#ifndef COMPOS_H
#define COMPOS_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/solvers/gj_aux.h"

template <class treal>
void compos(ZMatrix<treal> &a,treal &max)
  {
    size_t i,j,k,l,n= a.getNumberOfRows();
    treal s,xx;
    max=0;
    for(i=1;i<= n;i++)
      for(j=1;j<= n;j++)
        {
          if(i<j)
            { s= 0.0;k=j; }
          else
            {s= a(i,j);k=i+1;}
          if(k<=n)
            for(l=k;l<=n;l++) s+=a(i,l)*a(l,j);
          a(i,j)= s;
          if(i<j)
            {
              xx= gj_abs(s);
              if(xx>max) max=xx;
            }
        }
  }
#endif
