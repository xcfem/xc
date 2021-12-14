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
//HEXAHEDRON3D.h

#ifndef HEXAHEDRON3D_H
#define HEXAHEDRON3D_H

#include <iostream>
#include "Polyhedron3d.h"

class HalfSpace3d;
class Polyhedron3d;

//! @ingroup GEOM
//
//! @brief Hexahedron.
class Hexahedron3d: public Polyhedron3d
  {
  public:
    Hexahedron3d(void);
    Hexahedron3d(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3, const Pos3d &p4, const Pos3d &p5,const Pos3d &p6, const Pos3d &p7);
    GeomObj *getCopy(void) const
      { return new Hexahedron3d(*this); }
    GEOM_FT getVolumeWithSign(void) const;
    GEOM_FT getVolume(void) const;
    bool In(const Pos3d &,const double &) const;
    void Print(std::ostream &os) const;
  };

#endif
