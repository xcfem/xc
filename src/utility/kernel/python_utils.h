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
//python_utils.h

#ifndef PYTHON_UTILS_H
#define PYTHON_UTILS_H

#include <vector>
#include <set>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>



std::vector<int> vector_int_from_py_list(const boost::python::list &);
std::vector<double> vector_double_from_py_list(const boost::python::list &);

std::vector<std::string> vector_string_from_py_list(const boost::python::list &);

std::set<int> set_int_from_py_list(const boost::python::list &);

boost::python::list vector_int_to_py_list(const std::vector<int> &l);
boost::python::list vector_double_to_py_list(const std::vector<double> &l);

float mseImages(const std::string &fNameA, const std::string &fNameB);
float psnrImagenes(const std::string &fNameA, const std::string &fNameB);
float compare_images(const std::string &fNameA, const std::string &fNameB);


boost::python::object CommandEntity_eval(boost::python::object self, const std::string &str);

boost::python::object CommandEntity_exec(boost::python::object self, const std::string &block);

boost::python::object CommandEntity_exec_file(boost::python::object self, const std::string &fileName);

//Helper class to expose container like std::list
inline void IndexError(void)
  { PyErr_SetString(PyExc_IndexError, "Index out of range"); }

template<class T>
struct std_item
{
    typedef typename T::value_type V;
    static V& get(T const& x, int i)
      {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) return x[i];
        IndexError();
      }
    static void set(T const& x, int i, V const& v)
      {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x[i]=v;
        else IndexError();
      }
    static void del(T const& x, int i)
      {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x.erase(i);
        else IndexError();
      }
    static void add(T const& x, V const& v)
      {
        x.push_back(v);
      }
  };
#endif
