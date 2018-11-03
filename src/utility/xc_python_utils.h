//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//xc_python_utils.h

#ifndef XC_PYTHON_UTILS_H
#define XC_PYTHON_UTILS_H

#include <boost/python/list.hpp>
#include <vector>
#include "xc_utils/src/matrices/m_double.h"

namespace XC {
  class ID;

boost::python::list xc_id_to_py_list(const XC::ID &);

std::vector<double> vector_double_from_py_object(const boost::python::object &);
std::vector<int> vector_int_from_py_object(const boost::python::object &);
m_double m_double_from_py_object(const boost::python::object &);

} // end of XC namespace
#endif
