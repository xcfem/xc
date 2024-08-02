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
//m_double.cc

#include "m_double.h"

//! @brief Return the identity matrix.
m_double identidad(const m_double &m)
  {
    m_double retval(m);
    retval.Idn();
    return retval;
  }

//! @brief Return the identity matrix.
m_double identidad(const size_t &n_rows,const size_t &n_columns)
  {
    m_double retval(n_rows,n_columns);
    retval.Idn();
    return retval;
  }

//! @brief Convert a string (i. e.: [[1.0,0.0][0.0,1.0]]) into a matrix.
m_double string_to_m_double(const std::string &str)
  {
    m_double retval(1,1);
    retval.Input(str);
    return retval;
  }

boost::python::list m_double_to_py_list(const m_double &m)
  {
    boost::python::list retval;
    const size_t n_rows= m.getNumberOfRows();
    const size_t n_columns= m.getNumberOfColumns();
    for(size_t i=1; i<=n_rows; i++)
      {
	boost::python::list tmp;
	for(size_t j= 1; j<=n_columns; j++)
	  tmp.append(m(i,j));
        retval.append(tmp);
      }
    return retval;
  }
