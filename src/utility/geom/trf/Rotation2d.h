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
//Rotation2d.h
//Rotation in a two-dimensional space.

#ifndef ROTATION2D_H
#define ROTATION2D_H

#include "Trf2d.h"


//! @ingroup GEOM
//
//! @brief Rotación en dos dimensiones.
class Rotation2d: public Trf2d
  {
  public:
    explicit Rotation2d(const double &ang_rad= 0.0);
  };

#endif
