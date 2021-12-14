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
//Dir3d.h

#ifndef DIR3D_H
#define DIR3D_H

#include "../cgal_types.h"

class Vector3d;
class FT_matrix;


//! @ingroup GEOM
//
//! @brief Dirección en el espacio de tres dimensiones.
class Dir3d
  {
    CGDirection_3 cgd;
  public:
    explicit Dir3d(const CGDirection_3 &d)
      : cgd(d) {}
    Dir3d(GEOM_FT x,GEOM_FT y,GEOM_FT z);
    explicit Dir3d(const Vector3d &v);
    virtual bool operator==(const Dir3d &) const;
    const CGDirection_3 &ToCGAL(void) const
      { return cgd; }
    void Neg(void)
      { cgd=-cgd; }
    inline GEOM_FT operator()(const size_t &i) const
      { return cgd.delta(i-1); }
    inline GEOM_FT dx(void) const
      { return cgd.dx(); }
    inline GEOM_FT dy(void) const
      { return cgd.dy(); }
    inline GEOM_FT dz(void) const
      { return cgd.dz(); }
    FT_matrix getMatrix(void) const;
    Vector3d GetVector(void) const;
    friend std::ostream &operator << (std::ostream &stream,const Dir3d &n);
    inline virtual ~Dir3d(void) {}
  };

inline bool parallel(const Dir3d &v1,const Dir3d &v2)
  { return (v1==v2); } 

#endif
