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

//Elasticity.

material_vector_NDMat &(PhysicalProperties_NDMat::*getNDMatVector)(void) = &PhysicalProperties_NDMat::getMaterialsVector;
void (PhysicalProperties_NDMat::*setMaterialPtr)(const XC::NDMaterial *)= &PhysicalProperties_NDMat::setMaterial;
class_<PhysicalProperties_NDMat,  bases<CommandEntity,XC::MovableObject>, boost::noncopyable >("PhysicalProperties_NDMat", no_init)
  .add_property("getVectorMaterials",make_function(getNDMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
  .add_property("generalizedStrains",&PhysicalProperties_NDMat::getGeneralizedStrains,"Returns a matrix with strain values for each gauss point.")
  .add_property("generalizedStresses",&PhysicalProperties_NDMat::getGeneralizedStresses,"Returns a matrix with stress values for each gauss point.")
  .def("setMaterial",setMaterialPtr,"Set material.")
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
    .def("getMeanGeneralizedStressByName", &XC::NDMaterialPhysicalProperties::getMeanGeneralizedStressByName)
    .def("getMeanGeneralizedStrainByName", &XC::NDMaterialPhysicalProperties::getMeanGeneralizedStrainByName)
   ;

class_<XC::SolidMech2D, bases<XC::NDMaterialPhysicalProperties>, boost::noncopyable  >("SolidMech2D", no_init)
   ;


material_vector_SectionFDMat &(PhysicalProperties_SectionFDMat::*getSectionFDMatVector)(void) = &PhysicalProperties_SectionFDMat::getMaterialsVector;
class_<PhysicalProperties_SectionFDMat,  bases<CommandEntity,XC::MovableObject>, boost::noncopyable >("PhysicalProperties_SectionFDMat", no_init)
  .add_property("getVectorMaterials",make_function(getSectionFDMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
   ;

class_<XC::SectionFDPhysicalProperties, bases<PhysicalProperties_SectionFDMat>, boost::noncopyable  >("SectionFDPhysicalProperties", no_init)
    .def("getMeanInternalForce",&XC::SectionFDPhysicalProperties::getMeanInternalForce)
    .def("getMeanInternalDeformation",&XC::SectionFDPhysicalProperties::getMeanInternalDeformation)
    .def("getMeanGeneralizedStressByName", &XC::SectionFDPhysicalProperties::getMeanGeneralizedStressByName)
    .def("getMeanGeneralizedStrainByName", &XC::SectionFDPhysicalProperties::getMeanGeneralizedStrainByName)
   ;


material_vector_UMat &(PhysicalProperties_UMat::*getUMatVector)(void) = &PhysicalProperties_UMat::getMaterialsVector;
class_<PhysicalProperties_UMat,  bases<CommandEntity,XC::MovableObject>, boost::noncopyable >("PhysicalProperties_UMat", no_init)
  .add_property("getVectorMaterials",make_function(getUMatVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
   ;

class_<XC::UniaxialMatPhysicalProperties, bases<PhysicalProperties_UMat>, boost::noncopyable  >("UniaxialMatPhysicalProperties", no_init)
  ;

class_<XC::Joint2DPhysicalProperties, bases<XC::UniaxialMatPhysicalProperties>, boost::noncopyable  >("Joint2DPhysicalProperties", no_init)
  ;

material_vector_ElasticSection1d &(PhysicalProperties_ElasticSection1d::*getElasticSection1dVector)(void) = &PhysicalProperties_ElasticSection1d::getMaterialsVector;
class_<PhysicalProperties_ElasticSection1d, bases<CommandEntity,XC::MovableObject>, boost::noncopyable  >("PhysicalProperties_ElasticSection1d", no_init)
  .add_property("getVectorMaterials",make_function(getElasticSection1dVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
  ;

class_<ElasticSectionPhysicalProperties_ElasticSection1d, bases<PhysicalProperties_ElasticSection1d>, boost::noncopyable  >("ElasticSectionPhysicalProperties_ElasticSection1d", no_init)
  ;

class_<XC::ElasticSection1dPhysicalProperties, bases<ElasticSectionPhysicalProperties_ElasticSection1d>, boost::noncopyable  >("ElasticSection1dPhysicalProperties", no_init)
  ;

material_vector_ElasticSection2d &(PhysicalProperties_ElasticSection2d::*getElasticSection2dVector)(void) = &PhysicalProperties_ElasticSection2d::getMaterialsVector;
class_<PhysicalProperties_ElasticSection2d, bases<CommandEntity,XC::MovableObject>, boost::noncopyable  >("PhysicalProperties_ElasticSection2d", no_init)
  .add_property("getVectorMaterials",make_function(getElasticSection2dVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
  ;

class_<ElasticSectionPhysicalProperties_ElasticSection2d, bases<PhysicalProperties_ElasticSection2d>, boost::noncopyable  >("ElasticSectionPhysicalProperties_ElasticSection2d", no_init)
  ;

class_<XC::ElasticSection2dPhysicalProperties, bases<ElasticSectionPhysicalProperties_ElasticSection2d>, boost::noncopyable  >("ElasticSection2dPhysicalProperties", no_init)
  ;

material_vector_ElasticSection3d &(PhysicalProperties_ElasticSection3d::*getElasticSection3dVector)(void) = &PhysicalProperties_ElasticSection3d::getMaterialsVector;
class_<PhysicalProperties_ElasticSection3d, bases<CommandEntity,XC::MovableObject>, boost::noncopyable  >("PhysicalProperties_ElasticSection3d", no_init)
  .add_property("getVectorMaterials",make_function(getElasticSection3dVector,return_internal_reference<>() ),"Returns materials at Gauss points.")
  ;

class_<ElasticSectionPhysicalProperties_ElasticSection3d, bases<PhysicalProperties_ElasticSection3d>, boost::noncopyable  >("ElasticSectionPhysicalProperties_ElasticSection3d", no_init)
  ;

class_<XC::ElasticSection3dPhysicalProperties, bases<ElasticSectionPhysicalProperties_ElasticSection3d>, boost::noncopyable  >("ElasticSection3dPhysicalProperties", no_init)
  ;

// class_<XC::Joint3DPhysicalProperties, bases<XC::UniaxialMatPhysicalProperties>, boost::noncopyable  >("Joint3DPhysicalProperties", no_init)
//    ;

