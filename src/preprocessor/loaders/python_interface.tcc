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
class_<XC::Loader, bases<EntCmd,XC::MovableObject>, boost::noncopyable >("Loader", no_init)
  .add_property("getDomain", make_function( &XC::Loader::getDomain, return_internal_reference<>() ),"Returns a reference to the domain.")
    ;

XC::Node *(XC::NodeLoader::*newNodeFromXYZ)(const double &x,const double &y,const double &z)= &XC::NodeLoader::newNode;
XC::Node *(XC::NodeLoader::*newNodeFromXY)(const double &x,const double &y)= &XC::NodeLoader::newNode;
XC::Node *(XC::NodeLoader::*newNodeFromVector)(const XC::Vector &)= &XC::NodeLoader::newNode;
class_<XC::NodeLoader, bases<XC::Loader>, boost::noncopyable >("NodeLoader", no_init)
  .add_property("numGdls", &XC::NodeLoader::getNumGdls, &XC::NodeLoader::setNumGdls)
  .add_property("dimEspace", &XC::NodeLoader::getDimEspacio, &XC::NodeLoader::setDimEspacio)
  .add_property("defaultTag", &XC::NodeLoader::getDefaultTag, &XC::NodeLoader::setDefaultTag)
  .def("calculateNodalReactions", &XC::NodeLoader::calculateNodalReactions,"Calcula las reacciones en los nodos.")
  .def("getNode", &XC::NodeLoader::getNode,return_internal_reference<>(),"Returns the nodo cuyo identificador being passed as parameter.")
  .def("newNodeXYZ", newNodeFromXYZ,return_internal_reference<>(),"Crea un nodo con las coordenadas being passed as parameters.")
  .def("newNodeFromVector", newNodeFromVector,return_internal_reference<>(),"Crea un nodo con las coordenadas being passed as parameters.")
  .def("newNodeIDXYZ", &XC::NodeLoader::newNodeIDXYZ,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas being passed as parameters.")
  .def("newNodeXY", newNodeFromXY,return_internal_reference<>(),"Crea un nodo con las coordenadas being passed as parameters.")
  .def("newNodeIDXY", &XC::NodeLoader::newNodeIDXY,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas being passed as parameters.")
  .def("newNodeIDV", &XC::NodeLoader::newNodeIDV,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas being passed as parameters.")
  .def("newSeedNode", &XC::NodeLoader::newSeedNode,return_internal_reference<>(),"Crea un nodo con las coordenadas being passed as parameters.")
  .def("duplicateNode", &XC::NodeLoader::duplicateNode,return_internal_reference<>(),"Duplicates the hode.")
  ;

class_<XC::MaterialLoader, bases<XC::Loader>, boost::noncopyable >("MaterialLoader", no_init)
  .def("newMaterial", &XC::MaterialLoader::newMaterial,return_internal_reference<>(),"Creates a new material.")
  .def("getMaterial", &XC::MaterialLoader::getMaterial,return_internal_reference<>(),"Returns the material cuyo nombre being passed as parameter.")
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
  .add_property("dimElem", &XC::ProtoElementLoader::getDimElem, &XC::ProtoElementLoader::setDimElem)
  .add_property("numSections", &XC::ProtoElementLoader::getNumSections, &XC::ProtoElementLoader::setNumSections)
  .add_property("defaultMaterial", make_function( &XC::ProtoElementLoader::getDefaultMaterial, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultMaterial)
  .add_property("defaultTransformation", make_function( &XC::ProtoElementLoader::getDefaultTransf, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultTransf)
  .add_property("defaultIntegrator", make_function( &XC::ProtoElementLoader::getDefaultIntegrator, return_value_policy<copy_const_reference>() ), &XC::ProtoElementLoader::setDefaultIntegrator)
  .def("newElement", &XC::ProtoElementLoader::newElement,return_internal_reference<>(),"Creates an element of the type being passed as parameter.")
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
  .def("newSPConstraint", &XC::ConstraintLoader::newSPConstraint,return_internal_reference<>(),"New single freedom constraint.")
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

class_<XC::TransfCooLoader, bases<XC::Loader>, boost::noncopyable >("TransfCooLoader", no_init)
  .def("newLinearCrdTransf2d", &XC::TransfCooLoader::newLinearCrdTransf2d,return_internal_reference<>(),"New linear 2d coordinate transformation.")
  .def("newLinearCrdTransf3d", &XC::TransfCooLoader::newLinearCrdTransf3d,return_internal_reference<>(),"New linear 3d coordinate transformation.")
  .def("newPDeltaCrdTransf2d", &XC::TransfCooLoader::newPDeltaCrdTransf2d,return_internal_reference<>(),"New P-Delta 2d coordinate transformation.")
  .def("newPDeltaCrdTransf3d", &XC::TransfCooLoader::newPDeltaCrdTransf3d,return_internal_reference<>(),"New P-Delta 3d coordinate transformation.")
  .def("newCorotCrdTransf2d", &XC::TransfCooLoader::newCorotCrdTransf2d,return_internal_reference<>(),"New P-Delta 2d coordinate transformation.")
  .def("newCorotCrdTransf3d", &XC::TransfCooLoader::newCorotCrdTransf3d,return_internal_reference<>(),"New P-Delta 3d coordinate transformation.")
  .def("getName",&XC::TransfCooLoader::getName,"Returns the name thats corresponds to the identifier.")
  ;
