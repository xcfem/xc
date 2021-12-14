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
//TETRAHEDRON3D.h

#ifndef TETRAHEDRON3D_H
#define TETRAHEDRON3D_H

#include <iostream>
#include "PolyhedronBase.h"
#include "PolygonMap.h"

class HalfSpace3d;
class Polyhedron3d;

//! @ingroup GEOM
//
//! @brief Tetrahedron.
class Tetrahedron3d: public PolyhedronBase
  {
    CGTetrahedron_3 cgtetrahedron;
  public:
    Tetrahedron3d(void);
    Tetrahedron3d(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3);
    Tetrahedron3d(const HalfSpace3d &, const HalfSpace3d &,const HalfSpace3d &, const HalfSpace3d &);
    GeomObj *getCopy(void) const
      { return new Tetrahedron3d(*this); }
    Polyhedron3d getPolyhedron3d(void) const;
    GEOM_FT getArea(void) const;
    GEOM_FT getVolumeWithSign(void) const;
    GEOM_FT getVolume(void) const;
    double GetMax(short unsigned int i) const;
    double GetMin(short unsigned int i) const;
    bool In(const Pos3d &,const double &) const;
    void Print(std::ostream &os) const;
  };

PolygonMap<CGPolyhedron_3> getPolygonMap(const Tetrahedron3d &t);

#endif
