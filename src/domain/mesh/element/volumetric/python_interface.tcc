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
typedef XC::ElementBase<8> element_base_8n;
class_<element_base_8n, bases<XC::Element>, boost::noncopyable >("element_base_8n", no_init);

typedef XC::ElemWithMaterial<8,XC::NDMaterialPhysicalProperties> brick_base_8n;
class_<brick_base_8n, bases<element_base_8n>, boost::noncopyable >("brick_base_8n", no_init)
  .add_property("extrapolationMatrix",make_function(&brick_base_8n::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
  .def("getExtrapolatedValues", &brick_base_8n::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
  .def("setMaterial", &brick_base_8n::setMaterial,"Set the element material.")
  ;

XC::NDMaterialPhysicalProperties &(XC::BrickBase::*getBrickPhysicalPropertiesPtr)(void) = &brick_base_8n::getPhysicalProperties;
class_<XC::BrickBase , bases<brick_base_8n>, boost::noncopyable >("BrickBase", no_init)
  .add_property("physicalProperties",make_function(getBrickPhysicalPropertiesPtr,return_internal_reference<>() ),"Returns element physical properties (material).")
  ;

typedef XC::ElementBase<20> element_base_20n;
class_<element_base_20n, bases<XC::Element>, boost::noncopyable >("element_base_20n", no_init);

typedef XC::ElemWithMaterial<20,XC::NDMaterialPhysicalProperties> twenty_node_brick_elem;
class_<twenty_node_brick_elem, bases<element_base_20n>, boost::noncopyable >("twenty_node_brick_elem", no_init)
  .def("setMaterial", &twenty_node_brick_elem::setMaterial,"Set the element material.")
  ;

typedef XC::ElementBase<27> element_base_27n;
class_<element_base_27n, bases<XC::Element>, boost::noncopyable >("element_base_27n", no_init);

#include "brick/python_interface.tcc"
#include "TotalLagrangianFD20NodeBrick/python_interface.tcc"
#include "upU/python_interface.tcc"
#include "UP-ucsd/python_interface.tcc"
#include "27nbrick/python_interface.tcc"
#include "8nbrick/python_interface.tcc"
#include "20nbrick/python_interface.tcc"
