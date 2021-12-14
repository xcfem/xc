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
//polyhedron3d_bool_op.h
//Boolean operations between polyhedrons.

#ifndef POLYHEDRON3D_BOOL_OP_H
#define POLYHEDRON3D_BOOL_OP_H

class Polyhedron3d;


//! @ingroup GEOM
//
//! @brief Union of the polyhedron arguments.
Polyhedron3d Union(const Polyhedron3d &a,const Polyhedron3d &b);
//! @brief Intersection of the polyhedrons.
Polyhedron3d intersection(const Polyhedron3d &a,const Polyhedron3d &b);
//! @brief Difference of the polyhedrons.
Polyhedron3d Diferencia(const Polyhedron3d &a,const Polyhedron3d &b);

#endif
