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


class_<XC::NodeLocker,XC::NodeLocker *, bases<XC::ForceReprComponent>, boost::noncopyable >("NodeLocker", no_init)
  .def("newSPConstraint", &XC::NodeLocker::newSPConstraint,return_internal_reference<>(),"\n" "newSPConstraint(tag_nod,id_gdl,valor) \n" "Create a single-point boundary constraint by assigning a value=valor to the degree of freedom gdl of node whose ID is tag_nod.")
  .def("removeSPConstraint", &XC::NodeLocker::removeSFreedom_Constraint,"Remove a single-point boundary constraint.")
  .def("clearAll",&XC::NodeLocker::clearAll)
  ;

double &(XC::LoadPattern::*getGammaFRef)(void)= &XC::LoadPattern::GammaF;
XC::ElementalLoad *(XC::LoadPattern::*defElementalLoad)(const std::string &)= &XC::LoadPattern::newElementalLoad;
class_<XC::LoadPattern, bases<XC::NodeLocker>, boost::noncopyable >("LoadPattern", no_init)
  .add_property("description", make_function( &XC::LoadPattern::getDescription, return_value_policy<return_by_value>() ), &XC::LoadPattern::setGammaF,"load case description.")
  .add_property("loadFactor", make_function( &XC::LoadPattern::getLoadFactor, return_value_policy<return_by_value>() ))
  .add_property("gammaF", make_function( getGammaFRef, return_value_policy<return_by_value>() ), &XC::LoadPattern::setGammaF)
  .def("newNodalLoad", &XC::LoadPattern::newNodalLoad,return_internal_reference<>(),"Creates a nodal load.")
  .add_property("getNumNodalLoads",&XC::LoadPattern::getNumNodalLoads)
  .add_property("getNumElementalLoads",&XC::LoadPattern::getNumElementalLoads)
  .add_property("getNumLoads",&XC::LoadPattern::getNumLoads)
  .def("newElementalLoad", make_function(defElementalLoad,return_internal_reference<>()),"Crea una load over element.")
  .add_property("getNodalLoadIter", make_function( &XC::LoadPattern::getNodalLoads, return_internal_reference<>() ))
  .add_property("getElementalLoadIter", make_function( &XC::LoadPattern::getElementalLoads, return_internal_reference<>() ))
  .def("removeNodalLoad",&XC::LoadPattern::removeNodalLoad,"removes the nodal load with the tag passed as parameter.")
  .def("removeElementalLoad",&XC::LoadPattern::removeElementalLoad,"removes the elemental load with the tag passed as parameter.")
  .def("clearLoads",&XC::LoadPattern::clearLoads,"Deletes the pattern loads.")
  .def("addToDomain", &XC::LoadPattern::addToDomain,"Add combination to the domain.")
   .def("removeFromDomain", &XC::LoadPattern::removeFromDomain,"Eliminates combination from domain.")
  ;

#include "load_patterns/python_interface.tcc"

XC::LoadCombination &(XC::LoadCombination::*suma)(const std::string &)= &XC::LoadCombination::suma;
XC::LoadCombination &(XC::LoadCombination::*resta)(const std::string &)= &XC::LoadCombination::resta;
class_<XC::LoadCombination, XC::LoadCombination *, bases<XC::ForceReprComponent>, boost::noncopyable >("LoadCombination", no_init)
  .add_property("getName", make_function( &XC::LoadCombination::getNombre, return_value_policy<return_by_value>() ), "Returns combination's name.")
  .def("addToDomain", &XC::LoadCombination::addToDomain,"Add combination to the domain.")
  .def("removeFromDomain", &XC::LoadCombination::removeFromDomain,"Eliminates combination from domain.")
  .def("getCombPrevia", &XC::LoadCombination::getPtrCombPrevia,return_internal_reference<>(),"Returns previous load combination.")
  .def("getDescomp", &XC::LoadCombination::getString,"Returns combination expression.")
  .def("suma",suma,return_internal_reference<>())
  .def("resta",resta,return_internal_reference<>())
  .def("multiplica",&XC::LoadCombination::multiplica,return_internal_reference<>())
  .def("divide",&XC::LoadCombination::divide,return_internal_reference<>())
  .def("asigna",&XC::LoadCombination::asigna,return_internal_reference<>())
  ;

class_<XC::LoadCombinationMap, boost::noncopyable>("LoadCombinationMap")
//.def(map_indexing_suite<XC::LoadCombinationMap>() )
  ;

class_<XC::LoadCombinationGroup, bases<XC::LoadLoaderMember,XC::LoadCombinationMap>, boost::noncopyable >("LoadCombinationGroup", no_init)
  .def("newLoadCombination", &XC::LoadCombinationGroup::newLoadCombination,return_internal_reference<>(),"Creates a new load combination.")
  .def("addToDomain", &XC::LoadCombinationGroup::addToDomain,return_internal_reference<>(),"Add combination to the domain.")
  .def("remove", &XC::LoadCombinationGroup::remove,"Removes combination.")
  .def("removeFromDomain", &XC::LoadCombinationGroup::removeFromDomain,return_internal_reference<>(),"Eliminates combination from domain.")
  .def("removeAllFromDomain", &XC::LoadCombinationGroup::removeAllFromDomain,return_internal_reference<>(),"Eliminates all loads cases from domain.")
  .def("getComb", &XC::LoadCombinationGroup::buscaLoadCombination,return_internal_reference<>(),"Returns load combination.")
  .def("getCombPrevia", &XC::LoadCombinationGroup::getPtrCombPrevia,return_internal_reference<>(),"Returns previous load combination.")
  .def("getKeys", &XC::LoadCombinationGroup::getKeys)
  .def("__getitem__",&XC::LoadCombinationGroup::buscaLoadCombination, return_value_policy<reference_existing_object>())
  .def("clear", &XC::LoadCombinationGroup::clear)
  ;

class_<XC::TimeSeries, bases<EntCmd,XC::MovableObject>, boost::noncopyable >("TimeSeries", no_init)
  .def("getFactor", &XC::TimeSeries::getFactor)
  .add_property("getDuration", &XC::TimeSeries::getDuration)
  .add_property("getPeakFactor", &XC::TimeSeries::getPeakFactor)
  .def("getTimeIncr", &XC::TimeSeries::getTimeIncr)
  ;

#include "time_series/python_interface.tcc"

class_<XC::TimeSeriesIntegrator , bases<XC::MovableObject>, boost::noncopyable >("TimeSeriesIntegrator", no_init);

#include "time_series_integrator/python_interface.tcc"

const XC::LoadPattern *(XC::MapLoadPatterns::*buscaLoadPatternByName)(const std::string &) const= &XC::MapLoadPatterns::buscaLoadPattern;
class_<XC::MapLoadPatterns, bases<XC::LoadLoaderMember>, boost::noncopyable >("MapLoadPatterns", no_init)
  .add_property("defaultElementLoadTag", make_function( &XC::MapLoadPatterns::getCurrentElementLoadTag, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentElementLoadTag)
  .add_property("defaultNodeLoadTag", make_function( &XC::MapLoadPatterns::getCurrentElementLoadTag, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentElementLoadTag)
  .add_property("currentTimeSeries", make_function( &XC::MapLoadPatterns::getCurrentTimeSeries, return_internal_reference<>() ), &XC::MapLoadPatterns::setCurrentTimeSeries)
  .def("newTimeSeries", &XC::MapLoadPatterns::newTimeSeries,return_internal_reference<>(),"Crea una modulación de the load en el tiempo.")
  .add_property("currentLoadPattern", make_function( &XC::MapLoadPatterns::getCurrentLoadPattern, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentLoadPattern)
  .def("newLoadPattern", &XC::MapLoadPatterns::newLoadPattern,return_internal_reference<>(),"Creates a load pattern.")
  .def("addToDomain", &XC::MapLoadPatterns::addToDomain,return_internal_reference<>(),"Applies the load pattern to the domain.")
  .def("removeFromDomain", &XC::MapLoadPatterns::removeFromDomain,return_internal_reference<>(),"Eliminates load case from domain.")
  .def("removeAllFromDomain", &XC::MapLoadPatterns::removeAllFromDomain,return_internal_reference<>(),"Eliminates all loads cases from domain.")
  .def("getKeys", &XC::MapLoadPatterns::getKeys,"Returns load case names")
  .def("__getitem__",buscaLoadPatternByName, return_value_policy<reference_existing_object>())
  ;
