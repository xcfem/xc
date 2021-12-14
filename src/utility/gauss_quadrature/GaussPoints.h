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
//GaussPoints.h

#ifndef	GAUSSPOINTS_H
#define	GAUSSPOINTS_H

#include <cmath>
#include <vector>
#include "GaussWeights1d.h"

typedef std::vector<GaussWeights1D> ConjPG; //Pesos de Gauss;


class GaussPoints : public ConjPG
  {
  public:
    GaussPoints() : ConjPG(0) {}
    GaussPoints(double x1, double x2, int n);
    inline size_t Size() const
      { return size(); }
    inline const GaussWeights1D &Get(int p) const
      { return (*this)[p]; }
    friend std::ostream& operator<<(std::ostream& o, const GaussPoints& gpts);
  };

#endif
















