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

const ActionWrapperList &(ActionsFamily::*getFamilyActions)(void) const= &ActionsFamily::getActions;
class_<ActionsFamily, bases<NamedEntity> >("ActionsFamily")
  .def(init<std::string>())
  .add_property("actions", make_function( getFamilyActions, return_internal_reference<>() ), &ActionsFamily::setActions)
  .def("insert", make_function(&ActionsFamily::insert,return_internal_reference<>()))
  ;

class_<ActionFamilyContainer, bases<CommandEntity> >("ActionFamilyContainer")
  .add_property("permanentActions", make_function( &ActionFamilyContainer::getPermanentActions, return_internal_reference<>() ), &ActionFamilyContainer::setPermanentActions)
  .add_property("ncPermanentActions", make_function( &ActionFamilyContainer::getPermanentActionsNC, return_internal_reference<>() ), &ActionFamilyContainer::setPermanentActionsNC,"return a reference to the families of non-constant permanent actions container.")
  .add_property("variableActions", make_function( &ActionFamilyContainer::getVariableActions, return_internal_reference<>() ), &ActionFamilyContainer::setVariableActions,"return a reference to the families of variable actions container.")
  .add_property("accidentalActions", make_function( &ActionFamilyContainer::getAccidentalActions, return_internal_reference<>() ), &ActionFamilyContainer::setAccidentalActions)
  .add_property("seismicActions", make_function( &ActionFamilyContainer::getSeismicActions, return_internal_reference<>() ), &ActionFamilyContainer::setSeismicActions)
  .def("insert", make_function(&ActionFamilyContainer::insert,return_internal_reference<>()))
  ;

class_<ActionsAndFactors, bases<ActionFamilyContainer> >("ActionsAndFactors")
  .def(init<Factors>())
  .def("getFactors", &ActionsAndFactors::getFactors, return_internal_reference<>())
  ;
