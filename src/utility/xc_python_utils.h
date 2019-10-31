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
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

namespace XC {
  class ID;

boost::python::list xc_id_to_py_list(const XC::ID &);

std::vector<double> vector_double_from_py_object(const boost::python::object &);
std::vector<int> vector_int_from_py_object(const boost::python::object &);
m_double m_double_from_py_object(const boost::python::object &);


// Solution to export std::map<key, T *> to Python as iterable
// objects.
 
// CREDITS: Taken from a post of Phil in stackoverflow
// https://stackoverflow.com/users/2904770/phil
 
//Forward declaration
template <class Container, bool NoProxy, class DerivedPolicies>
class mapptr_indexing_suite;

template <class Container, bool NoProxy>
class final_mapptr_derived_policies
    : public mapptr_indexing_suite<Container,
        NoProxy, final_mapptr_derived_policies<Container, NoProxy> > {};

template <
    class Container,
    bool NoProxy = false,
    class DerivedPolicies
        = final_mapptr_derived_policies<Container, NoProxy> >
class mapptr_indexing_suite
  : public boost::python::map_indexing_suite<
    Container,
    NoProxy,
    DerivedPolicies
    >
  {
  public:
    // Must be explicit if the compiler is
    // going to take from the base class
    using typename boost::python::map_indexing_suite<
        Container,NoProxy,DerivedPolicies>::data_type;
    using typename boost::python::map_indexing_suite<
        Container,NoProxy,DerivedPolicies>::value_type;

    // Only one class needs to be overridden from the base
    template <class Class>
    static void
    extension_def(Class& cl)
    {
        //  Wrap the map's element (value_type)
        std::string elem_name = "mapptr_indexing_suite_";
        boost::python::object class_name(cl.attr("__name__"));
	boost::python::extract<std::string> class_name_extractor(class_name);
        elem_name += class_name_extractor();
        elem_name += "_entry";

        // use of is_pointer here is the only
        // difference to the base map_indexing_suite
        typedef typename boost::mpl::if_<
            boost::mpl::and_<std::is_pointer<data_type>, boost::mpl::bool_<!NoProxy> >
	  , boost::python::return_internal_reference<>
            , boost::python::default_call_policies
            >::type get_data_return_policy;

        boost::python::class_<value_type>(elem_name.c_str())
            .def("__repr__", &DerivedPolicies::print_elem)
            .def("data", &DerivedPolicies::get_data, get_data_return_policy())
            .def("key", &DerivedPolicies::get_key)
            ;
      }
  };
} // end of XC namespace
#endif
