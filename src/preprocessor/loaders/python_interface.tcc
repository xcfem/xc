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
class_<XC::Loader, bases<EntCmd,XC::MovableObject>, boost::noncopyable >("Loader", no_init)
   ;

XC::Node *(XC::NodeLoader::*nuevoNodoFromXYZ)(const double &x,const double &y,const double &z)= &XC::NodeLoader::nuevoNodo;
XC::Node *(XC::NodeLoader::*nuevoNodoFromXY)(const double &x,const double &y)= &XC::NodeLoader::nuevoNodo;
XC::Node *(XC::NodeLoader::*nuevoNodoFromVector)(const XC::Vector &)= &XC::NodeLoader::nuevoNodo;
class_<XC::NodeLoader, bases<XC::Loader>, boost::noncopyable >("NodeLoader", no_init)
  .add_property("numGdls", &XC::NodeLoader::getNumGdls, &XC::NodeLoader::setNumGdls)
  .add_property("dimEspace", &XC::NodeLoader::getDimEspacio, &XC::NodeLoader::setDimEspacio)
  .add_property("defaultTag", &XC::NodeLoader::getDefaultTag, &XC::NodeLoader::setDefaultTag)
  .def("calculateNodalReactions", &XC::NodeLoader::calculateNodalReactions,"Calcula las reacciones en los nodos.")
  .def("getNode", &XC::NodeLoader::getNode,return_internal_reference<>(),"Devuelve el nodo cuyo identificador se pasa como parámetro.")
  .def("__getitem__",&XC::NodeLoader::getNode, return_value_policy<reference_existing_object>())
  .def("newNodeXYZ", nuevoNodoFromXYZ,return_internal_reference<>(),"Crea un nodo con las coordenadas que se pasan como parámetro.")
  .def("newNodeFromVector", nuevoNodoFromVector,return_internal_reference<>(),"Crea un nodo con las coordenadas que se pasan como parámetro.")
  .def("newNodeIDXYZ", &XC::NodeLoader::newNodeIDXYZ,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas que se pasan como parámetro.")
  .def("newNodeXY", nuevoNodoFromXY,return_internal_reference<>(),"Crea un nodo con las coordenadas que se pasan como parámetro.")
  .def("newNodeIDXY", &XC::NodeLoader::newNodeIDXY,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas que se pasan como parámetro.")
  .def("newNodeIDV", &XC::NodeLoader::newNodeIDV,return_internal_reference<>(),"Crea un nodo con el tag y las coordenadas que se pasan como parámetro.")
  .def("newSeedNode", &XC::NodeLoader::newSeedNode,return_internal_reference<>(),"Crea un nodo con las coordenadas que se pasan como parámetro.")
  .def("duplicateNode", &XC::NodeLoader::duplicateNode,return_internal_reference<>(),"Duplicates the hode.")
  ;

class_<XC::MaterialLoader, bases<XC::Loader>, boost::noncopyable >("MaterialLoader", no_init)
  .def("newMaterial", &XC::MaterialLoader::nuevoMaterial,return_internal_reference<>(),"Crea un material.")
  .def("getMaterial", &XC::MaterialLoader::getMaterial,return_internal_reference<>(),"Devuelve el material cuyo nombre se pasa como parámetro.")
  .def("__getitem__",&XC::MaterialLoader::getMaterial, return_value_policy<reference_existing_object>())
  .def("materialExists",&XC::MaterialLoader::existeMaterial,"True if material is already defined.")
  .def("newSectionGeometry", &XC::MaterialLoader::newSectionGeometry,return_internal_reference<>())
  .def("getSectionGeometry", &XC::MaterialLoader::getGeomSection,return_internal_reference<>(),"Returns section geometry whose name is given.")
  .def("geomSectionExists",&XC::MaterialLoader::existeGeomSection,"True if section's shape is already defined.")
  .def("interactionDiagExists",&XC::MaterialLoader::existeDiagInteraccion,"True if intecractions diagram is already defined.")
  .def("newInteractionDiagram", &XC::MaterialLoader::newInteractionDiagram,return_internal_reference<>())
  .def("calcInteractionDiagram", &XC::MaterialLoader::calcInteractionDiagram,return_internal_reference<>())
  .def("interactionDiag2dExists",&XC::MaterialLoader::existeDiagInteraccion2d,"True if intecractions diagram is already defined.")
  .def("new2DInteractionDiagram", &XC::MaterialLoader::new2DInteractionDiagram,return_internal_reference<>())
  .def("calcInteractionDiagramNMy", &XC::MaterialLoader::calcInteractionDiagramNMy,return_internal_reference<>())
  .def("calcInteractionDiagramNMz", &XC::MaterialLoader::calcInteractionDiagramNMz,return_internal_reference<>())
   ;

class_<XC::BeamIntegratorLoader, bases<XC::Loader>, boost::noncopyable >("BeamIntegratorLoader", no_init)
  .def("newBeamIntegrator", &XC::BeamIntegratorLoader::newBI,return_internal_reference<>(),"Creates a new integration scheme.")
  .def("get", &XC::BeamIntegratorLoader::get,return_internal_reference<>(),"Retunrs the integrator.")
  .def("__getitem__",&XC::BeamIntegratorLoader::get, return_value_policy<reference_existing_object>())
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
  .def("newElement", &XC::ProtoElementLoader::nuevoElemento,return_internal_reference<>(),"Crea un elemento del tipo que se pasa como parámetro.")
   ;

class_<XC::ElementLoader::SeedElemLoader, bases<XC::ProtoElementLoader>, boost::noncopyable >("SeedElement", no_init)
   ;


class_<XC::ElementLoader, bases<XC::ProtoElementLoader>, boost::noncopyable >("ElementLoader", no_init)
  .add_property("seedElemLoader", make_function( &XC::ElementLoader::getSeedElemLoader, return_internal_reference<>() ))
  .def("getElement", &XC::ElementLoader::getElement,return_internal_reference<>(),"Devuelve el elemento cuyo identificador se pasa como parámetro.")
  .def("__getitem__",&XC::ElementLoader::getElement, return_value_policy<reference_existing_object>())
  .add_property("defaultTag", &XC::ElementLoader::getDefaultTag, &XC::ElementLoader::setDefaultTag)
   ;

class_<XC::ConstraintLoader, bases<XC::Loader>, boost::noncopyable >("ConstraintLoader", no_init)
  .add_property("getTagNextSPConstraint",make_function(&XC::ConstraintLoader::getTagNextSPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getTagNextMPConstraint",make_function(&XC::ConstraintLoader::getTagNextMPConstraint,return_value_policy<copy_const_reference>()))
  .add_property("getNumSPs",&XC::ConstraintLoader::getNumSPs,"Number of single node constraints.")
  .add_property("getNumMPs",&XC::ConstraintLoader::getNumMPs,"Number of multiple node constraints.")
  .add_property("getNumLPs",&XC::ConstraintLoader::getNumMPs,"Number of load patterns.")
  .def("newSPConstraint", &XC::ConstraintLoader::newSPConstraint,return_internal_reference<>(),"Crea una nueva condición de contorno monopunto.")
  .def("removeSPConstraint", &XC::ConstraintLoader::removeSPConstraint,return_internal_reference<>(),"Crea una nueva condición de contorno monopunto.")
  .def("newMPConstraint", &XC::ConstraintLoader::newMPConstraint,return_internal_reference<>(),"Crea una nueva condición de contorno multipunto.")
  .def("newEqualDOF", &XC::ConstraintLoader::newEqualDOF,return_internal_reference<>())
  .def("newRigidBeam", &XC::ConstraintLoader::newRigidBeam,return_internal_reference<>())
  .def("newRigidRod", &XC::ConstraintLoader::newRigidRod,return_internal_reference<>())
  .def("newMRMPConstraint", &XC::ConstraintLoader::newMRMPConstraint,return_internal_reference<>(),"Creates a new multi retained nodes constraint.")
  .def("newGlueNodeToElement", &XC::ConstraintLoader::newGlueNodeToElement,return_internal_reference<>(),"Glues a node to an element.")
   ;

XC::MapLoadPatterns &(XC::LoadLoader::*getLoadPatternsRef)(void)= &XC::LoadLoader::getLoadPatterns;
XC::GrupoCombinaciones &(XC::LoadLoader::*getCombinacionesRef)(void)= &XC::LoadLoader::getCombinaciones;
class_<XC::LoadLoader, bases<XC::Loader>, boost::noncopyable >("LoadLoader", no_init)
  .add_property("getLoadPatterns", make_function( getLoadPatternsRef, return_internal_reference<>() ))
  .add_property("getLoadCombinations", make_function( getCombinacionesRef, return_internal_reference<>() ))
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
  ;
