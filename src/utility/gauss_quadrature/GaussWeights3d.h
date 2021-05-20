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
#ifndef	PSOGSS3D_H
#define	PSOGSS3D_H

#include <iostream>
#include "GaussWeights2d.h"

struct GaussWeights3D: public GaussWeights2D
  {
    double z;
    GaussWeights3D()
      : GaussWeights2D(), z(0.0) {}
    GaussWeights3D(const GaussWeights3D& ggw)
      : GaussWeights2D(ggw), z(ggw.z) {}
    GaussWeights3D(const double &X, const double &Y, const double &Z, const double &W)
      : GaussWeights2D(X,Y,W), z(Z) {}
    GaussWeights3D &operator=(const GaussWeights3D &other)
      {
        GaussWeights2D::operator=(other);
        z= other.z;
        return *this;
      }
  };

std::ostream& operator<<(std::ostream& o, const GaussWeights3D &ggw);

#endif
