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
//souriau.h
// Computation of the characteristic equation of a matrix using the Souriau
// procedure.

#ifndef SOURIAU_H
#define SOURIAU_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"

template <class treal>
ZMatrix<treal> calbk(const ZMatrix<treal> &ak,const treal &tk)
  {
    size_t n= ak.getNumberOfRows();
    ZMatrix<treal> b(ak);
    for(size_t i=1;i<=n;i++)
      b(i,i)+=tk;
    return b;  
  }

template <class treal>  
//! @brief Souriau procedure to compute the characteristic equation of a matrix.
//	@param a:	matrix to obtain the characteristic equation of.
//	Returns:
//		vector whose element with index (k,1) is the
//		coefficient of degree n-k of the polynomial.
vectorZ<treal> souriau(ZMatrix<treal> &a)
  {
    size_t n= a.getNumberOfRows();
    vectorZ<treal> t(n+1,1);    
    ZMatrix<treal>  ak(n,n,0.0),b(n,n);
    t[0]= 1;
    for(size_t k=1;k<=n;k++)
      {
        b= calbk(ak,t[k-1]);
        ak= a*b;
        t[k]=-ak.Traza()/k;
      }
    return t;
  }
#endif
