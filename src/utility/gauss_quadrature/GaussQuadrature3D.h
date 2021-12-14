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
#ifndef	CUADRATURA3D_H
#define	CUADRATURA3D_H

#include <vector>
#include "GaussPoints.h"
#include "GaussWeights3d.h"

class Pos3d;

typedef std::vector<GaussWeights3D> ConjPG3;

class GaussQuadrature3D : public ConjPG3
  {
    int num_dim;
  public:
    GaussQuadrature3D(const Pos3d &p1,const Pos3d &p2,int nx = 2, int ny = 1, int nz = 1);
    inline size_t Size() const
      { return size(); }
    inline const GaussWeights3D &Get(int p) const
      { return (*this)[p]; }
    inline int NumDim() const
      { return num_dim; }
    friend std::ostream& operator<<(std::ostream& o, const GaussQuadrature3D& gpts);
  };

#endif







