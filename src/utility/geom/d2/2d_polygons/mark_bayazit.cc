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
//Mark_Bayazit.cc

#include "mark_bayazit.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

const double quiet_nan= std::numeric_limits<double>::quiet_NaN();

GEOM_FT area(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  { return (((b.x() - a.x())*(c.y() - a.y()))-((c.x() - a.x())*(b.y() - a.y()))); }

bool left(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  { return area(a, b, c) > 0; }

bool leftOn(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  { return area(a, b, c) >= 0; }

bool right(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  { return area(a, b, c) < 0; }

bool rightOn(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  { return area(a, b, c) <= 0; }

bool collinear(const Pos2d &a, const Pos2d &b, const Pos2d &c)
  {
    return area(a, b, c) == 0;
  }

GEOM_FT sqdist(const Pos2d &a, const Pos2d &b)
  {
    GEOM_FT dx = b.x() - a.x();
    GEOM_FT dy = b.y() - a.y();
    return dx * dx + dy * dy;
  }

bool isReflex(const Polygon2d &poly, const int &i)
  {
    return right(at(poly, i - 1), at(poly, i), at(poly, i + 1));
  }

bool eq(const GEOM_FT &a, const GEOM_FT &b)
  {
    return abs(a - b) <= 1e-8;
  }

Pos2d intersection(const Pos2d &p1, const Pos2d &p2, const Pos2d &q1, const Pos2d &q2)
  {
    Pos2d i(quiet_nan,quiet_nan);
    GEOM_FT a1, b1, c1, a2, b2, c2, det;
    a1 = p2.y() - p1.y();
    b1 = p1.x() - p2.x();
    c1 = a1 * p1.x() + b1 * p1.y();
    a2 = q2.y() - q1.y();
    b2 = q1.x() - q2.x();
    c2 = a2 * q1.x() + b2 * q1.y();
    det = a1 * b2 - a2*b1;
    if(!eq(det, 0))
      { // lines are not parallel
        i.SetX((b2 * c1 - b1 * c2) / det);
        i.SetY((a1 * c2 - a2 * c1) / det);
      }
    return i;
  }

void decompose_poly(const Polygon2d &plg, std::list<Polygon2d> &result)
  {
    Polygon2d poly= plg;
    poly.makeCounterClockWise();
    Pos2d upperInt, lowerInt, p, closestVert;
    GEOM_FT upperDist, lowerDist, d, closestDist;
    unsigned int upperIndex= 0, lowerIndex= 0, closestIndex= 0;
    Polygon2d lowerPoly, upperPoly;
    std::vector<Pos2d> steinerPoints, reflexVertices;
    const unsigned int polygon_size= poly.getNumVertices();

    for(unsigned int i= 0; i < polygon_size; ++i)
      {
        if(isReflex(poly, i))
	  {
            reflexVertices.push_back(poly[i]);
            upperDist = lowerDist = std::numeric_limits<double>::max();
            for(unsigned int j= 0; j < polygon_size; ++j)
	      {
                if(left(at(poly, i - 1), at(poly, i), at(poly, j)) && rightOn(at(poly, i - 1), at(poly, i), at(poly, j - 1)))
		  { // if line intersects with an edge
                    p = intersection(at(poly, i - 1), at(poly, i), at(poly, j), at(poly, j - 1)); // find the point of intersection
                    if(right(at(poly, i + 1), at(poly, i), p))
		      { // make sure it's inside the poly
                        d = sqdist(poly[i], p);
                        if(d < lowerDist)
			  { // keep only the closest intersection
                            lowerDist = d;
                            lowerInt = p;
                            lowerIndex = j;
			  }
		      }
		  }
                if(left(at(poly, i + 1), at(poly, i), at(poly, j + 1))
                        && rightOn(at(poly, i + 1), at(poly, i), at(poly, j))) {
                    p = intersection(at(poly, i + 1), at(poly, i), at(poly, j), at(poly, j + 1));
                    if(left(at(poly, i - 1), at(poly, i), p)) {
                        d = sqdist(poly[i], p);
                        if(d < upperDist) {
                            upperDist = d;
                            upperInt = p;
                            upperIndex = j;
                        }
                    }
                }
            }

            // if there are no vertices to connect to, choose a point in the middle
            if(lowerIndex == (upperIndex + 1) % polygon_size)
	      {
                //printf("Case 1: Vertex(%d), lowerIndex(%d), upperIndex(%d), poly.size(%d)\n", i, lowerIndex, upperIndex, (int) polygon_size);
                p.SetX((lowerInt.x() + upperInt.x()) / 2.0);
                p.SetY((lowerInt.y() + upperInt.y()) / 2.0);
                steinerPoints.push_back(p);

                if(i < upperIndex) {
                    lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin() + i, poly.vertices_begin() + upperIndex + 1);
                    lowerPoly.push_back(p);
                    upperPoly.push_back(p);
                    if(lowerIndex != 0) upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin() + lowerIndex, poly.vertices_end());
                    upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin(), poly.vertices_begin() + i + 1);
                } else {
                    if(i != 0) lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin() + i, poly.vertices_end());
                    lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin(), poly.vertices_begin() + upperIndex + 1);
                    lowerPoly.push_back(p);
                    upperPoly.push_back(p);
                    upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin() + lowerIndex, poly.vertices_begin() + i + 1);
                }
            } else {
                // connect to the closest point within the triangle
                //printf("Case 2: Vertex(%d), closestIndex(%d), poly.size(%d)\n", i, closestIndex, (int) polygon_size);

                if(lowerIndex > upperIndex) {
                    upperIndex += polygon_size;
                }
                closestDist = std::numeric_limits<double>::max();
                for(unsigned int j = lowerIndex; j <= upperIndex; ++j)
		  {
                    if(leftOn(at(poly, i - 1), at(poly, i), at(poly, j))
                            && rightOn(at(poly, i + 1), at(poly, i), at(poly, j))) {
                        d = sqdist(at(poly, i), at(poly, j));
                        if(d < closestDist) {
                            closestDist = d;
                            closestVert = at(poly, j);
                            closestIndex = j % polygon_size;
                        }
                    }
                }

                if(i < closestIndex)
		  {
                    lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin() + i, poly.vertices_begin() + closestIndex + 1);
                    if(closestIndex != 0) upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin() + closestIndex, poly.vertices_end());
                    upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin(), poly.vertices_begin() + i + 1);
                  }
		else
		  {
                    if(i != 0) lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin() + i, poly.vertices_end());
                    lowerPoly.insert(lowerPoly.vertices_end(), poly.vertices_begin(), poly.vertices_begin() + closestIndex + 1);
                    upperPoly.insert(upperPoly.vertices_end(), poly.vertices_begin() + closestIndex, poly.vertices_begin() + i + 1);
                  }
            }

            // solve smallest poly first
            if(lowerPoly.getNumVertices() < upperPoly.getNumVertices())
	      {
                decompose_poly(lowerPoly, result);
                decompose_poly(upperPoly, result);
              }
	    else
	      {
                decompose_poly(upperPoly, result);
                decompose_poly(lowerPoly, result);
              }
            return;
        }
      }
    result.push_back(poly);
  }
