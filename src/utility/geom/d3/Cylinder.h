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
//Cylinder.h
//Sólido de extrusión generado por un polígono.

#ifndef SOLIDEXTRUPLGNO3D_H
#define SOLIDEXTRUPLGNO3D_H

#include "SolidExtru3d.h"
#include "utility/geom/d2/Circle3d.h"


//! @ingroup GEOM
//
//! @brief Cylinder.
class Cylinder : public SolidExtru3d<Circle3d>
  {
  public:
    typedef SolidExtru3d<Circle3d> solid_extru_cil;
  public:
    Cylinder(void);
    Cylinder(const Circle3d &secc,const GEOM_FT &lng);

    virtual GeomObj *getCopy(void) const;

    BND3d Bnd(void) const;
    FT_matrix I(void) const;
    Pos3d getCenterOfMass(void) const;
  };






#endif
