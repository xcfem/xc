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
//Reflection3d.h
//Reflection in three-dimensional space.

#ifndef REFLECTION3D_H
#define REFLECTION3D_H

#include "Trf3d.h"

class Plane;


//! @ingroup GEOM
//
//! @brief Reflexión en 3d.
class Reflection3d: public Trf3d
  {
  private:
    static FT_matrix reflection3d_matrix(const Pos3d &Q,const Vector3d &d);

  public:
    Reflection3d(void);
    Reflection3d(const Pos3d &Q,const Vector3d &d);
    Reflection3d(const Plane &M);
  };

#endif
