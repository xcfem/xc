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

class_<XC::Information, boost::noncopyable >("Information", no_init);

class_<XC::ParticlePos2d>("ParticlePos2d")
  .def(init<const double &, const double &>())
  .def(init<Pos2d>())
  .add_property("r", make_function( &XC::ParticlePos2d::r_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos2d::set_r_coordinate, "r coordinate.")
  .add_property("s", make_function( &XC::ParticlePos2d::s_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos2d::set_s_coordinate, "s coordinate.")
  .def(self_ns::str(self_ns::self))
  ;

class_<XC::ParticlePos3d, bases<XC::ParticlePos2d> >("ParticlePos3d")
  .def(init<const double &, const double &, const double &>())
  .def(init<Pos2d>())
  .def(init<Pos3d>())
  .add_property("t", make_function( &XC::ParticlePos3d::t_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_t_coordinate, "t coordinate.")
  .def(self_ns::str(self_ns::self))
  ;

class_<XC::RayleighDampingFactors, bases<CommandEntity> >("RayleighDampingFactors")
  .def(init<double,double,double,double>())
  .add_property("alphaM",make_function( &XC::RayleighDampingFactors::getAlphaM, return_value_policy<return_by_value>()), &XC::RayleighDampingFactors::setAlphaM, "factor applied to elements or nodes mass matrix.")
  .add_property("betaK",make_function( &XC::RayleighDampingFactors::getBetaK, return_value_policy<return_by_value>()), &XC::RayleighDampingFactors::setBetaK, "factor applied to elements current stiffness matrix.")
  .add_property("betaKinit",make_function( &XC::RayleighDampingFactors::getBetaK0, return_value_policy<return_by_value>()), &XC::RayleighDampingFactors::setBetaK0, "factor applied to elements initial stiffness matrix.")
  .add_property("betaKcommit",make_function( &XC::RayleighDampingFactors::getBetaKc, return_value_policy<return_by_value>()), &XC::RayleighDampingFactors::setBetaKc, "factor applied to elements committed stiffness matrix.")
  .def(self_ns::str(self_ns::self))
  .def("getCurrentDampingRatio", &XC::RayleighDampingFactors::getCurrentDampingRatio, "Return the current damping value for the natural frequency argument (alphaM/(2*w)+betaK*w/2")
  .def("getInitialDampingRatio", &XC::RayleighDampingFactors::getInitialDampingRatio, "Return the current initial value for the natural frequency argument (alphaM/(2*w)+betaK*w/2")
  .def("getCommitedDampingRatio", &XC::RayleighDampingFactors::getCommitedDampingRatio, "Return the commited damping value for the natural frequency argument (alphaM/(2*w)+betaK*w/2")
  ;

typedef XC::NodePtrs::vector_ptr_nodes vector_ptr_nodes;
class_<vector_ptr_nodes, boost::noncopyable>("vector_ptr_nodes")
  .def(vector_indexing_suite<vector_ptr_nodes>() )
  ;

XC::Node *(XC::NodePtrs::*getNearestNodePtr)(const Pos3d &,bool initialGeometry)= &XC::NodePtrs::getNearestNode;
bool (XC::NodePtrs::*NodePtrsIn3D)(const GeomObj3d &,const double &,const double &) const= &XC::NodePtrs::In;
bool (XC::NodePtrs::*NodePtrsOut3D)(const GeomObj3d &,const double &,const double &) const= &XC::NodePtrs::Out;
int (XC::NodePtrs::*getNodeIndexPTR)(const XC::Node *) const= &XC::NodePtrs::getNodeIndex;
int (XC::NodePtrs::*getNodeIndexTAG)(const int &tag) const= &XC::NodePtrs::getNodeIndex;
boost::python::list (XC::NodePtrs::*get_connected_elements_node_ptrs_py)(void)= &XC::NodePtrs::getConnectedElementsPy;    
boost::python::list (XC::NodePtrs::*get_connected_elements_node_ptrs_set_py)(const XC::SetBase *)= &XC::NodePtrs::getConnectedElementsPy; 
boost::python::list (XC::NodePtrs::*get_connected_elements_tags_node_ptrs_py)(void) const= &XC::NodePtrs::getConnectedElementTags;    
boost::python::list (XC::NodePtrs::*get_connected_elements_tags_node_ptrs_set_py)(const XC::SetBase *) const= &XC::NodePtrs::getConnectedElementTags; 
class_<XC::NodePtrs, bases<CommandEntity,vector_ptr_nodes>, boost::noncopyable >("NodePtrs", no_init)
  .def("getNearestNode",make_function(getNearestNodePtr, return_internal_reference<>() ),"Return nearest node.")
  .def("In", NodePtrsIn3D,"\n""In(geomObject,factor,tolerance) \n""Return true if the current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie inside the geometric object.")
  .def("Out", NodePtrsOut3D,"\n""Out(geomObject,factor,tolerance) \n""Return true if current positions of all the nodes scaled by a factor: initialPos+factor*currentDisplacement lie outside the geometric object.")
  .def("getNodeIndex", getNodeIndexPTR, " Return the index of the node in the array.")
  .def("getNodeTAG", getNodeIndexTAG, " Return the index of the node in the array.")
  .add_property("connectedElements", get_connected_elements_node_ptrs_py, "Returns the elements connected to any of these nodes.")
  .def("getConnectedElements", get_connected_elements_node_ptrs_set_py, "Returns the elements from the given set that are connected to any of these nodes.")
.add_property("connectedElementTags", get_connected_elements_tags_node_ptrs_py, "Returns the tags of the elements connected to any of these nodes.")
  .def("getConnectedElementTags", get_connected_elements_tags_node_ptrs_set_py, "Returns the tags of the elements from the given set that are connected to any of these nodes.")
  ;

class_<XC::NodePtrsWithIDs, bases<XC::NodePtrs>, boost::noncopyable >("NodePtrsWithIDs", no_init)
  .add_property("getExternalNodes",make_function(&XC::NodePtrsWithIDs::getExternalNodes, return_internal_reference<>() ),"Return tags of external nodes.")
  ;

#include "damping/python_interface.tcc"
#include "physical_properties/python_interface.tcc"
#include "fvectors/python_interface.tcc"
#include "gauss_models/python_interface.tcc"
#include "body_forces/python_interface.tcc"
