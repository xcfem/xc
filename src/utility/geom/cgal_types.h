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
//cgal_types.h

#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#include <CGAL/Homogeneous.h>
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Point_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/enum.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Tetrahedron_3.h>
#include <iostream>
#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Arrangement_2.h>

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

//typedef CGAL::Homogeneous<CGAL::Gmpz>  GEOMKernel; //kernel.
typedef CGAL::Exact_predicates_inexact_constructions_kernel GEOMKernel; //kernel.
typedef GEOMKernel::RT GEOM_RT;
typedef GEOMKernel::FT GEOM_FT;

static const GEOM_FT geom_epsilon(1/100000);
static const GEOM_FT geom_epsilon2= geom_epsilon*geom_epsilon;
inline GEOM_FT Abs(const GEOM_FT &x) { return x >= GEOM_FT(0) ? x : -x; }


//Enums
typedef CGAL::Orientation Orientacion;

//2D
typedef CGAL::Point_2<GEOMKernel> CGPoint_2;
typedef CGAL::Vector_2<GEOMKernel> CGVector_2;
typedef CGAL::Direction_2<GEOMKernel> CGDirection_2;
typedef CGAL::Line_2<GEOMKernel> CGLine_2;
typedef CGAL::Ray_2<GEOMKernel> CGRay_2;
typedef CGAL::Segment_2<GEOMKernel> CGSegment_2;
typedef CGAL::Circle_2<GEOMKernel> CGCircle_2;
typedef CGAL::Triangle_2<GEOMKernel> CGTriangle_2;
typedef CGAL::Iso_rectangle_2<GEOMKernel> CGIsoRectangle_2;
typedef CGAL::Polygon_2<GEOMKernel> CGPolygon_2;
typedef CGAL::Polygon_with_holes_2<GEOMKernel> CGPolygonWithHoles_2;
typedef CGAL::Aff_transformation_2<GEOMKernel> CGTrfAfin_2;

//3D
typedef CGAL::Point_3<GEOMKernel> CGPoint_3;
typedef CGAL::Vector_3<GEOMKernel> CGVector_3;
typedef CGAL::Direction_3<GEOMKernel> CGDirection_3;
typedef CGAL::Line_3<GEOMKernel> CGLine_3;
typedef CGAL::Ray_3<GEOMKernel> CGRay_3;
typedef CGAL::Segment_3<GEOMKernel> CGSegment_3;
typedef CGAL::Plane_3<GEOMKernel> CGPlane_3;
typedef CGAL::Iso_cuboid_3<GEOMKernel> CGIsoCuboid_3;
typedef CGAL::Polyhedron_3<GEOMKernel> CGPolyhedron_3;
typedef CGAL::Tetrahedron_3<GEOMKernel> CGTetrahedron_3;
typedef CGAL::Bbox_3 CGBbox_3;
typedef CGAL::Aff_transformation_3<GEOMKernel> CGTrfAfin_3;


//Functions

//Misc
typedef CGAL::Geomview_stream CGview_stream;

GEOM_FT double_to_FT(const double &d);
GEOM_FT sqrt_FT(const GEOM_FT &n);
GEOM_FT abs_FT(const GEOM_FT &n);

CGPoint_2 Point_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y);
CGDirection_2 Dir_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y);
CGVector_2 Vector_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y);
CGPoint_2 Point_2_from_doubles(const double &x,const double &y);
CGVector_2 Vector_2_from_doubles(const double &x,const double &y);

CGPoint_3 Point_3_from_cartesianas(const GEOM_FT &,const GEOM_FT &,const GEOM_FT &);
CGDirection_3 Dir_3_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &);
CGVector_3 Vector_3_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &);
CGPoint_3 Point_3_from_doubles(const double &x,const double &y,const double &z);
CGVector_3 Vector_3_from_doubles(const double &x,const double &y,const double &z);

const GEOM_FT VARIABLE_IS_NOT_USED M_PI_FT= double_to_FT(M_PI);

#endif
