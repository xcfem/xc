//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
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
//python_interface.tcc

class_<ActionRelationships, bases<CommandEntity> >("ActionRelationships")
  .add_property("no_determinante", &ActionRelationships::NoDeterminante, &ActionRelationships::setNoDeterminante)
  .def("appendIncompatible", &ActionRelationships::appendIncompatible)
  .def("incompatibleNames", &ActionRelationships::incompatibleNames)
  .def("appendMain", &ActionRelationships::appendMain)
  .def("masterNames", &ActionRelationships::masterNames);

class_<Action, bases<NamedEntity> >("Action")
  .def(init<std::string, std::string>())
  .add_property("descripcion", make_function( &Action::GetDescripcion, return_value_policy<copy_const_reference>() ), &Action::SetDescripcion)
  .add_property("getRelaciones", make_function( &Action::getRelaciones, return_internal_reference<>() ))
  .add_property("weightingFactor", &Action::getWeightingFactor, &Action::setWeightingFactor)
  .def("getExpandedName", &Action::getExpandedName)
  .def("getCoeficientes", &Action::getCoeficientes)
  .def("getComponents", &Action::getComponents)
  .def("getComponentDict", &Action::getComponentsPy)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

class_<ActionRValue, bases<Action>, boost::noncopyable >("ActionRValue", no_init)
  .def("getPartialSafetyFactors", make_function( &ActionRValue::getPartialSafetyFactors, return_internal_reference<>() ), "Return the partial safety factors for this action.")
  .def("setPartialSafetyFactors", &ActionRValue::setPartialSafetyFactors, "Set the partial safety factors for this action.")
  ;

typedef std::deque<ActionRValue> dq_action_r_value;
class_<dq_action_r_value >("dq_action_r_values")
  .def(vector_indexing_suite<dq_action_r_value>())
  ;

class_<ActionRValueList, bases<dq_action_r_value,CommandEntity> >("ActionRValueLists")
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

const ActionRValueList &(ActionsFamily::*getFamilyActions)(void) const= &ActionsFamily::getActions;
class_<ActionsFamily, bases<NamedEntity> >("ActionsFamily")
  .def(init<std::string>())
  .add_property("actions", make_function( getFamilyActions, return_internal_reference<>() ), &ActionsFamily::setActions)
  .def("insert", make_function(&ActionsFamily::insert,return_internal_reference<>()))
  ;

class_<ActionContainer, bases<CommandEntity> >("ActionContainer")
  .add_property("permanentActions", make_function( &ActionContainer::getPermanentActions, return_internal_reference<>() ), &ActionContainer::setPermanentActions)
  .add_property("ncPermanentActions", make_function( &ActionContainer::getPermanentActionsNC, return_internal_reference<>() ), &ActionContainer::setPermanentActionsNC,"return a reference to the families of non-constant permanent actions container.")
  .add_property("variableActions", make_function( &ActionContainer::getVariableActions, return_internal_reference<>() ), &ActionContainer::setVariableActions,"return a reference to the families of variable actions container.")
  .add_property("accidentalActions", make_function( &ActionContainer::getAccidentalActions, return_internal_reference<>() ), &ActionContainer::setAccidentalActions)
  .add_property("seismicActions", make_function( &ActionContainer::getSeismicActions, return_internal_reference<>() ), &ActionContainer::setSeismicActions)
  .def("insert", make_function(&ActionContainer::insert,return_internal_reference<>()))
  ;

class_<ActionsAndFactors, bases<ActionContainer> >("ActionsAndFactors")
  .def(init<Factors>())
  .def("getFactors", &ActionsAndFactors::getFactors, return_internal_reference<>())
  ;
