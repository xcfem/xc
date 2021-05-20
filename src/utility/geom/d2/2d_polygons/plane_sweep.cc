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
//LineSegmentArrangement.cc
// Constructing an arrangements of intersecting line segments using the
// predefined kernel with exact constructions and exact predicates.

#include "plane_sweep.h"
//#include <CGAL/Surface_sweep_2_algorithms.h> CGAL 5.0
#include <CGAL/Sweep_line_2_algorithms.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel ArrKernel;
typedef CGAL::Arr_segment_traits_2<ArrKernel> ArrTraits_2;
typedef ArrKernel::FT Number_type;
typedef ArrTraits_2::Point_2 Pt_2;
typedef ArrTraits_2::Curve_2 Sg_2;

std::list<Segment2d> plane_sweep(const std::list<Segment2d> &input_segments)
  {
    std::list<Sg_2> segments;
    // Populate the input segments.
    for(std::list<Segment2d>::const_iterator i= input_segments.begin();
	i!= input_segments.end();i++)
      {
    	const Pos2d p1= i->getFromPoint();
    	const Pos2d p2= i->getToPoint();
    	const GEOM_FT x1= p1.x();
    	const GEOM_FT y1= p1.y();
    	const GEOM_FT x2= p2.x();
    	const GEOM_FT y2= p2.y();
        segments.push_back(Sg_2(Pt_2 (Number_type(x1), Number_type(y1)), Pt_2(Number_type(x2), Number_type(y2))));
      }
    // Compute all intersection points.
    std::list<Pt_2> pts;
    CGAL::compute_intersection_points(segments.begin(), segments.end(),std::back_inserter(pts));

    // Compute the non-intersecting sub-segments induced by the input segments.
    std::list<Sg_2> sub_segs;
    CGAL::compute_subcurves(segments.begin(), segments.end(), std::back_inserter(sub_segs));
    CGAL_assertion(CGAL::do_curves_intersect (segments.begin(), segments.end()));
    Number_type::set_relative_precision_of_to_double(1e-6); //specifies the relative precision that to_double() has to fulfill. 
    
    std::list<Segment2d> retval;
    // Populate the output segments.
    for(std::list<Sg_2>::const_iterator i= segments.begin();
    	i!= segments.end();i++)
      {
    	const Pt_2 p1= i->source();
    	const Pt_2 p2= i->target();
    	const GEOM_FT x1= to_double(p1.x());
    	const GEOM_FT y1= to_double(p1.y());
    	const GEOM_FT x2= to_double(p2.x());
    	const GEOM_FT y2= to_double(p2.y());
    	retval.push_back(Segment2d(Pos2d(x1,y1), Pos2d(x2,y2)));
      }
    return retval;
  }
