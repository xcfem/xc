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
//norm.h

#ifndef NORM_H
#define NORM_H

#include "math.h"
#include "ZMatrix.h"
#include "utility/utils/misc_utils/matem.h"

template <class treal>
treal norm(const ZMatrix<treal> &x)
  {
    treal r;
    size_t n= x.getNumberOfRows();
    r= gj_abs(x(1));
    for(size_t i= 2;i<=n;i++)
      if (r<gj_abs(x(i))) r= gj_abs(x(i));
    return r;
  }

#endif
