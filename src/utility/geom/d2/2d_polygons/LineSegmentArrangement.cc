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

#include "LineSegmentArrangement.h"

typedef ArrKernel::FT Number_type;
typedef ArrTraits_2::Point_2 Pt_2;
typedef ArrTraits_2::X_monotone_curve_2 MC_2;

LineSegmentArrangement::LineSegmentArrangement(void)
  : Surface2d(), cgarr()
  {}

LineSegmentArrangement::LineSegmentArrangement(const std::list<Segment2d> &segments)
  : Surface2d(), cgarr()
  {
    // Construct the arrangement by aggregately inserting all segments.
    const size_t sz= segments.size();
    std::list<MC_2> mcs;

    std::cout << "Performing aggregated insertion of " 
     	      << sz << " segments." << std::endl;

    for(std::list<Segment2d>::const_iterator i= segments.begin(); i!= segments.end();i++)
      {
    	const Pos2d p1= i->getFromPoint();
    	const Pos2d p2= i->getToPoint();
    	const GEOM_FT x1= p1.x();
    	const GEOM_FT y1= p1.y();
    	const GEOM_FT x2= p2.x();
    	const GEOM_FT y2= p2.y();
        mcs.push_back((MC_2 (Pt_2 (Number_type(x1), Number_type(y1)),
     			     Pt_2 (Number_type(x2), Number_type(y2)))));
      }
	
			   
    insert(cgarr, mcs.begin(), mcs.end());

  }

//! @brief Virtual constructor.
GeomObj *LineSegmentArrangement::clon(void) const
  { return new LineSegmentArrangement(*this); }

//! @brief Return the number of vertices.
size_t LineSegmentArrangement::getNumberOfVertices(void) const
  { return cgarr.number_of_vertices(); }

//! @brief Return the number of edges.
size_t LineSegmentArrangement::getNumberOfEdges(void) const
  { return cgarr.number_of_edges(); }

//! @brief Return the number of faces.
size_t LineSegmentArrangement::getNumberOfFaces(void) const
  { return cgarr.number_of_faces(); }

//! @brief Return the center of mass.
Pos2d LineSegmentArrangement::getCenterOfMass(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    Pos2d retval;
    return retval;
  }

//! @brief Return the maximum value of the i-th coordinate.
GEOM_FT LineSegmentArrangement::GetMax(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }

//! @brief Return the minimum value of the i-th coordinate.
GEOM_FT LineSegmentArrangement::GetMin(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }

//! @brief Return true if the point is inside the object.
bool LineSegmentArrangement::In(const Pos2d &p, const double &tol) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }

//! @brief Compute moment of inertia with respect to an axis parallel to the
//! x axis that passes through thecenter of mass of the surface.
//! Ix = Integral y^2 dA
GEOM_FT LineSegmentArrangement::Ix(void) const
  { 
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }

//! @brief Calcula el moment of inertia with respect to an axis parallel to the
//! y axis que pasa por el polygon centroid.
//! Iy = Integral x^2 dA
GEOM_FT LineSegmentArrangement::Iy(void) const
  {
     std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }
   
//! @brief Calcula el product of inertia with respect to the axis parallel
//! to the x and y that passes through the centroid of the polygon.
//! Pxy = Integral x*y dA
GEOM_FT LineSegmentArrangement::Pxy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    return NAN;
  }

//! @brief Applies the transformation being passed as parameter.
void LineSegmentArrangement::Transform(const Trf2d &trf2d)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
  }

void LineSegmentArrangement::Print(std::ostream &os) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
  }
