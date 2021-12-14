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
//LineSegmentArrangement.h

#ifndef LINESEGMENTARRANGEMENT_H
#define LINESEGMENTARRANGEMENT_H

#include "utility/geom/d2/Surface2d.h"
#include <list>
#include "../../cgal_types.h"
#include "utility/geom/d1/Segment2d.h"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

//Arrangement of intersecting line segments.

typedef CGAL::Exact_predicates_exact_constructions_kernel ArrKernel;
typedef CGAL::Arr_segment_traits_2<ArrKernel> ArrTraits_2;
typedef CGAL::Arrangement_2<ArrTraits_2> Arrangement_2;

//! @ingroup GEOM
//
//! @brief Base class for the polygonal surfaces.
class LineSegmentArrangement: public Surface2d
  {
    Arrangement_2 cgarr; // CGAL arrangement.
  public:
    LineSegmentArrangement(void);
    LineSegmentArrangement(const std::list<Segment2d> &);
    virtual GeomObj *getCopy(void) const;

    size_t getNumberOfVertices(void) const;
    size_t getNumberOfEdges(void) const;
    size_t getNumberOfFaces(void) const;
    
    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    Pos2d getCenterOfMass(void) const;
    bool In(const Pos2d &p, const double &tol) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Pxy(void) const;
    void Transform(const Trf2d &);

    void Print(std::ostream &os) const;
  };

#endif







