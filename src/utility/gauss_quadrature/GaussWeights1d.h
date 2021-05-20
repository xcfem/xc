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
#ifndef	PSOGSS1D_H
#define	PSOGSS1D_H

#include <iostream>


//! Class to store coordinates and weight in a one-dimensional space.
struct GaussWeights1D
  {
    double x, w;
    GaussWeights1D()
      : x(0.0), w(0.0) {}
    GaussWeights1D(double X, double W)
      : x(X), w(W) {}
    GaussWeights1D(const GaussWeights1D &gpw)
      : x(gpw.x), w(gpw.w) {}
    GaussWeights1D &operator=(const GaussWeights1D &other)
      {
        x= other.x;
        w= other.w;
        return *this;
      }
  };

std::ostream &operator<<(std::ostream &o, const GaussWeights1D &ggw);


#endif

