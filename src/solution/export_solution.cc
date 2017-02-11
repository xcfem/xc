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
//utils_python_interface.cxx

#include "python_interface.h"
#include "ProblemaEF.h"

void export_solution(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "analysis/python_interface.tcc"
#include "system_of_eqn/python_interface.tcc"

class_<XC::ConvergenceTest, bases<XC::MovableObject,EntCmd>, boost::noncopyable >("ConvergenceTest", no_init);

class_<XC::SoluMethod, bases<EntCmd>, boost::noncopyable >("SoluMethod", no_init)
    .def("newSolutionAlgorithm", &XC::SoluMethod::newSolutionAlgorithm,return_internal_reference<>(),"Defines the solution algorithm to use.")
    .def("newIntegrator", &XC::SoluMethod::newIntegrator,return_internal_reference<>(),"Define el integrator to use.")
    .def("newSystemOfEqn", &XC::SoluMethod::newSystemOfEqn,return_internal_reference<>(),"Define el system of equations to use.")
    .def("newConvergenceTest", &XC::SoluMethod::newConvergenceTest,return_internal_reference<>(),"Define el criterio de convergencia to use.")
    ;

class_<XC::MapSoluMethod, bases<EntCmd>, boost::noncopyable >("MapSoluMethod", no_init)
    .add_property("existeSoluMethod", &XC::MapSoluMethod::existeSoluMethod)
    .def("newSoluMethod", &XC::MapSoluMethod::newSoluMethod,return_internal_reference<>(),"Creates a new solution procedure.")
    ;

XC::ModelWrapper *(XC::ProcSoluControl::*getModelWrapperPtr)(const std::string &)= &XC::ProcSoluControl::getModelWrapper;
class_<XC::ProcSoluControl, bases<EntCmd>, boost::noncopyable >("SoluControl", no_init)
    .add_property("getModelWrapper", make_function( getModelWrapperPtr, return_internal_reference<>() ))
    .add_property("getModelWrapperContainer",  make_function(&XC::ProcSoluControl::getModelWrapperContainer, return_internal_reference<>()) )
    .add_property("getSoluMethodContainer",  make_function(&XC::ProcSoluControl::getSoluMethodContainer, return_internal_reference<>()) )
    ;

XC::ProcSoluControl &(XC::ProcSolu::*getSoluControlRef)(void)= &XC::ProcSolu::getSoluControl;
class_<XC::ProcSolu, bases<EntCmd>, boost::noncopyable >("ProcSolu", no_init)
    .add_property("getSoluControl", make_function( getSoluControlRef, return_internal_reference<>() ))
    .add_property("getAnalysis", make_function( &XC::ProcSolu::getAnalysis, return_internal_reference<>() ))
    .def("newAnalysis", &XC::ProcSolu::newAnalysis,return_internal_reference<>(),"Creates a new analysis.")
    ;

  }

