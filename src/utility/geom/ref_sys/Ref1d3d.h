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
//Ref1d3d.h

#ifndef REF1D3D_H
#define REF1D3D_H

#include "Ref.h"
#include "../coo_sys/Rect1d3dCooSys.h"
#include "../d3/GeomObj3d.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Dir3d.h"

class Line3d;
class Ref2d3d;
class Ref3d3d;

//! @ingroup SisRef
//! 
//! @brief 1D reference system defined in a
//! three-dimensional one.
class Ref1d3d : public Ref<Rect1d3dCooSys>
  {
  public:
    typedef GeomObj3d::list_Pos3d list_Pos3d;
    typedef Ref<Rect1d3dCooSys> BaseRef;
  public:
    Ref1d3d(void);
    explicit Ref1d3d(const Pos3d &o);
    Ref1d3d(const Pos3d &,const Rect1d3dCooSys &);
    Ref1d3d(const Pos3d &o,const Vector3d &vX);
    Ref1d3d(const Pos3d &o,const Dir3d &dirX);
    Ref1d3d(const Pos3d &o,const Pos3d &p);
    explicit Ref1d3d(const Ref2d3d &);
    explicit Ref1d3d(const Ref3d3d &);

    Vector3d getIVector(void) const;
    Line3d getXAxis(void) const;

    virtual ~Ref1d3d(void)
      {}
  };

#endif






