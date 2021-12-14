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
#ifndef	PSOGSS2D_H
#define	PSOGSS2D_H

#include <iostream>
#include "GaussWeights1d.h"

//! @brief Structure to store coordinates and weight in a
//! two-dimensional space.
struct GaussWeights2D: public GaussWeights1D
  {
    double y;
    GaussWeights2D(void)
     : GaussWeights1D(), y(0.0) {}
    GaussWeights2D(const GaussWeights2D& gpw)
     : GaussWeights1D(gpw), y(gpw.y) {}
    GaussWeights2D(double X, double Y, double W)
     : GaussWeights1D(X,W), y(Y) {}
    GaussWeights2D &operator=(const GaussWeights2D &other)
      {
        GaussWeights1D::operator=(other);
        y= other.y;
        return *this;
      }
  };

std::ostream& operator<<(std::ostream& o, const GaussWeights2D &ggw);

#endif
