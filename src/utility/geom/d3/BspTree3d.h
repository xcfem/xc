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
//BSPTREE3D.h

#ifndef BSPTREE3D_H
#define BSPTREE3D_H

#include "utility/geom/d2/Plane.h"
#include "utility/geom/d2/Polygon3d.h"
#include <list>


class BspTree3d
  {
  public:
    typedef std::list<Polygon3d> polygons_list;
    typedef polygons_list::value_type polygon;

    friend void Build_BSP_Tree(BspTree3d *,const polygons_list &);

  private:
    Plane particion;         //Partition plane.
    polygons_list polygons; //List of the polygons that lie 
                               //in the partition plane.
    BspTree3d *front,*back; //Pointer to the children.
  public:
    BspTree3d(void)
      : particion(), polygons(), front(nullptr), back(nullptr) {}
    explicit BspTree3d(const polygons_list &polygons)
      : particion(), polygons(), front(nullptr), back(nullptr)
      { Build_BSP_Tree(this,polygons); }
    inline BspTree3d *Delante(void) const
      { return front; }
    inline BspTree3d *Detras(void) const
      { return back; }
    inline void PutDelante(BspTree3d *ptr)
      { front= ptr; }
    inline void PutDetras(BspTree3d *ptr)
      { back= ptr; }
  };

#endif
