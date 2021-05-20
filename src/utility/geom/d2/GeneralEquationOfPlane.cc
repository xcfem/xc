//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GeneralEquationOfPlane.cc

#include "GeneralEquationOfPlane.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Pos2d.h"

// GeneralEquationOfPlane::GeneralEquationOfPlane(const GEOM_RT &a,const GEOM_RT &b,const GEOM_RT &c,const GEOM_RT &d)
//   : v(4,0.0) { v[0]= d; v[1]= a; v[2]= b; v[3]= c; }

GeneralEquationOfPlane::GeneralEquationOfPlane(const GEOM_FT &fa,const GEOM_FT &fb,const GEOM_FT &fc,const GEOM_FT &fd)
  : v(4,0.0) 
  { 
//     const GEOM_RT a= fa.numerator()*fb.denominator()*fc.denominator()*fd.denominator();
//     const GEOM_RT b= fa.denominator()*fb.numerator()*fc.denominator()*fd.denominator();
//     const GEOM_RT c= fa.denominator()*fb.denominator()*fc.numerator()*fd.denominator();
//     const GEOM_RT d= fa.denominator()*fb.denominator()*fc.denominator()*fd.numerator();
//     v[0]= d; v[1]= a; v[2]= b; v[3]= c; 
    v[0]= fd; v[1]= fa; v[2]= fb; v[3]= fc; 
  }

GEOM_FT GeneralEquationOfPlane::Eval(const Pos3d &p) const
  { return a()*p.x()+b()*p.y()+c()*p.z()+d(); }

//! @brief Return the x coordinate of the point of the plane
//! whose y and z coordinates are given.
GEOM_FT GeneralEquationOfPlane::x(const GEOM_FT &y,const GEOM_FT &z) const
      { return -(d()+b()*y+c()*z)/a(); }

//! @brief Return the x coordinate of the point of the plane
//! whose y and z coordinates are given.
GEOM_FT GeneralEquationOfPlane::x(const Pos2d &p) const
  { return x(p.x(),p.y()); }

//! @brief Return the y coordinate del point of the plane
//! whose x and z coordinates are given.
GEOM_FT GeneralEquationOfPlane::y(const GEOM_FT &x,const GEOM_FT &z) const
      { return -(d()+a()*x+c()*z)/b(); }

//! @brief Return the y coordinate del point of the plane
//! whose x and z coordinates are given.
GEOM_FT GeneralEquationOfPlane::y(const Pos2d &p) const
  { return y(p.x(),p.y()); }

//! @brief Return the z coordinate del point of the plane
//! whose x and y coordinates are given.
GEOM_FT GeneralEquationOfPlane::z(const GEOM_FT &x,const GEOM_FT &y) const
      { return -(d()+a()*x+b()*y)/c(); }

//! @brief Return the z coordinate del point of the plane
//! whose x and y coordinates are given.
GEOM_FT GeneralEquationOfPlane::z(const Pos2d &p) const
  { return z(p.x(),p.y()); }
