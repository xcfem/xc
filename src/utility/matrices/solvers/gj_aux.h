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
//gj_aux.h
//Auxiliary functions for the Gauss and Jordan methods
//for solving linear systems of equations.

#ifndef GJ_AUX_H
#define GJ_AUX_H

#include "math.h"
const double zero = 1e-7;

//The following functions must be defined for any object intended to be
//used with the routines from gauss.h and jordan.h

inline double gj_abs(const double &r)
  { return fabs(r); }
inline double gj_inv(const double &r)
  { return 1/r; }

#endif
