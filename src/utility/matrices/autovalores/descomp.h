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
//descomp.h
//Decomposition of a matrix in two triangular matrices.

#ifndef DESCOMP_H
#define DESCOMP_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"

template <class treal>
void descomp(ZMatrix<treal> &a)
  {
   size_t i,j,k;
   const size_t n= a.getNumberOfRows();
   for(i=1;i<= n-1;i++)
     {
       for(j= i+1;j<= n;j++)
         {
	   if(i>1)
             for(k=1;k<= i-1;k++)
               a(i,j)-= a(i,k)*a(k,j);
	   a(i,j)/= a(i,i);
         }
       for(j=i+1;j<=n;j++)
	 for(k=1;k<= i;k++)
           a(j,i+1)-= a(j,k)*a(k,i+1);
     }
}
#endif
