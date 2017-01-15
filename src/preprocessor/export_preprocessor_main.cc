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

void export_preprocessor_main(void)
  {
    using namespace boost::python;
    docstring_options doc_options;


class_<XC::MapSet, bases<EntCmd>, boost::noncopyable >("MapSet", no_init)
  .def("__getitem__",&XC::MapSet::getSet, return_internal_reference<>())
  .def("getSet", &XC::MapSet::getSet, return_internal_reference<>(),"Returns set by name.")
  .def("defSet", &XC::MapSet::defSet, return_internal_reference<>(),"Creates a new set with the name which is passed as a parameter.")
  .def("removeSet", &XC::MapSet::removeSet,"Deletes the set and removes it from the sets map.")

   ;


enum_<XC::dir_mallado>("meshDir")
  .value("I", XC::dirm_i)
  .value("J", XC::dirm_j)
  .value("K", XC::dirm_k)
   ;

XC::NodeLoader &(XC::Preprocessor::*getNodeLoaderRef)(void)= &XC::Preprocessor::getNodeLoader;
XC::MaterialLoader &(XC::Preprocessor::*getMaterialLoaderRef)(void)= &XC::Preprocessor::getMaterialLoader;
XC::BeamIntegratorLoader &(XC::Preprocessor::*getBeamIntegratorLoaderRef)(void)= &XC::Preprocessor::getBeamIntegratorLoader;
XC::ElementLoader &(XC::Preprocessor::*getElementLoaderRef)(void)= &XC::Preprocessor::getElementLoader;
XC::ConstraintLoader &(XC::Preprocessor::*getConstraintLoaderRef)(void)= &XC::Preprocessor::getConstraintLoader;
XC::LoadLoader &(XC::Preprocessor::*getLoadLoaderRef)(void)= &XC::Preprocessor::getLoadLoader;
XC::TransfCooLoader &(XC::Preprocessor::*getTransfCooLoaderRef)(void)= &XC::Preprocessor::getTransfCooLoader;
XC::Cad &(XC::Preprocessor::*getCadRef)(void)= &XC::Preprocessor::getCad;
XC::MapSet &(XC::Preprocessor::*getSetsRef)(void)= &XC::Preprocessor::get_sets;
XC::Domain *(XC::Preprocessor::*getDomainRf)(void)= &XC::Preprocessor::GetDominio;
class_<XC::Preprocessor, bases<EntCmd>, boost::noncopyable >("Preprocessor", no_init)
  .add_property("getNodeLoader", make_function( getNodeLoaderRef, return_internal_reference<>() ))
  .add_property("getMaterialLoader", make_function( getMaterialLoaderRef, return_internal_reference<>() ))
  .add_property("getBeamIntegratorLoader", make_function( getBeamIntegratorLoaderRef, return_internal_reference<>() ))
  .add_property("getElementLoader", make_function( getElementLoaderRef, return_internal_reference<>() ))
  .add_property("getConstraintLoader", make_function( getConstraintLoaderRef, return_internal_reference<>() ))
  .add_property("getLoadLoader", make_function( getLoadLoaderRef, return_internal_reference<>() ))
  .add_property("getTransfCooLoader", make_function( getTransfCooLoaderRef, return_internal_reference<>() ))
  .add_property("getCad", make_function( getCadRef, return_internal_reference<>() ))
  .add_property("getSets", make_function( getSetsRef, return_internal_reference<>() ))
  .add_property("getDomain", make_function( getDomainRf, return_internal_reference<>() ))
  .def("resetLoadCase",&XC::Preprocessor::resetLoadCase)
  .def("setDeadSRF",XC::Preprocessor::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  ;

  }

