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
//python_interface.tcc

XC::MaterialHandler *(XC::SectionRepres::*getMaterialHandlerPtr)(void)= &XC::SectionRepres::getMaterialHandler;
class_<XC::SectionRepres, bases<XC::TaggedObject>, boost::noncopyable >("SectionRepres", no_init)
  .add_property("geometry",make_function(&XC::SectionRepres::getGeom,return_internal_reference<>()),&XC::SectionRepres::setGeom,"Get/set the section geometry.")
  .def("setGeomNamed",&XC::SectionRepres::setGeomNamed)
  .add_property("getMaterialHandler", make_function(getMaterialHandlerPtr, return_internal_reference<>() ), "Return a reference to the material handler.")
  ;

class_<XC::FiberSectionRepr, bases<XC::SectionRepres>, boost::noncopyable >("FiberSectionRepr", no_init)
  .def("getNumFibers",&XC::FiberSectionRepr::getNumFibers, "Return the number of fibers.")
  .def("clear", &XC::FiberSectionRepr::clear,"Clear the fiber container.")
  ;
