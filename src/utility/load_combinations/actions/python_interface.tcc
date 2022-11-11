//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#include "utility/load_combinations/actions/factors/python_interface.tcc"

class_<ActionRelationships, bases<CommandEntity> >("ActionRelationships")
  .def("appendIncompatible", &ActionRelationships::appendIncompatible)
  .def("incompatibleNames", &ActionRelationships::incompatibleNames)
  .def("appendMain", &ActionRelationships::appendMain)
  .def("masterNames", &ActionRelationships::masterNames);

class_<Action, bases<NamedEntity> >("Action")
  .def(init<std::string, std::string>())
  .add_property("descripcion", make_function( &Action::getDescription, return_value_policy<copy_const_reference>() ), &Action::setDescription, "DEPRECATED: use description-")
  .add_property("description", make_function( &Action::getDescription, return_value_policy<copy_const_reference>() ), &Action::setDescription, "return the action description.")
  .add_property("getRelaciones", make_function( &Action::getRelaciones, return_internal_reference<>() ), "DEPRECATED, use relationships")
  .add_property("relationships", make_function( &Action::getRelaciones, return_internal_reference<>() ), "Return the actions relationships with other actions")
  .add_property("weightingFactor", &Action::getWeightingFactor, &Action::setWeightingFactor)
  .add_property("not_determinant", &Action::NoDeterminante, &Action::setNoDeterminante)
  .def("getExpandedName", &Action::getExpandedName)
  .def("getCoeficientes", &Action::getCoeficientes)
  .def("getComponents", &Action::getComponents)
  .def("getComponentDict", &Action::getComponentsPy)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

class_<ActionWrapper, bases<EntityWithOwner>, boost::noncopyable >("ActionWrapper", no_init)
  .add_property("name", &ActionWrapper::getName,"returns the wrapper name.")
  .add_property("relationships", make_function( &ActionWrapper::getRelaciones, return_internal_reference<>() ), "Return the relationships with other actions")
  .def("getPartialSafetyFactors", make_function( &ActionWrapper::getPartialSafetyFactors, return_internal_reference<>() ), "Return the partial safety factors for this action.")
  .def("setPartialSafetyFactors", &ActionWrapper::setPartialSafetyFactors, "Set the partial safety factors for this action.")
  ;

typedef std::deque<std::shared_ptr<ActionWrapper> > dq_action_wrappers;
class_<dq_action_wrappers>("dq_action_wrappers")
.def(vector_indexing_suite<dq_action_wrappers, true>())
  ;

class_<ActionWrapperList, bases<dq_action_wrappers,CommandEntity> >("ActionWrapperLists")
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  .def("insert", make_function( &ActionWrapperList::insert, return_internal_reference<>() ), "insert(action, comb_factors_name, partial_safety_factors_name): inserts the action into the list.")
  .def("insertGroup", make_function( &ActionWrapperList::insertGroupPy, return_internal_reference<>() ), "insertGroup(actionTuples, partial_safety_factors_name): inserts the action group into the list.")
  ;

#include "utility/load_combinations/actions/containers/python_interface.tcc"
