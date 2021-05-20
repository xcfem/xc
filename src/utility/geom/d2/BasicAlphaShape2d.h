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
//BasicAlphaShape2d.h

#ifndef BASICALPHASHAPE2D_H
#define BASICALPHASHAPE2D_H

#include "utility/geom/d2/GeomObj2d.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/algorithm.h>
#include <CGAL/assertions.h>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef K::FT                                                FT;
typedef K::Point_2                                           Point_2;
typedef K::Segment_2                                         Segment;
typedef CGAL::Alpha_shape_vertex_base_2<K>                   Vb;
typedef CGAL::Alpha_shape_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>          Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                Triangulation_2;
typedef CGAL::Alpha_shape_2<Triangulation_2>                 Alpha_shape_2;

class Polygon2d;

//! @ingroup GEOM
//
//! @brief Return the basic alpha shap of the point set argument.
//!
//! <a href="https://en.wikipedia.org/wiki/Alpha_shape">Alpha shape</a>
Polygon2d get_basic_alpha_shape2d(const GeomObj::list_Pos2d &);

//! @ingroup GEOM
//
//! @brief Basic alpha shape of the point set
//! argument.
//!
//! <a href="https://en.wikipedia.org/wiki/Alpha_shape">Alpha shape</a>
class BasicAlphaShape2d
  {
  protected:
    Alpha_shape_2 *A;
    void alloc(const GeomObj::list_Pos2d &lp, const double &alpha);
    void free(void);
  public:
    BasicAlphaShape2d(const GeomObj::list_Pos2d &lp, const double &alpha);
    ~BasicAlphaShape2d(void);
    
    void setAlpha(const double alpha);
    double getOptimalAlpha(const size_t &nb_components= 1) const;
    Polygon2d getPolygon2d(void);
  };



#endif
