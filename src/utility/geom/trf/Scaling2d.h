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
//Scaling2d.h
//2D scaling.

#ifndef SCALING2D_H
#define SCALING2D_H

#include "Trf2d.h"


//! @ingroup GEOM
//
//! @brief Scaling in a two-dimensional space.
class Scaling2d: public Trf2d
  {
  public:
    explicit Scaling2d(const GEOM_FT &fe= 1.0);
  };

#endif
