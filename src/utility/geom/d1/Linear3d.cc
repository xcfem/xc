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
//Linear3d.cc

#include "Linear3d.h"
#include "../pos_vec/Pos3d.h"
#include "utility/geom/lists/utils_list_pos3d.h"

//! @brief Return the lambda parameter for the given point:
//! p= getFromPoint()+lambda*VDir()
GEOM_FT Linear3d::getLambda(const Pos3d &p) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << std::endl;
    return 0.0;
  }

//! @brief Return the given points sorted by its parameter along the line
//! (assuming they are on the line) in the form of a list of (position, lambda)
//! pairs.
std::vector<std::pair<Pos3d, GEOM_FT> > Linear3d::sortPointsAlong(const GeomObj3d::list_Pos3d &points) const
  {
    const size_t sz= points.size();
    std::vector<std::pair<Pos3d, GEOM_FT> > retval(sz);
    for(GeomObj3d::list_Pos3d::const_iterator i= points.begin(); i!= points.end(); i++)
      {
	const Pos3d &pt= *i;
	const GEOM_FT &l= this->getLambda(pt);
	const size_t j= distance(points.begin(),i);
	retval[j]= std::pair<Pos3d, GEOM_FT>(pt, l);
      }
    // Sort on lambda.
    std::sort(retval.begin(), retval.end(), [](const std::pair<Pos3d, GEOM_FT> &left, const std::pair<Pos3d, GEOM_FT> &right) {return left.second < right.second;});
    return retval;
  }

//! @brief Return the given points sorted by its parameter along the line
//! (assuming they are on the line) in the form of a list of (position, lambda)
//! pairs.
boost::python::list Linear3d::sortPointsAlongPy(const boost::python::list &points) const
  {
    boost::python::list retval;
    std::vector<std::pair<Pos3d, GEOM_FT> > tmp= this->sortPointsAlong(python_to_list_pos3d(points));
    for(std::vector<std::pair<Pos3d, GEOM_FT> >::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const std::pair<Pos3d, GEOM_FT> &pair= *i;
	boost::python::tuple t= boost::python::make_tuple(pair.first, pair.second);
	retval.append(t);
      }
    return retval;
  }
