//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//ConvexHull2d.cc

#include "ConvexHull2d.h"
#include <vector>
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
//#include "utility/geom/d2/PolygonMap.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;


const double escala= 1e3;

inline double double2intdouble(const double &d)
  { return floor(d*escala); }

inline Point_2 Pos2d_to_Point_2(const Pos2d &p2d)
  {
    return Point_2(double2intdouble(p2d.x()),double2intdouble(p2d.y()));
  }

inline CGPoint_2 Point_to_Pos2d(const Point_2 &np)
  { return CGPoint_2(np.x()/escala,np.y()/escala); }

// class Point2Pos2d
//   {
//   public:
//     CGPoint_2 operator()(const Point_2 &np) const
//      { return Point_to_Pos2d(np); }
//   };

template <class InputIterator>
Polygon2d get_convex_hull2d(InputIterator first,InputIterator last)
  {
    std::vector<Point_2> result;
     CGAL::convex_hull_2(first, last, std::back_inserter(result));
    Polygon2d retval;
    for(std::vector<Point_2>::const_iterator i= result.begin();i!=result.end();i++)
      retval.push_back(Pos2d(Point_to_Pos2d(*i)));
    return retval;
  }

Polygon2d get_convex_hull2d(const GeomObj::list_Pos2d &lp)
  {
    std::vector<Point_2> input;
    for(GeomObj::list_Pos2d::const_iterator i= lp.begin();i!=lp.end();i++)
      input.push_back(Pos2d_to_Point_2(*i));
    return get_convex_hull2d(input.begin(),input.end());
 }

Polygon2d get_convex_hull2d(const boost::python::list &lp)
  {
    std::vector<Point_2> input;
    const size_t sz= len(lp);
    for(size_t i=0; i<sz; i++)
      {
	Pos2d p= boost::python::extract<Pos2d>(lp[i]);
        input.push_back(Pos2d_to_Point_2(p));
      }
    return get_convex_hull2d(input.begin(),input.end());
 }
