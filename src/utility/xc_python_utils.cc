//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
//xc_python_utils.cpp

#include "xc_python_utils.h"
#include <boost/python/extract.hpp>
#include "utility/matrix/ID.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/kernel/python_utils.h"


boost::python::list XC::xc_id_to_py_list(const XC::ID &id)
  {
    boost::python::list retval;
    const size_t sz= id.Size();
    for(size_t i= 0;i<sz;++i)
      retval.append(id(i));
    return retval;
  }

std::vector<double> XC::vector_double_from_py_object(const boost::python::object &o)
  {
    std::vector<double> retval;
    boost::python::extract<double> x(o);
    if (x.check())
      { retval= std::vector<double>(1,x()); }
    else
      {
        boost::python::extract<std::vector<double> > v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::Vector> vt(o);
            if(vt.check())
              retval= vector_to_std_vector(vt());
            else
              {
                boost::python::extract<boost::python::list> l(o);
                if(l.check())
                  retval= vector_double_from_py_list(l());
              }
          }
      }
    return retval;
  }

std::vector<int> XC::vector_int_from_py_object(const boost::python::object &o)
  {
    std::vector<int> retval;
    boost::python::extract<int> x(o);
    if (x.check())
      { retval= std::vector<int>(1,x()); }
    else
      {
        boost::python::extract<std::vector<int> > v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::ID > vt(o);
            if(vt.check())
              retval= vt();
            else
              {
                boost::python::extract<boost::python::list> l(o);
                if(l.check())
                  retval= vector_int_from_py_list(l());
              }
          }
      }
    return retval;
  }

m_double XC::m_double_from_py_object(const boost::python::object &o)
  {
    m_double retval;
    boost::python::extract<double> x(o);
    if (x.check())
      { retval= m_double(1,1,x()); }
    else
      {
        boost::python::extract<m_double> v(o);
        if(v.check())
          retval= v();
        else
	  {
            boost::python::extract<XC::Matrix > mt(o);
            if(mt.check())
              retval= matrix_to_m_double(mt());
          }
      }
    return retval;
  }
