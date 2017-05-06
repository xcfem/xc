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
//python_interface.tcc

XC::Mesh &(XC::Domain::*getMeshRef)(void)= &XC::Domain::getMesh;
XC::Preprocessor *(XC::Domain::*getPreprocessor)(void)= &XC::Domain::getPreprocessor;
XC::ConstrContainer &(XC::Domain::*getConstraintsRef)(void)= &XC::Domain::getConstraints;
class_<XC::Domain, bases<XC::ObjWithRecorders>, boost::noncopyable >("Domain", no_init)
  .add_property("getPreprocessor", make_function( getPreprocessor, return_internal_reference<>() ),"returns preprocessor.")
  .add_property("getMesh", make_function( getMeshRef, return_internal_reference<>() ),"returns finite element mesh.")
  .add_property("getConstraints", make_function( getConstraintsRef, return_internal_reference<>() ),"returns mesh constraints.")
  .def("setDeadSRF",XC::Domain::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .def("commit",&XC::Domain::commit)
  .def("revertToLastCommit",&XC::Domain::revertToLastCommit)
  .def("revertToStart",&XC::Domain::revertToStart)  
  .def("setTime",&XC::Domain::setTime,"Assigns time tracker time.")  
  ;
