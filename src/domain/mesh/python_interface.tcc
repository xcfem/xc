//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

//Exposición de los nodos.
#include "domain/mesh/node/python_interface.tcc"

//Exposición de los elementos.
#include "domain/mesh/element/python_interface.tcc"

class_<XC::MeshComponentContainer, bases<EntCmd>, boost::noncopyable >("MeshComponentContainer", no_init)
  ;

XC::Node *(XC::Mesh::*getNearestNodePtrMesh)(const Pos3d &)= &XC::Mesh::getNearestNode;
XC::Element *(XC::Mesh::*getNearestElementPtrMesh)(const Pos3d &)= &XC::Mesh::getNearestElement;
class_<XC::Mesh, bases<XC::MeshComponentContainer>, boost::noncopyable >("Mesh", no_init)
  .add_property("getNodeIter", make_function( &XC::Mesh::getNodes, return_internal_reference<>() ))
  .def("getNumNodes", &XC::Mesh::getNumNodes,"Returns the number of nodes.")
  .def("getNumLiveNodes", &XC::Mesh::getNumLiveNodes,"Returns the number of live nodes.")
  .def("getNumDeadNodes", &XC::Mesh::getNumDeadNodes,"Returns the number of dead nodes.")
  .def("getNumFrozenNodes", &XC::Mesh::getNumFrozenNodes,"Returns the number of frozen nodes.")
  .def("getNumFreeNodes", &XC::Mesh::getNumFreeNodes,"Returns the number of free nodes.")
  .def("freezeDeadNodes",&XC::Mesh::freeze_dead_nodes,"Restrain movement of dead nodes.")
  .def("meltAliveNodes",&XC::Mesh::melt_alive_nodes,"Allows movement of melted nodes.")
  .add_property("getElementIter", make_function( &XC::Mesh::getElements, return_internal_reference<>() ))
  .def("getNumElements", &XC::Mesh::getNumElements,"Returns the number of elements.")
  .def("getNumLiveElements", &XC::Mesh::getNumLiveElements,"Returns the number of live elements.")
  .def("getNumDeadElements", &XC::Mesh::getNumDeadElements,"Returns the number of dead elements.")
  .def("getNearestNode",make_function(getNearestNodePtrMesh, return_internal_reference<>() ),"Returns nearest node.")
  .def("getNearestElement",make_function(getNearestElementPtrMesh, return_internal_reference<>() ),"Returns nearest node.")
  .def("setDeadSRF",XC::Mesh::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .staticmethod("setDeadSRF")
  ;
