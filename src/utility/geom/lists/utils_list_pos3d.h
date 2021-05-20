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
//utils_list_pos3d.h

#ifndef UTILS_LIST_POS3D_H
#define UTILS_LIST_POS3D_H

#include "utility/geom/GeomObj.h"
#include "utility/geom/pos_vec/Vector3d.h"

class BND3d;
class Trf3d;
class Vector3d;

//Auxiliary functions for manipulation of position lists.


//! @ingroup GEOM
//
void move(GeomObj::list_Pos3d &l,const Vector3d &);
void transform(GeomObj::list_Pos3d &l,const Trf3d &);
Pos3d getPMax(const GeomObj::list_Pos3d &);
Pos3d getPMin(const GeomObj::list_Pos3d &);
BND3d getBnd(const GeomObj::list_Pos3d &);

//! @brief Returns the maximum corner angle quality parameter.
template <class InputIterator>
double getMaxCornerAngle(InputIterator first, InputIterator last)
  {
    std::deque<Vector3d> vectors;
    std::deque<Pos3d>::const_iterator i= first;
    Pos3d ptA= *i;
    i++;
    for(;i!=last;i++)
      {
        Pos3d ptB= *i;
	Vector3d v(ptA,ptB);
        vectors.push_back(v);
	ptA= ptB;
      }
    Vector3d v= Vector3d(ptA,*first);
    vectors.push_back(v);
    std::deque<Vector3d>::const_iterator j= vectors.begin();
    Vector3d a= *j;
    Vector3d b= *vectors.rbegin();
    double retval= M_PI-a.getAngle(b);
    j++;
    for(;j!=vectors.end();j++)
      {
	b= *j;
        retval= std::max(retval,M_PI-a.getAngle(b));
	a= b;
      }
    return retval;
  }

#endif

