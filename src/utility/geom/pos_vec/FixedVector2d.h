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
//FixedVector2d.h

#ifndef FIXEDVECTOR2D_H
#define FIXEDVECTOR2D_H


#include "SlidingVector2d.h"


//! @ingroup GEOM
//
//! @brief Fixed vector in a two-dimensional space.
class FixedVector2d : public SlidingVector2d
  {
  };

FixedVector2d operator-(const FixedVector2d &v);

#endif
