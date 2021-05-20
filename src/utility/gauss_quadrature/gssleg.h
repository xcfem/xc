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
#ifndef	GSSLEG_H
#define	GSSLEG_H

#include <cmath>
#include <cstdio>
#include <vector>

typedef std::vector<double> v_double;

/* I originally used the Gauss-Legendre routine 'gauleg' from
   Numerical Recipes in C.  Since that is copyrighted, I decided
   to write my own.  In doing so I think I found a couple errors
   in the Gauss-Legendre recurrence relation in NRiC (in the
   theory section, not in the code itself). */

void gss_leg(double x1, double x2,v_double &x,v_double &w);

#endif
