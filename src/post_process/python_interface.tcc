//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.cc
class_<XC::FieldInfo, bases<EntConNmb>, boost::noncopyable >("FieldInfo", no_init)
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

class_<XC::MapFields, bases<EntCmd,dq_fields> >("MapFields")
  .def("newField",make_function( &XC::MapFields::newField, return_internal_reference<>() ),"Defines a new field.")
  ;

