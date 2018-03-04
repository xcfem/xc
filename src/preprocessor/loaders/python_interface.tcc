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
class_<XC::Loader, bases<XC::PreprocessorContainer,XC::MovableObject>, boost::noncopyable >("Loader", no_init)
  ;

XC::Node *(XC::NodeLoader::*newNodeFromXYZ)(const double &x,const double &y,const double &z)= &XC::NodeLoader::newNode;
XC::Node *(XC::NodeLoader::*newNodeFromXY)(const double &x,const double &y)= &XC::NodeLoader::newNode;
XC::Node *(XC::NodeLoader::*newNodeFromVector)(const XC::Vector &)= &XC::NodeLoader::newNode;
class_<XC::NodeLoader, bases<XC::Loader>, boost::noncopyable >("NodeLoader", no_init)
  .add_property("numDOFs", &XC::NodeLoader::getNumDOFs, &XC::NodeLoader::setNumDOFs,"Number of degrees of freedom per node.")
  .add_property("dimSpace", &XC::NodeLoader::getDimEspacio, &XC::NodeLoader::setDimEspacio, "Espace dimension.")
  .add_property("defaultTag", &XC::NodeLoader::getDefaultTag, &XC::NodeLoader::setDefaultTag,"Starting ID number to apply to the next creation of nodes ")
  .def("calculateNodalReactions", &XC::NodeLoader::calculateNodalReactions,"\n""calculateNodalReactions(inclInertia)\n""Calculate nodal reaction forces and moments.\n""If inclInertia is True, the unbalance load vector for each node is calculated including inertial forces.\n" "A tolerance of 1e-4 is taken as default value for the checking of the reaction forces and moments. \n")
  .def("getNode", &XC::NodeLoader::getNode,return_internal_reference<>(),"\n""getNode(tag)\n""Return the node whose ID is the value tag passed as parameter.")
  .def("newNodeXYZ", newNodeFromXYZ,return_internal_reference<>(),"\n""newNodeXYZ(x,y,z)\n""Create a node from global coordinates (x,y,z).")
  .def("newNodeFromVector", newNodeFromVector,return_internal_reference<>(),"Create a node at the position pointed by the vector passed as parameter.")
  .def("newNodeIDXYZ", &XC::NodeLoader::newNodeIDXYZ,return_internal_reference<>(),"\n""newNodeIDXYZ(tag,x,y,z)""Create a node whose ID=tag from global coordinates (x,y,z).")
  .def("newNodeXY", newNodeFromXY,return_internal_reference<>(),"\n""newNodeXY(x,y)\n""Create a node from global coordinates (x,y).")
  .def("newNodeIDXY", &XC::NodeLoader::newNodeIDXY,return_internal_reference<>(),"\n""newNodeIDXY(tag,x,y)""Create a node whose ID=tag from global coordinates (x,y).")
  .def("newNodeIDV", &XC::NodeLoader::newNodeIDV,return_internal_reference<>(),"\n""newNodeIDV(tag,vector)""Create a node whose ID=tag from the vector passed as parameter.")
  .def("newSeedNode", &XC::NodeLoader::newSeedNode,return_internal_reference<>(),"\n""newSeedNode()\n""Defines the seed node.")
  .def("duplicateNode", &XC::NodeLoader::duplicateNode,return_internal_reference<>(),"\n""duplicateNode(tagNodoOrg) \n" "Create a duplicate copy of node with ID=tagNodoOrg")
  ;

class_<XC::MaterialLoader, bases<XC::Loader>, boost::noncopyable >("MaterialLoader", no_init)
  .def("newMaterial", &XC::MaterialLoader::newMaterial,return_internal_reference<>(),"Creates a new material.")
  .def("getMaterial", &XC::MaterialLoader::getMaterial,return_internal_reference<>(),"Returns the material which name being passed as parameter.")
  .def("materialExists",&XC::MaterialLoader::existeMaterial,"True if material is already defined.")
  .def("getName",&XC::MaterialLoader::getName,"Returns the name thats corresponds to the identifier.")
  .def("newSectionGeometry", &XC::MaterialLoader::newSectionGeometry,return_internal_reference<>())
  .def("getSectionGeometry", &XC::MaterialLoader::getGeomSection,return_internal_reference<>(),"Returns section geometry whose name is given.")
  .def("geomSectionExists",&XC::MaterialLoader::existeGeomSection,"True if section's shape is already defined.")
  .def("interactionDiagExists",&XC::MaterialLoader::InteractionDiagramExists,"True if intecractions diagram is already defined.")
  .def("newInteractionDiagram", &XC::MaterialLoader::newInteractionDiagram,return_internal_reference<>())
  .def("calcInteractionDiagram", &XC::MaterialLoader::calcInteractionDiagram,return_internal_reference<>())
  .def("interactionDiag2dExists",&XC::MaterialLoader::InteractionDiagramExists2d,"True if intecractions diagram is already defined.")
  .def("new2DInteractionDiagram", &XC::MaterialLoader::new2DInteractionDiagram,return_internal_reference<>())
  .def("calcInteractionDiagramNMy", &XC::MaterialLoader::calcInteractionDiagramNMy,return_internal_reference<>())
  .def("calcInteractionDiagramNMz", &XC::MaterialLoader::calcInteractionDiagramNMz,return_internal_reference<>())
   ;

class_<XC::BeamIntegratorLoader, bases<XC::Loader>, boost::noncopyable >("BeamIntegratorLoader", no_init)
  .def("newBeamIntegrator", &XC::BeamIntegratorLoader::newBI,return_internal_reference<>(),"Creates a new integration scheme.")
  .def("get", &XC::BeamIntegratorLoader::get,return_internal_reference<>(),"Retunrs the integrator.")
  .def("exists",&XC::BeamIntegratorLoader::exists,"True if integrator is already defined.")
  .add_property("size", &XC::BeamIntegratorLoader::size, "Returns container's size (number of defined integration schemes).")
  .def("__len__",&XC::BeamIntegratorLoader::size, "Returns container's size (number of defined integration schemes).")
  .def("clear",&XC::BeamIntegratorLoader::clearAll,"Removes all items.")
 ;

class_<XC::ProtoElementLoader, bases<XC::Loader>, boost::noncopyable >("ProtoElementLoader", no_init)
 .add_property("dimElem", &XC::ProtoElementLoader::getDimElem, &XC::ProtoElementLoader::setDimElem, "Set the default dimension for the elements to be created: 0, 1, 2 or 3 for 0D, 1D, 2D or 3D, respectively.")
  .add_property("numSections", &XC::ProtoElementLoader::getNumSections, &XC::ProtoElementLoader::setNumSections, "Set the default number of sections for the elements to be created")
  .add_property("defaultMaterial", make_function( &XC::ProtoElementLoader::getDefaultMaterial, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultMaterial,"Set the default material (called by its name) for the elements to be created")
  .add_property("defaultTransformation", make_function( &XC::ProtoElementLoader::getDefaultTransf, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultTransf,"Set the default coordinate transformation (called by its name) for the elements to be created")
  .add_property("defaultIntegrator", make_function( &XC::ProtoElementLoader::getDefaultIntegrator, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultIntegrator,"Set the default integrator (called by its name) for the elements to be created")
  .def("newElement", &XC::ProtoElementLoader::newElement,return_internal_reference<>(),"\n newElement(tipo,iNodos): Create a new element of type 'tipo' from the nodes passed as parameter with the XC.ID object 'iNodos'. \n" "Parameters:\n""-tipo: type of element. Available types:'truss','truss_section','corot_truss','corot_truss_section','muelle', 'spring', 'beam2d_02', 'beam2d_03',  'beam2d_04', 'beam3d_01', 'beam3d_02', 'elastic_beam2d', 'elastic_beam3d', 'beam_with_hinges_2d', 'beam_with_hinges_3d', 'nl_beam_column_2d', 'nl_beam_column_3d','force_beam_column_2d', 'force_beam_column_3d', 'shell_mitc4', ' shell_nl', 'quad4n', 'tri31', 'brick', 'zero_length', 'zero_length_contact_2d', 'zero_length_contact_3d', 'zero_length_section'. \n""-iNodos: nodes ID, e.g. xc.ID([1,2]) to create a linear element from node 1 to node 2. \n")
   ;

class_<XC::ElementLoader::SeedElemLoader, bases<XC::ProtoElementLoader>, boost::noncopyable >("SeedElement", no_init)
   ;


class_<XC::ElementLoader, bases<XC::ProtoElementLoader>, boost::noncopyable >("ElementLoader", no_init)
  .add_property("seedElemLoader", make_function( &XC::ElementLoader::getSeedElemLoader, return_internal_reference<>() ))
  .def("getElement", &XC::ElementLoader::getElement,return_internal_reference<>(),"Returns the element identified by the parameter.")
  .add_property("defaultTag", &XC::ElementLoader::getDefaultTag, &XC::ElementLoader::setDefaultTag)
   ;

class_<XC::ConstraintLoader, bases<XC::Loader>, boost::noncopyable >("ConstraintLoader", no_init)
  .add_property("getTagNextSPConstraint",make_function(&XC::ConstraintLoader::getTagNextSPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getTagNextMPConstraint",make_function(&XC::ConstraintLoader::getTagNextMPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getNumSPs",&XC::ConstraintLoader::getNumSPs,"Number of single node constraints.")
  .add_property("getNumMPs",&XC::ConstraintLoader::getNumMPs,"Number of multiple node constraints.")
  .add_property("getNumLPs",&XC::ConstraintLoader::getNumMPs,"Number of load patterns.")
  .def("newSPConstraint", &XC::ConstraintLoader::newSPConstraint,return_internal_reference<>(),"\n" "newSPConstraint(tag_nod,id_gdl,value) \n" "Create a single-point boundary constraint by assigning a value to the degree of freedom gdl of node whose ID is tag_nod.")
  .def("removeSPConstraint", &XC::ConstraintLoader::removeSPConstraint,return_internal_reference<>(),"Deletes a single freedom constraint.")
  .def("newMPConstraint", &XC::ConstraintLoader::newMPConstraint,return_internal_reference<>(),"New multi-freedom constraint.")
  .def("newEqualDOF", &XC::ConstraintLoader::newEqualDOF,return_internal_reference<>(),"Imposes the same displacements on two nodes for the components specified in the argument.")
  .def("newRigidBeam", &XC::ConstraintLoader::newRigidBeam,return_internal_reference<>())
  .def("newRigidRod", &XC::ConstraintLoader::newRigidRod,return_internal_reference<>())
  .def("newMRMPConstraint", &XC::ConstraintLoader::newMRMPConstraint,return_internal_reference<>(),"Creates a new multi retained nodes constraint.")
  .def("newGlueNodeToElement", &XC::ConstraintLoader::newGlueNodeToElement,return_internal_reference<>(),"Glues a node to an element.")
   ;

XC::MapLoadPatterns &(XC::LoadLoader::*getLoadPatternsRef)(void)= &XC::LoadLoader::getLoadPatterns;
XC::LoadCombinationGroup &(XC::LoadLoader::*getLoadCombinationsRef)(void)= &XC::LoadLoader::getLoadCombinations;
class_<XC::LoadLoader, bases<XC::Loader>, boost::noncopyable >("LoadLoader", no_init)
  .add_property("getLoadPatterns", make_function( getLoadPatternsRef, return_internal_reference<>() ))
  .add_property("getLoadCombinations", make_function( getLoadCombinationsRef, return_internal_reference<>() ))
  .def("addToDomain", &XC::LoadLoader::addToDomain,return_internal_reference<>(),"Add combination to the domain.")
  .def("removeFromDomain", &XC::LoadLoader::removeFromDomain,return_internal_reference<>(),"Eliminates combination from domain.")
  .def("removeAllFromDomain", &XC::LoadLoader::removeAllFromDomain,return_internal_reference<>(),"Eliminates all loads cases from domain.")
    ;

XC::CrdTransf *(XC::TransfCooLoader::*getCoordTransf)(const std::string &)= &XC::TransfCooLoader::find_ptr;
class_<XC::TransfCooLoader, bases<XC::Loader>, boost::noncopyable >("TransfCooLoader", no_init)
  .def("newLinearCrdTransf2d", &XC::TransfCooLoader::newLinearCrdTransf2d,return_internal_reference<>(),"New linear 2d coordinate transformation.")
  .def("newLinearCrdTransf3d", &XC::TransfCooLoader::newLinearCrdTransf3d,return_internal_reference<>(),"New linear 3d coordinate transformation.")
  .def("newPDeltaCrdTransf2d", &XC::TransfCooLoader::newPDeltaCrdTransf2d,return_internal_reference<>(),"New P-Delta 2d coordinate transformation.")
  .def("newPDeltaCrdTransf3d", &XC::TransfCooLoader::newPDeltaCrdTransf3d,return_internal_reference<>(),"New P-Delta 3d coordinate transformation.")
  .def("newCorotCrdTransf2d", &XC::TransfCooLoader::newCorotCrdTransf2d,return_internal_reference<>(),"New corotational 2d coordinate transformation.")
  .def("newCorotCrdTransf3d", &XC::TransfCooLoader::newCorotCrdTransf3d,return_internal_reference<>(),"New corotational 3d coordinate transformation.")
  .def("getName",&XC::TransfCooLoader::getName,"Returns the name thats corresponds to the identifier.")
  .def("getCoordTransf", make_function(getCoordTransf, return_internal_reference<>() ),"Return the coordinate transformation from its name.")
  ;
