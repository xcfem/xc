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

class_<XC::Constraint, bases<XC::ContinuaReprComponent>, boost::noncopyable >("Constraint", no_init)
  .add_property("nodeTag",&XC::Constraint::getNodeTag,&XC::Constraint::setNodeTag,"sets/gets node tag.")
  .add_property("getNode",make_function(&XC::Constraint::getNode, return_internal_reference<>()),"gets node.")
  .add_property("getNodeIdx",&XC::Constraint::getNodeIdx,"Returns node idxmfor Vtk graphics.")
  .add_property("getVtkCellType",&XC::Constraint::getVtkCellType,"Returns cell type for Vtk graphics.")
  ;

class_<XC::SFreedom_Constraint, XC::SFreedom_Constraint *, bases<XC::Constraint>, boost::noncopyable >("SFreedom_Constraint", no_init)
  .add_property("getDOFNumber", &XC::SFreedom_Constraint::getDOF_Number,"return DOF's number.")
  .add_property("getValue", &XC::SFreedom_Constraint::getValue,"returns imposed value for DOF.")
  .add_property("isHomogeneous", &XC::SFreedom_Constraint::isHomogeneous,"true if it's an homogeneous boundary condition.")
  .add_property("loadPatternTag", &XC::SFreedom_Constraint::getLoadPatternTag,&XC::SFreedom_Constraint::setLoadPatternTag,"assigns/retrieves load pattern tag.")  .add_property("constant", &XC::SFreedom_Constraint::getIsConstant, &XC::SFreedom_Constraint::setIsConstant,"determines if the constraint is constant in time or not.")
 
  .add_property("getVtkCellType", &XC::SFreedom_Constraint::getVtkCellType, "returns VTK cell type")
  ;

class_<XC::SFreedom_ConstraintIter, boost::noncopyable >("SFreedom_ConstraintIter", no_init)
  .def("next", &XC::SFreedom_ConstraintIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;

class_<XC::ImposedMotionBase , bases<XC::SFreedom_Constraint>, boost::noncopyable >("ImposedMotionBase", no_init);

class_<XC::ImposedMotionSP , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP", no_init);

class_<XC::ImposedMotionSP1 , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP1", no_init);

class_<XC::MFreedom_ConstraintBase, bases<XC::Constraint>, boost::noncopyable >("MFreedom_ConstraintBase", no_init)
  .add_property("getIdxNodes",&XC::MFreedom_ConstraintBase::getIdxNodes,"Return the node indices for its use in VTK arrays.")
  ;


class_<XC::MFreedom_Constraint, bases<XC::MFreedom_ConstraintBase>, boost::noncopyable >("MFreedom_Constraint", no_init);

class_<XC::MFreedom_ConstraintIter, boost::noncopyable >("MFreedom_ConstraintIter", no_init)
  .def("next", &XC::MFreedom_ConstraintIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;

class_<XC::EqualDOF, bases<XC::MFreedom_Constraint>, boost::noncopyable >("EqualDOF", no_init);

class_<XC::MFreedom_Joint2D , bases<XC::MFreedom_Constraint>, boost::noncopyable >("MFreedom_Joint2D", no_init);

class_<XC::MFreedom_Joint3D, bases<XC::MFreedom_Constraint>, boost::noncopyable >("MFreedom_Joint3D", no_init);

class_<XC::RigidDiaphragm, bases<XC::MFreedom_Constraint>, boost::noncopyable >("RigidDiaphragm", no_init);

class_<XC::RigidBase, bases<XC::MFreedom_Constraint>, boost::noncopyable >("RigidBase", no_init);

class_<XC::RigidBeam, bases<XC::RigidBase>, boost::noncopyable >("RigidBeam", no_init);

class_<XC::RigidRod, bases<XC::RigidBase>, boost::noncopyable >("RigidRod", no_init);

class_<XC::MRMFreedom_Constraint, bases<XC::MFreedom_ConstraintBase>, boost::noncopyable >("MRMFreedom_Constraint", no_init);

class_<XC::MRMFreedom_ConstraintIter, boost::noncopyable >("MRMFreedom_ConstraintIter", no_init)
  .def("next", &XC::MRMFreedom_ConstraintIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;


class_<XC::GlueNodeToElement, bases<XC::MRMFreedom_Constraint>, boost::noncopyable >("GlueNodeToElement", no_init);

//class_<XC::MapActiveLoadPatterns, bases<CommandEntity>, boost::noncopyable >("MapActiveLoadPatterns", no_init);

typedef std::map<int,XC::LoadPattern *> map_load_patterns;
class_<map_load_patterns, boost::noncopyable>("map_load_patterns")
  .def(XC::mapptr_indexing_suite<map_load_patterns>() )
  ;

typedef std::map<int,XC::NodeLocker *> map_node_locker;
class_<map_node_locker, boost::noncopyable>("map_node_locker")
  .def(XC::mapptr_indexing_suite<map_node_locker>() )
  ;

map_load_patterns &(XC::ConstrContainer::*getLoadPatternsPtr)(void)= &XC::ConstrContainer::getLoadPatterns;
map_node_locker &(XC::ConstrContainer::*getNodeLockersPtr)(void)= &XC::ConstrContainer::getNodeLockers;
class_<XC::ConstrContainer, bases<XC::MeshComponentContainer>, boost::noncopyable >("ConstrContainer", no_init)
  .def("getNumSPs", &XC::ConstrContainer::getNumSPs,"Returns the number of single point constraints.")
  .def("getNumMPs", &XC::ConstrContainer::getNumMPs,"Returns the number of multi-point constraints.")
  .def("getNumMRMPs", &XC::ConstrContainer::getNumMRMPs,"Returns the number of multi-row multi-point constraints.")
  .add_property("getSPs", make_function( &XC::ConstrContainer::getSPs, return_internal_reference<>() ),"Returns the single point constraints container.")
  .add_property("getMPs", make_function( &XC::ConstrContainer::getMPs, return_internal_reference<>() ),"Returns the multi-point constraints container.")
  .add_property("getMRMPs", make_function( &XC::ConstrContainer::getMRMPs, return_internal_reference<>() ),"Returns the multi-point constraints container.")
  .def("getNumLoadPatterns", &XC::ConstrContainer::getNumLoadPatterns,"Returns the number of load cases.")
  .def("getNumNodeLockers", &XC::ConstrContainer::getNumNodeLockers,"Returns the number of node lockers.")
  .def("getNodeLockers", make_function(getNodeLockersPtr, return_internal_reference<>() ),"Returns the node lockers container.")
  .add_property("getLoadPatterns", make_function(getLoadPatternsPtr, return_internal_reference<>() ),"Returns the load pattern container.")
  ;
