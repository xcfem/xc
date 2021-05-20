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
//Rotation3d.h
//Rotation in a three-dimensional space.

#ifndef ROTATION3D_H
#define ROTATION3D_H

#include "Trf3d.h"

class Line3d;


//! @ingroup GEOM
//
//! @brief Rotación en tres dimensiones.
class Rotation3d: public Trf3d
  {
  public:
    Rotation3d(void);
    Rotation3d(const Line3d &,const GEOM_FT &);
  };

#endif
