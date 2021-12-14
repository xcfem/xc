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
//potiter.h

#ifndef POTITER_H
#define POTITER_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/norm.h"

template <class treal>
treal valorp(const ZMatrix<treal> &a)
  {
    const treal epsi = 1e-5;
    size_t n= a.getNumberOfRows();
    ZMatrix<treal> v(n,1,1.0),va(n,1);
    treal ln=10*epsi,la= 0.0;
    while (gj_abs(la-ln)>epsi)
      {
        va= v; la= ln;
        v= a*va;
        ln= norm(v);
        v*=(gj_inv(ln));
      }
    if (v(1)*va(1)< 0)
       return -ln;
    else
       return ln;
  }
#endif
