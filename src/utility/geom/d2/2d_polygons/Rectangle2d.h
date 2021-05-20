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
//Rectangle2d.h

#ifndef RECTANGLE2D_H
#define RECTANGLE2D_H

#include "Quadrilateral2d.h"

//! @ingroup GEOM
//
//! @brief Rectangle in a two dimensional space.
class Rectangle2d: public Quadrilateral2d
  {
  public:
    Rectangle2d(const double &b=1.0,const double &h=1.0): Quadrilateral2d()
      {
        push_back(Pos2d(0,0));
        push_back(Pos2d(b,0));
        push_back(Pos2d(b,h));
        push_back(Pos2d(0,h));
      }
    Rectangle2d(const Pos2d &o,const double &b=1.0,const double &h=1.0): Quadrilateral2d()
      {
        push_back(o+Vector2d(0,0));
        push_back(o+Vector2d(b,0));
        push_back(o+Vector2d(b,h));
        push_back(o+Vector2d(0,h));
      }
      //Rectangle2d(const Ref2d &ref, const double &b=1.0,const double &h=1.0) : Quadrilateral2d(ref,b),height(h) {}

    virtual GeomObj *clon(void) const
      { return new Rectangle2d(*this); }
/*     void Offset(const double &offset) */
/*       { */
/* 	Quadrilateral2d::Offset(offset); */
/*         height+=2.0*offset; */
/*       } */
/*     Rectangle2d GetOffset(const double &offset) const */
/*       { */
/*         Rectangle2d retval(*this); */
/*         retval.Offset(offset); */
/*         return retval; */
/*       } */
/*     inline const double &Base(void) const */
/*       { return lado; } */
/*     inline double &Base(void) */
/*       { return lado; } */
/*     inline const double &getHeight(void) const */
/*       { return height; } */
/*     inline double &getHeight(void) */
/*       { return height; } */
  };

#endif





