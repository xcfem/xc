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
   ;

class_<ElemPlano4N_Mech2D, bases<ElemWithMaterial4N_Mech2D>, boost::noncopyable >("ElemPlano4N_Mech2D", no_init)
  .def("getPerimeter", &ElemPlano4N_Mech2D::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano4N_Mech2D::getArea, "Returns element's area.")
   ;

class_<QuadBase4N_Mech2D, bases<ElemPlano4N_Mech2D>, boost::noncopyable >("QuadBase4N_Mech2D", no_init)
   ;

#include "fourNodeQuad/python_interface.tcc"

XC::SolidMech2D &(ElemWithMaterial3N_Mech2D::*getMech2D3NPhysicalProp)(void)= &ElemWithMaterial3N_Mech2D::getPhysicalProperties;
class_<ElemWithMaterial3N_Mech2D, bases<XC::ElementBase<3> >, boost::noncopyable >("ElemWithMaterial3N_Mech2D", no_init)
  .add_property("getPhysicalProperties", make_function(getMech2D3NPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<ElemPlano3N_Mech2D, bases<ElemWithMaterial3N_Mech2D>, boost::noncopyable >("ElemPlano3N_Mech2D", no_init)
  .def("getPerimeter", &ElemPlano3N_Mech2D::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano3N_Mech2D::getArea, "Returns element's area.")
   ;

class_<TriBase3N_Mech2D, bases<ElemPlano3N_Mech2D>, boost::noncopyable >("QuadBase3N_Mech2D", no_init)
   ;

#include "triangle/python_interface.tcc"

XC::SectionFDPhysicalProperties &(ElemWithMaterial4N_SFD::*getSectionFDPhysicalProp)(void)= &ElemWithMaterial4N_SFD::getPhysicalProperties;
class_<ElemWithMaterial4N_SFD, bases<XC::ElementBase<4> >, boost::noncopyable >("ElemWithMaterial4N_SFD", no_init)
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<ElemPlano4N_SFD, bases<ElemWithMaterial4N_SFD>, boost::noncopyable >("ElemPlano4N_SFD", no_init)
  .def("getPerimeter", &ElemPlano4N_SFD::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano4N_SFD::getArea, "Returns element's area.")
   ;


//Shell
#include "shell/python_interface.tcc"
