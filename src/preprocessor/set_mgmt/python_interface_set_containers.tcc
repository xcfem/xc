//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//python_interface_set_containers.tcc


typedef std::map<std::string,XC::SetBase *> map_sets;
class_<map_sets, boost::noncopyable >("map_sets", no_init)
  .def(XC::mapptr_indexing_suite<map_sets >())
  ;

boost::python::list (XC::MapSetBase::*get_sets_node_py)(const XC::Node *)= &XC::MapSetBase::get_sets_py;
boost::python::list (XC::MapSetBase::*get_sets_element_py)(const XC::Element *) = &XC::MapSetBase::get_sets_py;
class_<XC::MapSetBase , bases<map_sets>, boost::noncopyable >("MapSetBase", no_init)
  .def("removeSet", &XC::MapSetBase::remove,"Delete the set and remove it from the sets map.")
  .def("exists",&XC::MapSetBase::exists,"Return true if the sets already exists.")
  .def("getSetsNames",&XC::MapSetBase::getSetsNamesPy,"Returns the names of the sets.")
  .def("getKeys",&XC::MapSetBase::getKeysPy,"Returns the map keys in a list.")
  .def("rename",&XC::MapSetBase::rename,"Rename a set.")
  .def("getSetsContaining",get_sets_node_py,"Returns the sets that contain the node identified by the tag argument.")
  .def("getSetsContaining",get_sets_element_py, "Returns the sets that contain the element identified by the tag argument.")
  ;

XC::MapSet::const_iterator (XC::MapSet::*cBegin)(void) const= &XC::MapSet::begin;
XC::MapSet::const_iterator (XC::MapSet::*cEnd)(void) const= &XC::MapSet::end;
class_<XC::MapSet, bases<XC::PreprocessorContainer,XC::MapSetBase>, boost::noncopyable >("MapSet", no_init)
  .def("__iter__",range(cBegin, cEnd))
  .def("__getitem__",&XC::MapSet::getSet, return_internal_reference<>())
  .def("getSet", make_function(&XC::MapSet::getSet, return_internal_reference<>()),"Returns set by name.")
  .def("defSet", make_function(&XC::MapSet::defSet, return_internal_reference<>()),"Creates a new set with the name which is passed as a parameter.")
  .def("getSetsContainingNode",&XC::MapSet::get_sets_containing_node_py,"Returns the sets that contain the node identified by the tag argument.")
  .def("getSetsContainingElement",&XC::MapSet::get_sets_containing_element_py,"Returns the sets that contain the element identified by the tag argument.")
  ;
