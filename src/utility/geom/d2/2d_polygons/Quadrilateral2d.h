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
//Quadrilateral2d.h

#ifndef QUADRILATERAL2D_H
#define QUADRILATERAL2D_H

#include "Polygon2d.h"

class Triangle2d;
class Pos2dArray;

// Orden de los vértices 1->2->3->4.
//
// 4 +---------------+ 3
//   |               |
//   |               |
//   |               |
// 1 +---------------+ 2


//! @ingroup GEOM
//
//! @brief Cuadrilátero en dos dimensiones.
class Quadrilateral2d: public Polygon2d
  {
  public:
    Quadrilateral2d(void): Polygon2d() 
      {
        push_back(Pos2d(0,0));
        push_back(Pos2d(1,0));
        push_back(Pos2d(1,1));
        push_back(Pos2d(0,1));
      }
    Quadrilateral2d(const Pos2d &p1,const Pos2d &p2,const Pos2d &p3,const Pos2d &p4);

    virtual GeomObj *clon(void) const
      { return new Quadrilateral2d(*this); }
    inline void push_back(const Pos2d &p)
      {
        if(getNumVertices()<5)
	  Polygon2d::push_back(p);
      }
    Triangle2d getFirstTriangle(void) const;
    Triangle2d getSecondTriangle(void) const;
    Pos2dArray genMesh(int n1,int n2) const;
    Pos2dArray genBilinMesh(const size_t &n1,const size_t &n2) const;
  };
#endif
