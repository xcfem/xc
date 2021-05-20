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
//FixedVector3d.h

#ifndef FIXEDVECTOR3D_H
#define FIXEDVECTOR3D_H


#include "SlidingVector3d.h"


//! @ingroup GEOM
//
//! @brief Fixed vector in a three-dimensional space.
class FixedVector3d : public SlidingVector3d
  {
  };

FixedVector3d operator-(const FixedVector3d &v);

#endif
