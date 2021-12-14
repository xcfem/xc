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
//Grid2d.cpp

#include "Grid2d.h" 



#include "utility/geom/d2/2d_polygons/Quadrilateral2d.h"
#include "utility/geom/FT_matrix.h"
#include "utility/geom/trf/Trf2d.h"


//! @brief Constructor.
Grid2d::Grid2d(void)
  : Surface2d(), points(1,1) {}

//! @brief Constructor.
Grid2d::Grid2d(int numSubdivIJ, int numSubdivJK)
  : Surface2d(), points(numSubdivIJ,numSubdivJK) {}

Grid2d::~Grid2d(void)
  {}

//! @brief Return the area of the grid.
GEOM_FT Grid2d::getArea(void) const
  { return points.GetArea();  }

//! @briefMoment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Grid2d::Ix(void) const
  { return points.Ix(); }

//! @brief Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Grid2d::Iy(void) const
  { return points.Iy(); }

//! @brief product of inertia with respect to the center of mass in local coordinates.
GEOM_FT Grid2d::Pxy(void) const
  { return points.Pxy(); }

//! @brief Return the center of mass.
Pos2d Grid2d::getCenterOfMass(void) const
  { return points.getCenterOfMass(); }

//! @brief Return the coordinates of the i,j point.
const FT_matrix &Grid2d::GetVertCoords(const size_t &i,const size_t &j) const
  { return points.GetVertCoords(i,j); }

//! @brief Return the point i,j.
Pos2d Grid2d::getPoint(const size_t &i,const size_t &j) const
  { return points.getPoint(i,j); }

//! @brief Return the i,j cell quad:
Quadrilateral2d Grid2d::GetQuad(const size_t &i,const size_t &j) const
  { return points.GetQuad(i,j); }

//! @brief Return true if the point belongs to the grid.
bool Grid2d::In(const Pos2d &p, const double &tol) const
  { return points.In(p,tol); }

//! @brief Return the maximum value of the i coordinate.
GEOM_FT Grid2d::GetMax(unsigned short int i) const
  { return points.GetMax(i); }

//! @brief Return the minimum value of the i coordinate.
GEOM_FT Grid2d::GetMin(unsigned short int i) const
  { return points.GetMin(i); }


//! @brief Aplica al objeto la transformación que se
//! pasa como parámetro.
void Grid2d::Transform(const Trf2d &trf2d)
  { points.Transform(trf2d); }

void Grid2d::Print(std::ostream &os) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }
