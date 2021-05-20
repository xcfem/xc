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
//pivote.h
// Procedure for obtaining the pivot in the methods
// of Gauss and Jordan for the solution of a linear system of equations.

#ifndef PIVOTE_H
#define PIVOTE_H

#include <cmath>
#include "utility/matrices/ZMatrix.h"
#include <set>
#include "utility/matrices/solvers/gj_aux.h"

typedef std::set<size_t, std::less<size_t> > set_szt;

inline int In(const set_szt &fp,const size_t &i)
  {
    set_szt::iterator j;
    j = fp.find(i);
    if ( j == fp.end() )
        return 0;
    else
        return 1;
  }

template <class treal>
size_t pivote(const size_t &j,const ZMatrix<treal> &a,const set_szt &fp)
  {
    size_t i,k=0,n= a.getNumberOfRows();
    treal amax=zero;
    for(i= 1;i<=n;i++)
      if (!In(fp,i) && ( amax<gj_abs(a(i,j)) ) )
	{ k=i; amax=gj_abs(a(i,j)); }
    return k;
  }
#endif

