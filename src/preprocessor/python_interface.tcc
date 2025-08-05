//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
bool (XC::Preprocessor::*removeElement)(XC::Element *)= &XC::Preprocessor::remove;
bool (XC::Preprocessor::*removeNode)(XC::Node *)= &XC::Preprocessor::remove;
bool (XC::Preprocessor::*removeConstraint)(XC::Constraint *)= &XC::Preprocessor::remove;
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
  .def("removeElement", removeElement, "Remove the given element from the FE problem.")
  .def("removeElement", &XC::Preprocessor::removeElement, "Remove the element with the given tag from the FE problem.")
  .def("removeNode", removeNode, "Remove the given node from the FE problem.")
  .def("removeNode", &XC::Preprocessor::removeNode, "Remove the node with the given tag from the FE problem.")
  .def("removeConstraint", removeConstraint, "Remove the given node from the FE problem.")
  .def("removeSFreedom_Constraint", &XC::Preprocessor::removeSFreedom_Constraint, "Remove the constraint with the given tag from the FE problem.")
  .def("removeMFreedom_Constraint", &XC::Preprocessor::removeMFreedom_Constraint, "Remove the constraint with the given tag from the FE problem.")
  .def("removeMRMFreedom_Constraint", &XC::Preprocessor::removeMRMFreedom_Constraint, "Remove the constraint with the given tag from the FE problem.")
  .def("clearAll",  &XC::Preprocessor::clearAll, "Removes all the objects contained in the preprocessor")
  ;
