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
#ifndef	CUADRATURA2D_H
#define	CUADRATURA2D_H

#include <vector>
#include "GaussWeights2d.h"
#include "GaussPoints.h"

typedef std::vector<GaussWeights2D> ConjPG2;


class GaussQuadrature2D : public ConjPG2
  {
  public:
    GaussQuadrature2D( double x1, double x2,double y1, double y2,int nx, int ny);
    inline size_t Size() const
      { return size(); }
    inline const GaussWeights2D &Get(int p) const
      { return (*this)[p]; }
    friend std::ostream& operator<<(std::ostream& o, const GaussQuadrature2D& gpts);
  };

const GaussQuadrature2D TwoByTwo(-1.0,1.0,-1.0,1.0,2,2);

#endif







