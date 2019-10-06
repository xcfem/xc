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

//Python exposition of nodes.
#include "domain/mesh/node/python_interface.tcc"

//Python exposition of the elements.
#include "domain/mesh/element/python_interface.tcc"

class_<XC::MeshComponentContainer, bases<CommandEntity>, boost::noncopyable >("MeshComponentContainer", no_init)
  ;

XC::Node *(XC::Mesh::*getNodePtr)(int tag)= &XC::Mesh::getNode;
XC::Node *(XC::Mesh::*getNearestNodePtrMesh)(const Pos3d &)= &XC::Mesh::getNearestNode;
XC::Element *(XC::Mesh::*getNearestElementPtrMesh)(const Pos3d &)= &XC::Mesh::getNearestElement;
XC::Element *(XC::Mesh::*getElementPtr)(int tag)= &XC::Mesh::getElement;
class_<XC::Mesh, bases<XC::MeshComponentContainer>, boost::noncopyable >("Mesh", no_init)
  .add_property("getNodeIter", make_function( &XC::Mesh::getNodes, return_internal_reference<>() ))
  .def("getNumNodes", &XC::Mesh::getNumNodes,"Returns the number of nodes.")
  .def("getNode", make_function(getNodePtr, return_internal_reference<>() ),"Returns a node from its identifier.")
  .def("getNearestNode",make_function(getNearestNodePtrMesh, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNumLiveNodes", &XC::Mesh::getNumLiveNodes,"Returns the number of live nodes.")
  .def("getNumDeadNodes", &XC::Mesh::getNumDeadNodes,"Returns the number of dead nodes.")
  .def("getNumFrozenNodes", &XC::Mesh::getNumFrozenNodes,"Returns the number of frozen nodes.")
  .def("getNumFreeNodes", &XC::Mesh::getNumFreeNodes,"Returns the number of free nodes.")
  .def("freezeDeadNodes",&XC::Mesh::freeze_dead_nodes,"Restrain movement of dead nodes. Syntax: freezeDeadNodes(lockerName)")
  .def("meltAliveNodes",&XC::Mesh::melt_alive_nodes,"Allows movement of melted nodes.")
  .def("calculateNodalReactions",&XC::Mesh::calculateNodalReactions,"triggers nodal reaction calculation.")
  .def("checkNodalReactions",&XC::Mesh::checkNodalReactions,"checkNodalReactions(tolerande): check that reactions at nodes correspond to constrained degrees of freedom.")
  .add_property("getElementIter", make_function( &XC::Mesh::getElements, return_internal_reference<>() ),"returns an iterator over the elements of the mesh.")
  .def("getElement", make_function(getElementPtr, return_internal_reference<>() ),"Returns an element from its identifier.")
  .def("getNumElements", &XC::Mesh::getNumElements,"Returns the number of elements.")
  .def("getNumLiveElements", &XC::Mesh::getNumLiveElements,"Returns the number of live elements.")
  .def("getNumDeadElements", &XC::Mesh::getNumDeadElements,"Returns the number of dead elements.")
  .def("getNearestElement",make_function(getNearestElementPtrMesh, return_internal_reference<>() ),"Returns nearest node.")
  .def("setDeadSRF",XC::Mesh::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation. Syntax: setDeadSRF(factor)")
  .def("normalizeEigenvectors",&XC::Mesh::normalizeEigenvectors,"Normalize node eigenvectors for the argument mode. Syntax: normalizeEigenvectors(mode)")
  .staticmethod("setDeadSRF")
  ;
