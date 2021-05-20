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
//ThreePoints3d.h

#ifndef THREEPOINTS3D_H
#define THREEPOINTS3D_H

#include "../pos_vec/Pos3d.h"


//! @ingroup GEOM
//
//! @brief Data structure that stores three points.
class ThreePoints3d : public ProtoGeom
  {
    Pos3d _org;
    Pos3d _p1;
    Pos3d _p2;
  protected:
    //void salva_miembros(std::ostream &os,const std::string &indent) const;
  public:
    ThreePoints3d(void): ProtoGeom() {}
    ThreePoints3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3): ProtoGeom()
      { ThreePoints3d(p1,p2,p3); }
    const Pos3d &Org(void) const
      { return _org; }
    const Pos3d &P1(void) const
      { return _p1; }
    const Pos3d &P2(void) const
      { return _p2; }
  };

#endif
