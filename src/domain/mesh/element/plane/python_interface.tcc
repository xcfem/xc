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

//Elasticity.

XC::SolidMech2D &(ElemWithMaterial4N_Mech2D::*getMech2DPhysicalProp)(void)= &ElemWithMaterial4N_Mech2D::getPhysicalProperties;
class_<ElemWithMaterial4N_Mech2D, bases<XC::ElementBase<4> >, boost::noncopyable >("ElemWithMaterial4N_Mech2D", no_init)
  .add_property("getPhysicalProperties", make_function(getMech2DPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
  .add_property("extrapolationMatrix",make_function(&ElemWithMaterial4N_Mech2D::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
  .def("getExtrapolatedValues", &ElemWithMaterial4N_Mech2D::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
   ;

class_<PlaneElement4N_Mech2D, bases<ElemWithMaterial4N_Mech2D>, boost::noncopyable >("PlaneElement4N_Mech2D", no_init)
  .def("getPerimeter", &PlaneElement4N_Mech2D::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &PlaneElement4N_Mech2D::getArea, "getArea(initialGeometry): return element's area. If initialGeometry is True the returned area corresponds to its undeformed geometry.")
  .def("getMaximumCornerAngle", &PlaneElement4N_Mech2D::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
   ;

class_<QuadBase4N_Mech2D, bases<PlaneElement4N_Mech2D>, boost::noncopyable >("QuadBase4N_Mech2D", no_init)
   ;

#include "fourNodeQuad/python_interface.tcc"

XC::SolidMech2D &(ElemWithMaterial3N_Mech2D::*getMech2D3NPhysicalProp)(void)= &ElemWithMaterial3N_Mech2D::getPhysicalProperties;
class_<ElemWithMaterial3N_Mech2D, bases<XC::ElementBase<3> >, boost::noncopyable >("ElemWithMaterial3N_Mech2D", no_init)
  .add_property("getPhysicalProperties", make_function(getMech2D3NPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<PlaneElement3N_Mech2D, bases<ElemWithMaterial3N_Mech2D>, boost::noncopyable >("PlaneElement3N_Mech2D", no_init)
  .def("getPerimeter", &PlaneElement3N_Mech2D::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &PlaneElement3N_Mech2D::getArea, "getArea(initialGeometry): return element's area. If initialGeometry is True the returned area corresponds to its undeformed geometry.")
  .def("getMaximumCornerAngle", &PlaneElement3N_Mech2D::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
   ;

class_<TriBase3N_Mech2D, bases<PlaneElement3N_Mech2D>, boost::noncopyable >("TriBase3N_Mech2D", no_init)
   ;

#include "triangle/python_interface.tcc"

XC::SectionFDPhysicalProperties &(ElemWithMaterial4N_SFD::*getSectionFDPhysicalProp)(void)= &ElemWithMaterial4N_SFD::getPhysicalProperties;
class_<ElemWithMaterial4N_SFD, bases<XC::ElementBase<4> >, boost::noncopyable >("ElemWithMaterial4N_SFD", no_init)
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<PlaneElement4N_SFD, bases<ElemWithMaterial4N_SFD>, boost::noncopyable >("PlaneElement4N_SFD", no_init)
  .def("getPerimeter", &PlaneElement4N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &PlaneElement4N_SFD::getArea, "getArea(initialGeometry): return element's area. If initialGeometry is True the returned area corresponds to its undeformed geometry.")
  .def("getMaximumCornerAngle", &PlaneElement4N_SFD::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
   ;

class_<ElemWithMaterial9N_SFD, bases<XC::ElementBase<9> >, boost::noncopyable >("ElemWithMaterial9N_SFD", no_init)
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<PlaneElement9N_SFD, bases<ElemWithMaterial9N_SFD>, boost::noncopyable >("PlaneElement9N_SFD", no_init)
  .def("getPerimeter", &PlaneElement9N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &PlaneElement9N_SFD::getArea, "getArea(initialGeometry): return element's area. If initialGeometry is True the returned area corresponds to its undeformed geometry.")
  .def("getMaximumCornerAngle", &PlaneElement9N_SFD::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
   ;


//Shell
#include "shell/python_interface.tcc"
