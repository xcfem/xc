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
//export_preprocessor_main.cxx

#include "python_interface.h"
#include "FEProblem.h"

void export_preprocessor_main(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

    typedef std::map<std::string,XC::SetBase *> map_sets;
    class_<map_sets, boost::noncopyable >("map_sets", no_init)
      //.def(boost::python::map_indexing_suite<map_sets >())
      ;
    class_<XC::MapSetBase , bases<map_sets>, boost::noncopyable >("MapSetBase", no_init)
      .def("removeSet", &XC::MapSetBase::removeSet,"Delete the set and remove it from the sets map.")
      .def("exists",&XC::MapSetBase::exists,"Return true if the sets already exists..")
      .def("getSetsNames",&XC::MapSetBase::getSetsNamesPy,"Returns the names of the sets.")
      ;
 
    XC::MapSet::const_iterator (XC::MapSet::*cBegin)(void) const= &XC::MapSet::begin;
    XC::MapSet::const_iterator (XC::MapSet::*cEnd)(void) const= &XC::MapSet::end;
    class_<XC::MapSet, bases<XC::PreprocessorContainer,XC::MapSetBase>, boost::noncopyable >("MapSet", no_init)
      .def("__iter__",range(cBegin, cEnd))
      .def("__getitem__",&XC::MapSet::getSet, return_internal_reference<>())
      .def("getSet", &XC::MapSet::getSet, return_internal_reference<>(),"Returns set by name.")
      .def("defSet", &XC::MapSet::defSet, return_internal_reference<>(),"Creates a new set with the name which is passed as a parameter.")
      ;


enum_<XC::meshing_dir>("meshDir")
  .value("I", XC::dirm_i)
  .value("J", XC::dirm_j)
  .value("K", XC::dirm_k)
   ;

XC::NodeHandler &(XC::Preprocessor::*getNodeHandlerRef)(void)= &XC::Preprocessor::getNodeHandler;
XC::MaterialHandler &(XC::Preprocessor::*getMaterialHandlerRef)(void)= &XC::Preprocessor::getMaterialHandler;
XC::BeamIntegratorHandler &(XC::Preprocessor::*getBeamIntegratorHandlerRef)(void)= &XC::Preprocessor::getBeamIntegratorHandler;
XC::ElementHandler &(XC::Preprocessor::*getElementHandlerRef)(void)= &XC::Preprocessor::getElementHandler;
XC::BoundaryCondHandler &(XC::Preprocessor::*getBoundaryCondHandlerRef)(void)= &XC::Preprocessor::getBoundaryCondHandler;
XC::LoadHandler &(XC::Preprocessor::*getLoadHandlerRef)(void)= &XC::Preprocessor::getLoadHandler;
XC::TransfCooHandler &(XC::Preprocessor::*getTransfCooHandlerRef)(void)= &XC::Preprocessor::getTransfCooHandler;
XC::MultiBlockTopology &(XC::Preprocessor::*getMultiBlockTopologyRef)(void)= &XC::Preprocessor::getMultiBlockTopology;
XC::MapSet &(XC::Preprocessor::*getSetsRef)(void)= &XC::Preprocessor::get_sets;
XC::Domain *(XC::Preprocessor::*getDomainRf)(void)= &XC::Preprocessor::getDomain;
XC::FEProblem *(XC::Preprocessor::*getProblemRf)(void)= &XC::Preprocessor::getProblem;
class_<XC::Preprocessor, bases<CommandEntity>, boost::noncopyable >("Preprocessor", no_init)
  .add_property("getNodeHandler", make_function( getNodeHandlerRef, return_internal_reference<>() ))
  .add_property("getMaterialHandler", make_function( getMaterialHandlerRef, return_internal_reference<>() ))
  .add_property("getBeamIntegratorHandler", make_function( getBeamIntegratorHandlerRef, return_internal_reference<>() ))
  .add_property("getElementHandler", make_function( getElementHandlerRef, return_internal_reference<>() ))
  .add_property("getBoundaryCondHandler", make_function( getBoundaryCondHandlerRef, return_internal_reference<>() ))
  .add_property("getLoadHandler", make_function( getLoadHandlerRef, return_internal_reference<>() ))
  .add_property("getTransfCooHandler", make_function( getTransfCooHandlerRef, return_internal_reference<>() ))
  .add_property("getMultiBlockTopology", make_function( getMultiBlockTopologyRef, return_internal_reference<>() ))
  .add_property("getSets", make_function( getSetsRef, return_internal_reference<>() ))
  .add_property("getDomain", make_function( getDomainRf, return_internal_reference<>() ))
  .add_property("getProblem", make_function( getProblemRf, return_internal_reference<>() ))
  .def("resetLoadCase",&XC::Preprocessor::resetLoadCase)
  .def("setDeadSRF",XC::Preprocessor::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  ;

  }

