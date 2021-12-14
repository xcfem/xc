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
//TriangularPrism3d.h

#ifndef TRIANGULARPRISM3D_H
#define TRIANGULARPRISM3D_H

#include "RightPrism3d.h"
#include "utility/geom/d2/Triangle3d.h"


//! @ingroup GEOM
//
//! @brief Triangular right prism.
class TriangularPrism3d : public RightPrism3d<Triangle3d>
  {
  public:
    typedef RightPrism3d<Triangle3d> triangular_prism;
  public:
    TriangularPrism3d(const Triangle3d &tr= Triangle3d(),const double &h=1.0);
    virtual GeomObj *getCopy(void) const;
    
  };
#endif






