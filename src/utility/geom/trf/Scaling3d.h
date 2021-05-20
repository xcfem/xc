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
//Scaling3d.h
//Scaling de un objeto en 3 dimensiones.

#ifndef SCALING3D_H
#define SCALING3D_H

#include "Trf3d.h"


//! @ingroup GEOM
//
//! @brief Scaling en tres dimensiones.
class Scaling3d: public Trf3d
  {
  public:
    explicit Scaling3d(const GEOM_FT &fe= 1.0);
  };

#endif
