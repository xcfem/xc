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
//Translation3d.h
//Translation in a three-dimensional space.

#ifndef TRANSLATION3D_H
#define TRANSLATION3D_H

#include "Trf3d.h"


//! @ingroup GEOM
//
//! @brief Traslación en tres dimensiones.
class Translation3d: public Trf3d
  {
  public:
    Translation3d(void);
    explicit Translation3d(const Vector3d &v);
  };

#endif
