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
  .def("newSPConstraint", &XC::NodeLocker::newSPConstraint,return_internal_reference<>(),"\n" "newSPConstraint(nodeTag,dofId,value) \n" "Create a single-point boundary constraint by assigning a value to the degree of freedom dofId of node whose ID is nodeTag.")
  .def("removeSPConstraint", &XC::NodeLocker::removeSFreedom_Constraint,"Remove a single-point boundary constraint.")
  .def("clearAll",&XC::NodeLocker::clearAll)
  ;

class_<XC::LoadContainer, bases<CommandEntity>, boost::noncopyable >("LoadContainer", no_init)
  .add_property("getNodalLoadIter", make_function( &XC::LoadContainer::getNodalLoads, return_internal_reference<>() ),"return an iterator over the nodal loads.")
  .add_property("getElementalLoadIter", make_function( &XC::LoadContainer::getElementalLoads, return_internal_reference<>() ),"return an iterator over the elemental loads.")
  .add_property("getNumNodalLoads",&XC::LoadContainer::getNumNodalLoads,"return the number of nodal loads.")
    .add_property("getNumElementalLoads",&XC::LoadContainer::getNumElementalLoads,"return the number of elemental loads.")
  .add_property("getNumLoads",&XC::LoadContainer::getNumLoads,"return the totalnumber of loads.")
  ;

double &(XC::LoadPattern::*getGammaFRef)(void)= &XC::LoadPattern::GammaF;
XC::ElementalLoad *(XC::LoadPattern::*defElementalLoad)(const std::string &)= &XC::LoadPattern::newElementalLoad;
XC::LoadContainer &(XC::LoadPattern::*getLoadsRef)(void)= &XC::LoadPattern::getLoads;
class_<XC::LoadPattern, bases<XC::NodeLocker>, boost::noncopyable >("LoadPattern", no_init)
  .def("getName", make_function(&XC::LoadPattern::getName, return_value_policy<return_by_value>() ),"return the load pattern name.")
  .add_property("name", make_function(&XC::LoadPattern::getName, return_value_policy<return_by_value>() ),"return the load pattern name.")
  .add_property("description", make_function( &XC::LoadPattern::getDescription, return_value_policy<return_by_value>() ), &XC::LoadPattern::setDescription,"load case description.")
.add_property("loadFactor", make_function( &XC::LoadPattern::getLoadFactor, return_value_policy<return_by_value>() ), "Return the current load factor.")
.add_property("gammaF", make_function( getGammaFRef, return_value_policy<return_by_value>() ), &XC::LoadPattern::setGammaF,"Get/set the partial safety factor for this load pattern.")
  .add_property("constant", &XC::LoadPattern::getIsConstant, &XC::LoadPattern::setIsConstant,"determines if the load is constant in time or not.")
  .def("newNodalLoad", &XC::LoadPattern::newNodalLoad,return_internal_reference<>(),"Create a nodal load.")
  .add_property("getNumNodalLoads",&XC::LoadPattern::getNumNodalLoads,"return the number of nodal loads.")
  .add_property("getNumElementalLoads",&XC::LoadPattern::getNumElementalLoads,"return the number of elemental loads.")
  .add_property("getNumLoads",&XC::LoadPattern::getNumLoads,"return the totalnumber of loads.")
  .def("newElementalLoad", make_function(defElementalLoad,return_internal_reference<>()),"\n" "Create a load over an element.\n"  "Possible load types:  beam2d_uniform_load ,  beam2d_point_load , beam_strain_load , beam3d_point_load , beam3d_uniform_load , brick_self_weight , shell_uniform_load , bidim_strain_load , shell_strain_load , truss_temp_load ")
  .add_property("loads", make_function(getLoadsRef, return_internal_reference<>() ),"return a reference to the load container.")
  .def("removeNodalLoad",&XC::LoadPattern::removeNodalLoad,"removes the nodal load with the tag passed as parameter.")
  .def("removeElementalLoad",&XC::LoadPattern::removeElementalLoad,"remove the elemental load with the tag passed as parameter.")
  .def("clearLoads",&XC::LoadPattern::clearLoads,"Delete the pattern loads.")
  .def("addToDomain", &XC::LoadPattern::addToDomain,"Add load pattern to the domain.")
  .def("removeFromDomain", &XC::LoadPattern::removeFromDomain,"Removes the load pattern from the domain.")
  .def("isActive", &XC::LoadPattern::isActive,"Return true if the load pattern is currently added to the domain.")
  .def(self *= double())
  ;

#include "load_patterns/python_interface.tcc"

bool (XC::LoadPatternCombination::*addToDomain)(void)= &XC::LoadPatternCombination::addToDomain;
bool (XC::LoadPatternCombination::*addToDomainWithFilter)(boost::python::list &)= &XC::LoadPatternCombination::addToDomain;
class_<XC::LoadPatternCombination, XC::LoadPatternCombination *, bases<XC::ForceReprComponent>, boost::noncopyable >("LoadPatternCombination", no_init)
  .add_property("getName", make_function( &XC::LoadPatternCombination::getName, return_value_policy<return_by_value>() ), "Returns combination's name.")
  .add_property("name", make_function( &XC::LoadPatternCombination::getName, return_value_policy<return_by_value>() ), "Returns combination's name.")
  .def("addToDomain", addToDomain,"Add combination to the domain.")
  .def("addToDomain", addToDomainWithFilter,"Add only the load patterns of the combination whose name is in the list.")
  .def("removeFromDomain", &XC::LoadPatternCombination::removeFromDomain,"Remove combination from the domain.")
  .def("isActive", &XC::LoadPatternCombination::isActive,"Return true if the combination is fully added to the domain.")
  .def("getDescomp", &XC::LoadPatternCombination::getString,"Returns combination expression.")
  ;

XC::LoadCombination &(XC::LoadCombination::*add)(const std::string &)= &XC::LoadCombination::add;
XC::LoadCombination &(XC::LoadCombination::*subtract)(const std::string &)= &XC::LoadCombination::subtract;
class_<XC::LoadCombination, XC::LoadCombination *, bases<XC::LoadPatternCombination>, boost::noncopyable >("LoadCombination", no_init)
  .def("getCombPrevia", &XC::LoadCombination::getPtrCombPrevia,return_internal_reference<>(),"Returns previous load combination.")
  .def("add",add,return_internal_reference<>())
  .def("subtract",subtract,return_internal_reference<>())
  .def("multiplica",&XC::LoadCombination::multiplica,return_internal_reference<>())
  .def("divide",&XC::LoadCombination::divide,return_internal_reference<>())
  .def("asigna",&XC::LoadCombination::asigna,return_internal_reference<>())
  .def(self + self)
  .def(self *= double())
  ;

class_<XC::LoadCombinationMap, boost::noncopyable>("LoadCombinationMap")
//.def(map_indexing_suite<XC::LoadCombinationMap>() )
  ;

class_<XC::LoadCombinationGroup, bases<XC::LoadHandlerMember,XC::LoadCombinationMap>, boost::noncopyable >("LoadCombinationGroup", no_init)
  .def("newLoadCombination", &XC::LoadCombinationGroup::newLoadCombination,return_internal_reference<>(),"Creates a new load combination.")
  .def("addToDomain", &XC::LoadCombinationGroup::addToDomain,return_internal_reference<>(),"Add combination to the domain.")
  .def("remove", &XC::LoadCombinationGroup::remove,"Remove combination.")
  .def("removeFromDomain", &XC::LoadCombinationGroup::removeFromDomain,return_internal_reference<>(),"Remove combination from the domain.")
  .def("removeAllFromDomain", &XC::LoadCombinationGroup::removeAllFromDomain,return_internal_reference<>(),"Remove all loads cases from domain.")
  .def("getComb", &XC::LoadCombinationGroup::buscaLoadCombination,return_internal_reference<>(),"Returns load combination.")
  .def("getCombPrevia", &XC::LoadCombinationGroup::getPtrCombPrevia,return_internal_reference<>(),"Returns previous load combination.")
  .def("getKeys", &XC::LoadCombinationGroup::getKeys)
  .def("__getitem__",&XC::LoadCombinationGroup::buscaLoadCombination, return_value_policy<reference_existing_object>())
  .def("clear", &XC::LoadCombinationGroup::clear)
  ;

class_<XC::TimeSeries, bases<CommandEntity,XC::MovableObject>, boost::noncopyable >("TimeSeries", no_init)
  .add_property("name", make_function(&XC::TimeSeries::getName, return_value_policy<return_by_value>() ),"return the load pattern name.")
  .def("getFactor", &XC::TimeSeries::getFactor,"getFactor(pseudoTime): get load factor.")
  .def("getDuration",&XC::TimeSeries::getDuration,"Returns time series duration.")

  .def("getPeakFactor",&XC::TimeSeries::getPeakFactor,"Returns time series peak factor")
  .def("getTimeIncr", &XC::TimeSeries::getTimeIncr)
  ;

#include "time_series/python_interface.tcc"

class_<XC::TimeSeriesIntegrator , bases<XC::MovableObject>, boost::noncopyable >("TimeSeriesIntegrator", no_init);

#include "time_series_integrator/python_interface.tcc"

const XC::LoadPattern *(XC::MapLoadPatterns::*buscaLoadPatternByName)(const std::string &) const= &XC::MapLoadPatterns::buscaLoadPattern;
XC::MapLoadPatterns::const_iterator (XC::MapLoadPatterns::*cBegin)(void) const= &XC::MapLoadPatterns::begin;
XC::MapLoadPatterns::const_iterator (XC::MapLoadPatterns::*cEnd)(void) const= &XC::MapLoadPatterns::end;
class_<XC::MapLoadPatterns, bases<XC::LoadHandlerMember>, boost::noncopyable >("MapLoadPatterns", no_init)
  .add_property("defaultElementLoadTag", make_function( &XC::MapLoadPatterns::getCurrentElementLoadTag, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentElementLoadTag)
  .add_property("defaultNodeLoadTag", make_function( &XC::MapLoadPatterns::getCurrentElementLoadTag, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentElementLoadTag)
  .add_property("currentTimeSeries", make_function( &XC::MapLoadPatterns::getCurrentTimeSeries, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentTimeSeries)
  .def("newTimeSeries", &XC::MapLoadPatterns::newTimeSeries,return_internal_reference<>(),"Creates a time load modulation and associates it to the load pattern. Syntax: newTimeSeries(type,name), where type can be equal to 'constant_ts', 'linear_ts', 'path_ts', 'path_time_ts', 'pulse_ts','rectangular_ts', 'triangular_ts', 'trig_ts'")
  .add_property("currentLoadPattern", make_function( &XC::MapLoadPatterns::getCurrentLoadPattern, return_value_policy<copy_const_reference>() ), &XC::MapLoadPatterns::setCurrentLoadPattern, "Return the name of the current load pattern object.")
  .def("newLoadPattern", &XC::MapLoadPatterns::newLoadPattern,return_internal_reference<>(),"Creates a load pattern. Syntax: newLoadPattern(type,name), where type can be equal to 'default'(ordinary load pattern,'uniform_excitation','multi_support_pattern' or 'pbowl_loading'")
  .def("addToDomain", &XC::MapLoadPatterns::addToDomain,return_internal_reference<>(),"Applies the load pattern to the domain.")
  .def("removeFromDomain", &XC::MapLoadPatterns::removeFromDomain,return_internal_reference<>(),"Remove load case from the domain.")
  .def("removeAllFromDomain", &XC::MapLoadPatterns::removeAllFromDomain,return_internal_reference<>(),"Remove all loads cases from the domain.")
  .def("getKeys", &XC::MapLoadPatterns::getKeys,"Returns load case names")
  .def("__getitem__",buscaLoadPatternByName, return_value_policy<reference_existing_object>())
  .def("__iter__",range(cBegin, cEnd))
  ;
