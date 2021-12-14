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

typedef std::vector<Action> v_accion;
class_<v_accion >("v_accion")
  .def(vector_indexing_suite<v_accion >())
 ;

const ActionsAndFactors *(ActionWeightingMap::*findActionsAndFactorsByName)(const std::string &) const= &ActionWeightingMap::findByName;
ActionWeightingMap::const_iterator (ActionWeightingMap::*awMapBegin)(void) const= &ActionWeightingMap::begin;
ActionWeightingMap::const_iterator (ActionWeightingMap::*awMapEnd)(void) const= &ActionWeightingMap::end;
class_<ActionWeightingMap, bases<NamedEntity>, boost::noncopyable >("ActionWeightingMap", no_init)
  .def("getKeys", &ActionWeightingMap::getKeys,"Returns load case names")
  .def("__len__",&ActionWeightingMap::size)
  .def("__getitem__",findActionsAndFactorsByName, return_value_policy<reference_existing_object>())
  .def("__iter__",range(awMapBegin, awMapEnd))
  .def("create", make_function(&ActionWeightingMap::create,return_internal_reference<>()),"Create a set of actions with its factors.")
  ;

class_<LoadCombinationVector, bases<v_accion,CommandEntity> >("LoadCombContainer")
  ;

class_<LoadCombinations, bases<CommandEntity> >("Combinations")
  .add_property("getULSTransientCombinations", &LoadCombinations::getULSTransientCombinations)
  .add_property("getULSAccidentalCombinations", &LoadCombinations::getULSAccidentalCombinations)
  .add_property("getULSSeismicCombinations", &LoadCombinations::getULSSeismicCombinations)

  .add_property("getSLSCharacteristicCombinations", &LoadCombinations::getSLSCharacteristicCombinations)
  .add_property("getSLSFrequentCombinations", &LoadCombinations::getSLSFrequentCombinations)
  .add_property("getSLSQuasiPermanentCombinations", &LoadCombinations::getSLSQuasiPermanentCombinations)
;


class_<LoadCombinationGenerator, bases<CommandEntity> >("LoadCombGenerator")
  .add_property("actionWeighting", make_function( &LoadCombinationGenerator::getActionWeighting, return_internal_reference<>() ), &LoadCombinationGenerator::setActionWeighting)
.def("insert", make_function(&LoadCombinationGenerator::insert, return_internal_reference<>(), (arg("weighting"), arg("family"), arg("action"), arg("combination_factors_name"), arg("partial_safety_factors_name"))),"Insert an action into the load combination generator.")
  .def("genera", &LoadCombinationGenerator::genera, "generate the load combinations.")
  .add_property("getLoadCombinations", make_function(&LoadCombinationGenerator::getLoadCombinations,return_internal_reference<>()))
  ;

