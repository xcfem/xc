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

//Elasticity.

material_vector_NDMat &(PhysicalProperties_NDMat::*getNDMatVector)(void) = &PhysicalProperties_NDMat::getMaterialsVector;
class_<PhysicalProperties_NDMat,  bases<XC::MovableObject>, boost::noncopyable >("PhysicalProperties_NDMat", no_init)
  .add_property("getVectorMaterials",make_function(getNDMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
   ;

const XC::Vector &(XC::NDMaterialPhysicalProperties::*getCommittedStrainVector)(const size_t &) const= &XC::NDMaterialPhysicalProperties::getCommittedStrain;
XC::Matrix (XC::NDMaterialPhysicalProperties::*getCommittedStrainMatrix)(void) const= &XC::NDMaterialPhysicalProperties::getCommittedStrain;
const XC::Vector &(XC::NDMaterialPhysicalProperties::*getCommittedStressVector)(const size_t &) const= &XC::NDMaterialPhysicalProperties::getCommittedStress;
XC::Matrix (XC::NDMaterialPhysicalProperties::*getCommittedStressMatrix)(void) const= &XC::NDMaterialPhysicalProperties::getCommittedStress;
XC::Vector (XC::NDMaterialPhysicalProperties::*getCommittedAvgStressVector)(void) const= &XC::NDMaterialPhysicalProperties::getCommittedAvgStress;
XC::Vector (XC::NDMaterialPhysicalProperties::*getCommittedAvgStrainVector)(void) const= &XC::NDMaterialPhysicalProperties::getCommittedAvgStrain;
double (XC::NDMaterialPhysicalProperties::*getCommittedAvgStressDouble)(const size_t &) const= &XC::NDMaterialPhysicalProperties::getCommittedAvgStress;
double (XC::NDMaterialPhysicalProperties::*getCommittedAvgStrainDouble)(const size_t &) const= &XC::NDMaterialPhysicalProperties::getCommittedAvgStrain;
class_<XC::NDMaterialPhysicalProperties, bases<PhysicalProperties_NDMat>, boost::noncopyable  >("NDMaterialPhysicalProperties", no_init)
  .add_property("getCommittedStrains",getCommittedStrainMatrix,"Returns a matrix with strain values for each gauss point.")
  .add_property("getCommittedStresses",getCommittedStressMatrix,"Returns a matrix with stress values for each gauss point.")
  .def("getCommittedStrain",make_function(getCommittedStrainVector,return_internal_reference<>() ),"Returns a vector with strain values for gauss point i.")
  .def("getCommittedStress",make_function(getCommittedStressVector,return_internal_reference<>() ),"Returns a vector with stress values for gauss point i.")
  .add_property("getCommittedAvgStrain",getCommittedAvgStrainVector,"Returns a vector with average strain values for gauss points.")
  .add_property("getCommittedAvgStress",getCommittedAvgStressVector,"Returns a vector with average stress values for gauss points.")
  .def("getCommittedAvgStrainComponent",getCommittedAvgStrainDouble,"Returns i-component of average strain values vector for gauss points.")
  .def("getCommittedAvgStressComponent",getCommittedAvgStressDouble,"Returns i-component  of average stress values vector for gauss points.")
   ;

class_<XC::SolidMech2D, bases<XC::NDMaterialPhysicalProperties>, boost::noncopyable  >("NDMaterialPhysicalProperties", no_init)
   ;


material_vector_SectionFDMat &(PhysicalProperties_SectionFDMat::*getSectionFDMatVector)(void) = &PhysicalProperties_SectionFDMat::getMaterialsVector;
class_<PhysicalProperties_SectionFDMat,  bases<XC::MovableObject>, boost::noncopyable >("PhysicalProperties_SectionFDMat", no_init)
  .add_property("getVectorMaterials",make_function(getSectionFDMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
   ;

class_<XC::SectionFDPhysicalProperties, bases<PhysicalProperties_SectionFDMat>, boost::noncopyable  >("SectionFDPhysicalProperties", no_init)
    .def("getMeanInternalForce",&XC::SectionFDPhysicalProperties::getMeanInternalForce)
    .def("getMeanInternalDeformation",&XC::SectionFDPhysicalProperties::getMeanInternalDeformation)
   ;


material_vector_UMat &(PhysicalProperties_UMat::*getUMatVector)(void) = &PhysicalProperties_UMat::getMaterialsVector;
class_<PhysicalProperties_UMat,  bases<XC::MovableObject>, boost::noncopyable >("PhysicalProperties_UMat", no_init)
  .add_property("getVectorMaterials",make_function(getUMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
   ;

class_<XC::UniaxialMatPhysicalProperties, bases<PhysicalProperties_UMat>, boost::noncopyable  >("UniaxialMatPhysicalProperties", no_init)
  ;

class_<XC::Joint2DPhysicalProperties, bases<XC::UniaxialMatPhysicalProperties>, boost::noncopyable  >("Joint2DPhysicalProperties", no_init)
   ;

// class_<XC::Joint3DPhysicalProperties, bases<XC::UniaxialMatPhysicalProperties>, boost::noncopyable  >("Joint3DPhysicalProperties", no_init)
//    ;

