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

class_<cmb_acc::SLSPartialSafetyFactors, bases<CommandEntity> >("SLSPartialSafetyFactors")
  .def(init<float, float>())
  .add_property("favorable", &cmb_acc::SLSPartialSafetyFactors::getFavorable, &cmb_acc::SLSPartialSafetyFactors::setFavorable)
  .add_property("desfavorable", &cmb_acc::SLSPartialSafetyFactors::getDesfavorable, &cmb_acc::SLSPartialSafetyFactors::setDesfavorable)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

class_<ULSPartialSafetyFactors, bases<SLSPartialSafetyFactors> >("ULSPartialSafetyFactors")
  .def(init<float, float,float,float>())
  .add_property("favorableAccidental", &ULSPartialSafetyFactors::getFavorableAccidental, &ULSPartialSafetyFactors::setFavorableAccidental)
  .add_property("desfavorableAccidental", &ULSPartialSafetyFactors::getDesfavorableAccidental, &ULSPartialSafetyFactors::setDesfavorableAccidental);

class_<PartialSafetyFactors, bases<CommandEntity> >("PartialSafetyFactors")
  .def(init<ULSPartialSafetyFactors, SLSPartialSafetyFactors>())
  .add_property("getULSPartialSafetyFactors", make_function( &PartialSafetyFactors::getULSPartialSafetyFactors, return_internal_reference<>() ))
  .add_property("getSLSPartialSafetyFactors", make_function( &PartialSafetyFactors::getSLSPartialSafetyFactors, return_internal_reference<>() ))
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

typedef std::map<std::string,PartialSafetyFactors> map_partial_safety_factors;
class_<map_partial_safety_factors >("map_partial_safety_factorss")
  .def(map_indexing_suite<map_partial_safety_factors>())
  ;

typedef FactorsMap<PartialSafetyFactors> factors_map_partial_safety_factors;
class_<factors_map_partial_safety_factors, bases<CommandEntity,map_partial_safety_factors> >("factors_map_partial_safety_factors")
  ;

class_<PartialSafetyFactorsMap, bases<factors_map_partial_safety_factors> >("PartialSafetyFactorsDict")
  .def("get", &PartialSafetyFactorsMap::BuscaCoefs, return_value_policy<copy_const_reference>())
  .def("insert", &PartialSafetyFactorsMap::insert);

class_<CombinationFactors, bases<CommandEntity> >("CombinationFactors")
  .def(init<double, double, double>())
  .def("getCombinationFactor", &CombinationFactors::getCombinationFactor, return_value_policy<copy_const_reference>(),"Return the r-th combination factor.")
  ;

typedef std::map<std::string,CombinationFactors> map_combination_factors;
class_<map_combination_factors >("map_combination_factors")
  .def(map_indexing_suite<map_combination_factors>())
  ;

typedef FactorsMap<CombinationFactors> factors_map_combination_factors;
class_<factors_map_combination_factors, bases<CommandEntity,map_combination_factors> >("factors_map_combination_factors")
  ;

class_<CombinationFactorsMap, bases<factors_map_combination_factors> >("CombinationFactorsDict")
  .def("get", &CombinationFactorsMap::BuscaCoefs, return_value_policy<copy_const_reference>())
  .def("insert", &CombinationFactorsMap::insert);


class_<Factors, bases<CommandEntity> >("Factors")
  .def("getPartialSafetyFactors", &Factors::getPartialSafetyFactors, return_internal_reference<>())
  .def("getCombinationFactors", &Factors::getCombinationFactors, return_internal_reference<>())
  ;
