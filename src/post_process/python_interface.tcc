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
//python_interface.cc
class_<XC::FieldInfo, bases<NamedEntity>, boost::noncopyable >("FieldInfo", no_init)
  .add_property("isDefinedOnNodes",&XC::FieldInfo::isDefinedOnNodes,"True if field defined on nodes.")
  .add_property("isDefinedOnElements",&XC::FieldInfo::isDefinedOnElements,"True if field defined on elements.")
  .add_property("isDefinedOnGaussPoints",&XC::FieldInfo::isDefinedOnElements,"True if field defined on element's gauss points.")
  .def("definedOnNodes",&XC::FieldInfo::definedOnNodes,"Field defined on nodes.")
  .def("definedOnElements",&XC::FieldInfo::definedOnElements,"Field defined on elements.")
  .def("definedOnGaussPoints",&XC::FieldInfo::definedOnGaussPoints,"Field defined on element's gauss points.")
  .add_property("setName", make_function( &XC::FieldInfo::getSetName, return_value_policy<return_by_value>() ), &XC::FieldInfo::setSetName, "Name for which the field is defined.")
  .add_property("numberOfComponents", &XC::FieldInfo::getNumberOfComponents, "Returns field's number of components.")
  .add_property("componentsType", make_function( &XC::FieldInfo::getComponentsType, return_value_policy<return_by_value>() ), &XC::FieldInfo::setComponentsType, "Set field component's type.")
  .add_property("componentsProperty", make_function( &XC::FieldInfo::getComponentsProperty, return_value_policy<return_by_value>() ), &XC::FieldInfo::setComponentsProperty, "Set name to retrieve property.")
  .add_property("componentNames", make_function( &XC::FieldInfo::getComponentNames, return_value_policy<return_by_value>() ), &XC::FieldInfo::setComponentNames, "Set field component's names.")
  .add_property("componentDescriptions", make_function( &XC::FieldInfo::getComponentDescriptions, return_value_policy<return_by_value>() ), &XC::FieldInfo::setComponentDescriptions, "Set component's descriptions.")
  .add_property("componentUnits", make_function( &XC::FieldInfo::getComponentUnits, return_value_policy<return_by_value>() ), &XC::FieldInfo::setComponentUnits, "Set field component's units.")
  .add_property("iterationNumber", make_function( &XC::FieldInfo::getIterationNumber, return_value_policy<return_by_value>() ), &XC::FieldInfo::setIterationNumber, "iteration number for the field data.")
  .add_property("time", make_function( &XC::FieldInfo::getTime, return_value_policy<return_by_value>() ), &XC::FieldInfo::setTime, "Time for the field data.")
  ;

typedef std::deque<XC::FieldInfo> dq_fields;
class_<dq_fields, boost::noncopyable >("dqFields", no_init)
  .def(vector_indexing_suite<dq_fields >())  
  ;

class_<XC::MapFields, bases<CommandEntity,dq_fields> >("MapFields")
  .def("newField",make_function( &XC::MapFields::newField, return_internal_reference<>() ),"Defines a new field.")
  ;

