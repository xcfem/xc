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
//Dir2d.h

#ifndef DIR2D_H
#define DIR2D_H

#include "../cgal_types.h"
#include <iostream>

class Vector2d;
class FT_matrix;


//! @ingroup GEOM
//
//! @brief Dirección en dos dimensiones.
class Dir2d
  {
    CGDirection_2 cgd;
  public:
    explicit Dir2d(const CGDirection_2 d): cgd(d) {}
    Dir2d(const GEOM_FT &x,const GEOM_FT &y);
    explicit Dir2d(const double &ang);
    explicit Dir2d(const Vector2d &v);
    virtual bool operator==(const Dir2d &) const;
    const CGDirection_2 &ToCGAL(void) const
      { return cgd; }
    void Neg(void)
      { cgd= -cgd; }
    inline GEOM_FT operator()(const size_t &i) const
      { return cgd.delta(i-1); }
    inline GEOM_FT dx(void) const
      { return cgd.dx(); }
    inline GEOM_FT dy(void) const
      { return cgd.dy(); }
    //! @brief Return the perpendicular direction in counterclockwise sense.
    inline Dir2d Perpendicular(void) const
      { return Dir2d(-dy(),dx()); }
    FT_matrix getMatrix(void) const;
    Vector2d GetVector(void) const;
    friend std::ostream &operator << (std::ostream &stream,const Dir2d &n);
    inline virtual ~Dir2d(void) {}
  };
inline Dir2d operator-(const Dir2d &d)
  {
    Dir2d retval(d);
    retval.Neg();
    return retval;
  }

inline bool parallel(const Dir2d &v1,const Dir2d &v2)
  { return ((v1==v2) || (v1==-v2)); } 

#endif
