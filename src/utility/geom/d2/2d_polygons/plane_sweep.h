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
//plane_sweep.h

#ifndef PLANE_SWEEP_H
#define PLANE_SWEEP_H

#include <list>
#include "../../cgal_types.h"
#include "utility/geom/d1/Segment2d.h"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>


//! @ingroup GEOM
//
//! @brief Compute the intersection points among segments using the
//! surface-sweep algorithm and returning the non-intersecting
//! sub-segments induced by the input segments.
std::list<Segment2d> plane_sweep(const std::list<Segment2d> &);

#endif







