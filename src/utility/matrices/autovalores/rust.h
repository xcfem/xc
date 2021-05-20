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
//rust.h
// Computation of the characteristic equation of a matrix using the Rustihauser
// procedure.

#ifndef RUST_H
#define RUST_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include "utility/matrices/util/matem.h"


template <class treal>
void rust(ZMatrix<treal> &a,treal &epsi)
  {
    const int maxiter = 500;
    int j;
    treal max;
    j= 0;
    do
      {
        descomp(a);
        compos(a,max);
        j++;
      }
    while (!(max<epsi) || (j>maxiter));
    epsi= max;
  }
#endif
