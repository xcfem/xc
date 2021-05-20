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
//Revolution3d.h
//Revolution in a three-dimensional space.

#ifndef REVOLUTION3D_H
#define REVOLUTION3D_H

#include "utility/geom/d1/Line3d.h"
#include "Trf3d.h"

class Pos3dArray;
class Pos3dArray3d;


//! @ingroup GEOM
//
//! @brief Revolution transformation.
class Revolution3d: public Trf3d
  {
    Line3d axis; //Revolution axis.
    GEOM_FT theta; //!< Swept angle.
    size_t ndiv; //number of divisions.
  public:
    Revolution3d(const Line3d &e,const GEOM_FT &th= 0.0,const size_t &nd= 1);
    Pos3dArray Aplica0d(const Pos3d &p) const;
    Pos3dArray Aplica1d(const Pos3dArray &m) const;
    Pos3dArray3d Aplica2d(const Pos3dArray &m) const;
    Pos3dArray operator()(const Pos3d &) const;
    Pos3dArray operator()(const Pos3dArray &) const;
  };

#endif
