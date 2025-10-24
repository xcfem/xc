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
class_<XC::PrepHandler, bases<XC::PreprocessorContainer,XC::MovableObject>, boost::noncopyable >("PrepHandler", no_init)
  ;

XC::Node *(XC::NodeHandler::*newNodeFromXYZ)(const double &x,const double &y,const double &z)= &XC::NodeHandler::newNode;
XC::Node *(XC::NodeHandler::*newNodeFromXY)(const double &x,const double &y)= &XC::NodeHandler::newNode;
XC::Node *(XC::NodeHandler::*newNodeFromX)(const double &x)= &XC::NodeHandler::newNode;
XC::Node *(XC::NodeHandler::*newNodeFromVector)(const XC::Vector &)= &XC::NodeHandler::newNode;
XC::Node *(XC::NodeHandler::*duplicateNodeFromNode)(const XC::Node &)= &XC::NodeHandler::duplicateNode;
XC::Node *(XC::NodeHandler::*duplicateNodeFromTag)(const int &)= &XC::NodeHandler::duplicateNode;
class_<XC::NodeHandler, bases<XC::PrepHandler>, boost::noncopyable >("NodeHandler", no_init)
  .add_property("numDOFs", &XC::NodeHandler::getNumDOFs, &XC::NodeHandler::setNumDOFs,"Number of degrees ocf freedom per node.")
  .add_property("dimSpace", &XC::NodeHandler::getSpaceDim, &XC::NodeHandler::setSpaceDim, "Space dimension.")
  .add_property("defaultTag", &XC::NodeHandler::getDefaultTag, &XC::NodeHandler::setDefaultTag,"Starting ID number to apply to the next creation of a node.")
  .def("calculateNodalReactions", &XC::NodeHandler::calculateNodalReactions,"\n""calculateNodalReactions(inclInertia, tolerance)\n""Calculate nodal reaction forces and moments.\n""If inclInertia is True, the unbalance load vector for each node is calculated including inertial forces.\n" "A tolerance of 1e-4 is taken as default value for the checking of the reaction forces and moments. \n")
  .def("getNode", &XC::NodeHandler::getNode,return_internal_reference<>(),"\n""getNode(tag)\n""Return the node whose ID is the value tag passed as parameter.")
  .def("newNodeXYZ", newNodeFromXYZ,return_internal_reference<>(),"\n""newNodeXYZ(x,y,z)\n""Create a node from global coordinates (x,y,z).")
  .def("newNodeFromVector", newNodeFromVector,return_internal_reference<>(),"Create a node at the position pointed by the vector passed as parameter.")
  .def("newNodeIDXYZ", &XC::NodeHandler::newNodeIDXYZ,return_internal_reference<>(),"\n""newNodeIDXYZ(tag,x,y,z)""Create a node whose ID=tag from global coordinates (x,y,z).")
  .def("newNodeXY", newNodeFromXY,return_internal_reference<>(),"\n""newNodeXY(x,y)\n""Create a node from global coordinates (x,y).")
  .def("newNodeIDXY", &XC::NodeHandler::newNodeIDXY,return_internal_reference<>(),"\n""newNodeIDXY(tag,x,y)""Create a node whose ID=tag from global coordinates (x,y).")
  .def("newNodeIDV", &XC::NodeHandler::newNodeIDV,return_internal_reference<>(),"\n""newNodeIDV(tag,vector)""Create a node whose ID=tag from the vector passed as parameter.")
  .def("newNodeX", newNodeFromX,return_internal_reference<>(),"\n""newNodeX(x)\n""Create a node from global coordinate (x).")
  .def("newSeedNode", &XC::NodeHandler::newSeedNode,return_internal_reference<>(),"\n""newSeedNode()\n""Defines the seed node.")
  .def("duplicateNode", duplicateNodeFromNode,return_internal_reference<>(),"\n""duplicateNode(orgNode) \n" "Create a duplicate of the given node.")
  .def("duplicateNode", duplicateNodeFromTag,return_internal_reference<>(),"\n""duplicateNode(orgNodeTag) \n" "Create a duplicate of the node with the given identifier.")
  ;

bool (XC::MaterialHandler::*materialExistsFromName)(const std::string &) const= &XC::MaterialHandler::materialExists;
bool (XC::MaterialHandler::*materialExistsFromTag)(const int &) const= &XC::MaterialHandler::materialExists;
XC::Material &(XC::MaterialHandler::*getMaterialFromName)(const std::string &)= &XC::MaterialHandler::getMaterial;
XC::Material &(XC::MaterialHandler::*getMaterialFromTag)(const int &)= &XC::MaterialHandler::getMaterial;
class_<XC::MaterialHandler, bases<XC::PrepHandler>, boost::noncopyable >("MaterialHandler", no_init)
  .def("newMaterial", &XC::MaterialHandler::newMaterial,return_internal_reference<>(),"Creates a new material.")
  .def("removeMaterial", &XC::MaterialHandler::removeMaterial, "Remove the material with the given name.")
  .def("getMaterial", getMaterialFromName,return_internal_reference<>(),"Returns the material with the given name.")
  .def("getMaterial", getMaterialFromTag,return_internal_reference<>(),"Returns the material with the given tag.")
  .def("materialExists",materialExistsFromName,"True if material is already defined.")
  .def("materialExists",materialExistsFromTag,"True if material is already defined.")
  .def("getName",&XC::MaterialHandler::getName,"Returns the name that corresponds to the identifier.")
  .def("newSectionGeometry", &XC::MaterialHandler::newSectionGeometry,return_internal_reference<>())
  .def("removeSectionGeometry", &XC::MaterialHandler::removeSectionGeometry, "Remove the section geometry with the given name.")
  .def("getSectionGeometry", &XC::MaterialHandler::getSectionGeometry,return_internal_reference<>(),"Returns section geometry whose name is given.")
  .def("geomSectionExists",&XC::MaterialHandler::sectionGeometryExists,"True if section's shape is already defined.")
  .def("interactionDiagExists",&XC::MaterialHandler::InteractionDiagramExists,"True if intecractions diagram is already defined.")
  .def("newInteractionDiagram", &XC::MaterialHandler::newInteractionDiagram,return_internal_reference<>())
  .def("removeInteractionDiagram", &XC::MaterialHandler::removeInteractionDiagram, "Remove the 3D interaction diagram with the given name.")
  .def("calcInteractionDiagram", &XC::MaterialHandler::calcInteractionDiagram,return_internal_reference<>())
  .def("interactionDiag2dExists",&XC::MaterialHandler::InteractionDiagramExists2d,"True if intecractions diagram is already defined.")
  .def("new2DInteractionDiagram", &XC::MaterialHandler::new2DInteractionDiagram,return_internal_reference<>())
  .def("remove2DInteractionDiagram", &XC::MaterialHandler::remove2DInteractionDiagram, "Remove the 2D interaction diagram with the given name.")
  .def("calcInteractionDiagramNMy", &XC::MaterialHandler::calcInteractionDiagramNMy,return_internal_reference<>())
  .def("calcInteractionDiagramNMz", &XC::MaterialHandler::calcInteractionDiagramNMz,return_internal_reference<>())
  .def("getMaterialNames", &XC::MaterialHandler::getMaterialNamesPy, "Return the names of the defined materials.")
  .def("setCreepOn", &XC::MaterialHandler::setCreepOn,"Activate creep.").staticmethod("setCreepOn")
  .def("setCreepOff", &XC::MaterialHandler::setCreepOff,"Deactivate creep.").staticmethod("setCreepOff")
  .def("getCreepDt", &XC::MaterialHandler::getCreepDt,"Get time increment for creep.").staticmethod("getCreepDt")
  .def("setCreepDt", &XC::MaterialHandler::setCreepDt,"Set time increment for creep.").staticmethod("setCreepDt")
   ;

class_<XC::BeamIntegratorHandler, bases<XC::PrepHandler>, boost::noncopyable >("BeamIntegratorHandler", no_init)
  .def("newBeamIntegrator", &XC::BeamIntegratorHandler::newBI,return_internal_reference<>(),"Creates a new integration scheme.")
  .def("get", &XC::BeamIntegratorHandler::get,return_internal_reference<>(),"Returns the integrator.")
  .def("exists",&XC::BeamIntegratorHandler::exists,"True if integrator is already defined.")
  .add_property("size", &XC::BeamIntegratorHandler::size, "Returns container's size (number of defined integration schemes).")
  .def("__len__",&XC::BeamIntegratorHandler::size, "Returns container's size (number of defined integration schemes).")
  .def("clear",&XC::BeamIntegratorHandler::clearAll,"Removes all items.")
 ;

class_<XC::ProtoElementHandler, bases<XC::PrepHandler>, boost::noncopyable >("ProtoElementHandler", no_init)
 .add_property("dimElem", &XC::ProtoElementHandler::getDimElem, &XC::ProtoElementHandler::setDimElem, "Set the default dimension for the elements to be created: 0, 1, 2 or 3 for 0D, 1D, 2D or 3D, respectively.")
  .add_property("numSections", &XC::ProtoElementHandler::getNumSections, &XC::ProtoElementHandler::setNumSections, "Set the default number of sections for the elements to be created")
  .add_property("defaultMaterial", make_function( &XC::ProtoElementHandler::getDefaultMaterial, return_value_policy<copy_const_reference>() ), &XC::ProtoElementHandler::setDefaultMaterial,"Set the default material (called by its name) for the elements to be created")
  .add_property("defaultTransformation", make_function( &XC::ProtoElementHandler::getDefaultTransf, return_value_policy<copy_const_reference>() ), &XC::ProtoElementHandler::setDefaultTransf,"Set the default coordinate transformation (called by its name) for the elements to be created")
  .add_property("defaultIntegrator", make_function( &XC::ProtoElementHandler::getDefaultIntegrator, return_value_policy<copy_const_reference>() ), &XC::ProtoElementHandler::setDefaultIntegrator,"Set the default integrator (called by its name) for the elements to be created")
  .def("newElement", &XC::ProtoElementHandler::newElement,return_internal_reference<>(),"\n newElement(type,iNodes): Create a new element of type 'type' from the nodes passed as parameter with the XC.ID object 'iNodes'. \n" "Parameters:\n""-type: type of element. Available types:'truss','truss_section','corot_truss','corot_truss_section','muelle', 'spring', 'beam2d_02', 'beam2d_03',  'beam2d_04', 'beam3d_01', 'beam3d_02', 'elastic_beam2d', 'elastic_beam3d', 'beam_with_hinges_2d', 'beam_with_hinges_3d', 'nl_beam_column_2d', 'nl_beam_column_3d','force_beam_column_2d', 'force_beam_column_3d', 'shell_mitc4', ' shell_nl', 'quad4n', 'tri31', 'brick', 'zero_length', 'zero_length_contact_2d', 'zero_length_contact_3d', 'zero_length_section'. \n""-iNodes: nodes ID, e.g. xc.ID([1,2]) to create a linear element from node 1 to node 2. \n")
   ;


XC::Element *(XC::ElementHandler::SeedElemHandler::*newElementTags)(const std::string &, const XC::ID &)= &XC::ElementHandler::SeedElemHandler::newElement;
XC::Element *(XC::ElementHandler::SeedElemHandler::*newElementNoTags)(const std::string &)= &XC::ElementHandler::SeedElemHandler::newElement;
class_<XC::ElementHandler::SeedElemHandler, bases<XC::ProtoElementHandler>, boost::noncopyable >("SeedElementHandler", no_init)
  .def("newElement", newElementNoTags,return_internal_reference<>(),"\n newElement(type,iNodes): Create a new element of type 'type' from the nodes passed as parameter with the XC.ID object 'iNodes'. \n" "Parameters:\n""-type: type of element. Available types:'truss','truss_section','corot_truss','corot_truss_section','muelle', 'spring', 'beam2d_02', 'beam2d_03',  'beam2d_04', 'beam3d_01', 'beam3d_02', 'elastic_beam2d', 'elastic_beam3d', 'beam_with_hinges_2d', 'beam_with_hinges_3d', 'nl_beam_column_2d', 'nl_beam_column_3d','force_beam_column_2d', 'force_beam_column_3d', 'shell_mitc4', ' shell_nl', 'quad4n', 'tri31', 'brick', 'zero_length', 'zero_length_contact_2d', 'zero_length_contact_3d', 'zero_length_section'. \n")
  .def("newElement", newElementTags,return_internal_reference<>(),"\n DEPRECATED. Remove node tags parameter.")
   ;


class_<XC::ElementHandler, bases<XC::ProtoElementHandler>, boost::noncopyable >("ElementHandler", no_init)
  .add_property("seedElemHandler", make_function( &XC::ElementHandler::getSeedElemHandler, return_internal_reference<>() ))
  .def("getElement", &XC::ElementHandler::getElement,return_internal_reference<>(),"Returns the element identified by the parameter.")
  .add_property("defaultTag", &XC::ElementHandler::getDefaultTag, &XC::ElementHandler::setDefaultTag,"Starting ID number to apply to the next creation of an element.")
   ;

XC::Skew_Constraint *(XC::BoundaryCondHandler::*newSymmetryConstraintLine)(const int &, const Line2d &)= &XC::BoundaryCondHandler::newSymmetryConstraint;
XC::Skew_Constraint *(XC::BoundaryCondHandler::*newSymmetryConstraintPlane)(const int &, const Plane &)= &XC::BoundaryCondHandler::newSymmetryConstraint;
XC::Skew_Constraint *(XC::BoundaryCondHandler::*newAntiSymmetryConstraintLine)(const int &, const Line2d &)= &XC::BoundaryCondHandler::newAntiSymmetryConstraint;
XC::Skew_Constraint *(XC::BoundaryCondHandler::*newAntiSymmetryConstraintPlane)(const int &, const Plane &)= &XC::BoundaryCondHandler::newAntiSymmetryConstraint;
class_<XC::BoundaryCondHandler, bases<XC::PrepHandler>, boost::noncopyable >("BoundaryCondHandler", no_init)
  .add_property("getTagNextSPConstraint",make_function(&XC::BoundaryCondHandler::getTagNextSPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getTagNextMPConstraint",make_function(&XC::BoundaryCondHandler::getTagNextMPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getNumSPs",&XC::BoundaryCondHandler::getNumSPs,"Number of single node constraints.")
  .add_property("getNumMPs",&XC::BoundaryCondHandler::getNumMPs,"Number of multiple node constraints.")
  .add_property("getNumLPs",&XC::BoundaryCondHandler::getNumMPs,"Number of load patterns.")
  .def("newSPConstraint", &XC::BoundaryCondHandler::newSPConstraint,return_internal_reference<>(),"\n" "newSPConstraint(nodeTag,dofId,value) \n" "Create a single-point boundary constraint by assigning a value to the degree of freedom dofId of node whose ID is nodeTag.")
  .def("removeSPConstraint", &XC::BoundaryCondHandler::removeSPConstraint,return_internal_reference<>(),"Deletes a single freedom constraint.")
  .def("newSkewConstraint", &XC::BoundaryCondHandler::newSkewConstraint,return_internal_reference<>(),"Creates as skew constraint.")
  .def("newSymmetryConstraint", newSymmetryConstraintLine,return_internal_reference<>(),"Creates a new symmetry constraint.")
  .def("newSymmetryConstraint", newSymmetryConstraintPlane,return_internal_reference<>(),"Creates a new symmetry constraint.")
  .def("newAntiSymmetryConstraint", newAntiSymmetryConstraintLine,return_internal_reference<>(),"Creates a new anti-symmetry constraint.")
  .def("newAntiSymmetryConstraint", newAntiSymmetryConstraintPlane,return_internal_reference<>(),"Creates a new anti-symmetry constraint.")
   .def("newMPConstraint", &XC::BoundaryCondHandler::newMPConstraint,return_internal_reference<>(),"New multi-freedom constraint.")
  .def("newEqualDOF", &XC::BoundaryCondHandler::newEqualDOF,return_internal_reference<>(),"Imposes the same displacements on two nodes for the given components.")
  .def("newRigidBeam", &XC::BoundaryCondHandler::newRigidBeam,return_internal_reference<>())
  .def("newRigidRod", &XC::BoundaryCondHandler::newRigidRod,return_internal_reference<>())
  .def("newMRMPConstraint", &XC::BoundaryCondHandler::newMRMPConstraint,return_internal_reference<>(),"Creates a new multi retained nodes constraint.")
  .def("newGlueNodeToElement", &XC::BoundaryCondHandler::newGlueNodeToElement,return_internal_reference<>(),"Glues a node to an element.")
   ;

XC::MapLoadPatterns &(XC::LoadHandler::*getLoadPatternsRef)(void)= &XC::LoadHandler::getLoadPatterns;
XC::LoadCombinationGroup &(XC::LoadHandler::*getLoadCombinationsRef)(void)= &XC::LoadHandler::getLoadCombinations;
boost::python::list (XC::LoadHandler::*getLoadPatternsActionOnNode)(const XC::Node *)= &XC::LoadHandler::getLoadPatternsActingOnPy;
boost::python::list (XC::LoadHandler::*getLoadPatternsActionOnElement)(const XC::Element *)= &XC::LoadHandler::getLoadPatternsActingOnPy;
void (XC::LoadHandler::*copyNodalLoads)(const XC::Node *, const XC::Node *)= &XC::LoadHandler::copyLoads;
void (XC::LoadHandler::*copyElementalLoads)(const XC::Element *, const XC::Element *)= &XC::LoadHandler::copyLoads;
class_<XC::LoadHandler, bases<XC::PrepHandler>, boost::noncopyable >("LoadHandler", no_init)
  .add_property("getLoadPatterns", make_function( getLoadPatternsRef, return_internal_reference<>() ), "Return the load pattern container.")
  .add_property("getLoadCombinations", make_function( getLoadCombinationsRef, return_internal_reference<>() ))
  .def("addToDomain", &XC::LoadHandler::addToDomain,return_internal_reference<>(),"Add combination to the domain.")
  .def("removeFromDomain", &XC::LoadHandler::removeFromDomain,return_internal_reference<>(),"Removes the combination from the domain.")
  .def("removeAllFromDomain", &XC::LoadHandler::removeAllFromDomain,return_internal_reference<>(),"Removes all loads cases from the domain.")
  .def("getLoadPatternsActingOn", getLoadPatternsActionOnNode, "Return the load patterns that act on the given node.")
  .def("getLoadPatternsActingOn", getLoadPatternsActionOnElement, "Return the load patterns that act on the given element.")
  .def("copyLoads",copyNodalLoads, "Copy the loads on the first node to the second one.") 
  .def("copyLoads",copyElementalLoads, "Copy the loads on the first element to the second one.") 
    ;

XC::CrdTransf *(XC::TransfCooHandler::*getCoordTransf)(const std::string &)= &XC::TransfCooHandler::find_ptr;
class_<XC::TransfCooHandler, bases<XC::PrepHandler>, boost::noncopyable >("TransfCooHandler", no_init)
  .def("newLinearCrdTransf2d", &XC::TransfCooHandler::newLinearCrdTransf2d,return_internal_reference<>(),"New linear 2d coordinate transformation.")
  .def("newLinearCrdTransf3d", &XC::TransfCooHandler::newLinearCrdTransf3d,return_internal_reference<>(),"New linear 3d coordinate transformation.")
  .def("newPDeltaCrdTransf2d", &XC::TransfCooHandler::newPDeltaCrdTransf2d,return_internal_reference<>(),"New P-Delta 2d coordinate transformation.")
  .def("newPDeltaCrdTransf3d", &XC::TransfCooHandler::newPDeltaCrdTransf3d,return_internal_reference<>(),"New P-Delta 3d coordinate transformation.")
  .def("newCorotCrdTransf2d", &XC::TransfCooHandler::newCorotCrdTransf2d,return_internal_reference<>(),"New corotational 2d coordinate transformation.")
  .def("newCorotCrdTransf3d", &XC::TransfCooHandler::newCorotCrdTransf3d,return_internal_reference<>(),"New corotational 3d coordinate transformation.")
  .def("getName",&XC::TransfCooHandler::getName,"Returns the name that corresponds to the identifier.")
  .def("getCoordTransf", make_function(getCoordTransf, return_internal_reference<>() ),"Return the coordinate transformation from its name.")
  ;
