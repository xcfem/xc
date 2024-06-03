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

class_<XC::UniaxialMaterial,XC::UniaxialMaterial *, bases<XC::Material>, boost::noncopyable >("UniaxialMaterial", no_init)
  .add_property("rho", &XC::UniaxialMaterial::getRho, &XC::UniaxialMaterial::setRho,"Material density.")
  .add_property("initialStrain", &XC::UniaxialMaterial::getInitialStrain, &XC::UniaxialMaterial::setInitialStrain,"Value of the initial strain.")
  .def("getInitialStrain", &XC::UniaxialMaterial::getInitialStrain, "Get the initial strain.")
  .def("setInitialStrain", &XC::UniaxialMaterial::setInitialStrain, "Set the initial strain.")
  .def("incrementInitialStrain", &XC::UniaxialMaterial::incrementInitialStrain, "Increments initial strain.")
  .def("zeroInitialStrain", &XC::UniaxialMaterial::zeroInitialStrain, "Zeroes initial strain.")
  .def("setTrialStrain", &XC::UniaxialMaterial::setTrialStrain,"Assigns trial strain.")
  .def("getStrain", &XC::UniaxialMaterial::getStrain,"Returns material strain.")
  .def("getStress", &XC::UniaxialMaterial::getStress,"Returns material stress.")
  .def("getGeneralizedStress",make_function(&XC::UniaxialMaterial::getGeneralizedStress,return_internal_reference<>()), "Returns material generalized stress.")
  .def("getGeneralizedStrain",make_function(&XC::UniaxialMaterial::getGeneralizedStrain,return_internal_reference<>()), "Returns material generalized stress.")
  .def("setInitialGeneralizedStrain",&XC::UniaxialMaterial::setInitialGeneralizedStrain, "Set material generalized initial strain.")
  .def("getInitialGeneralizedStrain",make_function(&XC::UniaxialMaterial::getInitialGeneralizedStrain,return_internal_reference<>()), "Get material generalized initial strain.")
  .def("getTangent",&XC::UniaxialMaterial::getTangent)
  .def("getInitialTangent",&XC::UniaxialMaterial::getInitialTangent)
  .def("getDampTangent",&XC::UniaxialMaterial::getDampTangent)
  .def("getSecant",&XC::UniaxialMaterial::getSecant)
  .def("getFlexibility",&XC::UniaxialMaterial::getFlexibility)
  .def("getInitialFlexibility",&XC::UniaxialMaterial::getInitialFlexibility)
   ;

class_<XC::ElasticBaseMaterial, bases<XC::UniaxialMaterial>, boost::noncopyable >("ElasticBaseMaterial", no_init)
    .add_property("E", &XC::ElasticBaseMaterial::getE, &XC::ElasticBaseMaterial::setE)
  ;

class_<XC::ElasticMaterial, bases<XC::ElasticBaseMaterial> >("ElasticMaterial")
  ;

class_<XC::ENTMaterial , bases<XC::ElasticBaseMaterial>, boost::noncopyable >("ENTMaterial", no_init)
  .add_property("a", &XC::ENTMaterial::getA, &XC::ENTMaterial::setA,"Get/set the a parameter of the material.")
  .add_property("b", &XC::ENTMaterial::getB, &XC::ENTMaterial::setB,"Get/set the a parameter of the material.")
  ;

class_<XC::EPPBaseMaterial , bases<XC::ElasticBaseMaterial>, boost::noncopyable >("EPPBaseMaterial", no_init);

class_<XC::EPPGapMaterial , bases<XC::EPPBaseMaterial>, boost::noncopyable >("EPPGapMaterial", no_init);

class_<XC::ElasticPPMaterialBase , bases<XC::EPPBaseMaterial>, boost::noncopyable >("ElasticPPMaterialBase", no_init)
  .add_property("fyp", &XC::ElasticPPMaterialBase::get_fyp, &XC::ElasticPPMaterialBase::set_fyp,"Positive yield stress.")
  .add_property("eyp", &XC::ElasticPPMaterialBase::get_eyp,"Positive yield strain.")
  .add_property("fyn", &XC::ElasticPPMaterialBase::get_fyn, &XC::ElasticPPMaterialBase::set_fyn,"Negative yield stress.")
  .add_property("eyn", &XC::ElasticPPMaterialBase::get_eyn,"Negative yield strain.")
  ;

class_<XC::ElasticPPMaterial , bases<XC::ElasticPPMaterialBase>, boost::noncopyable >("ElasticPPMaterial", no_init)
  ;

class_<XC::CableMaterial, bases<XC::ElasticBaseMaterial> >("CableMaterial")
  .add_property("prestress", &XC::CableMaterial::getPrestress, &XC::CableMaterial::setPrestress,"Cable's prestress.")
  .add_property("length", &XC::CableMaterial::getLength, &XC::CableMaterial::setLength, "Return the length of the cable.")
  ;

XC::UniaxialMaterial *(XC::EncapsulatedUniaxialMaterial::*getMaterialPtr)(void)= &XC::EncapsulatedUniaxialMaterial::getMaterial;
void (XC::EncapsulatedUniaxialMaterial::*setEncapsulatedUniaxialMaterial)(const std::string &)= &XC::EncapsulatedUniaxialMaterial::setMaterial;
class_<XC::EncapsulatedUniaxialMaterial, bases<XC::UniaxialMaterial>, boost::noncopyable >("EncapsulatedUniaxialMaterial", no_init)
  .add_property("material", make_function(getMaterialPtr,return_internal_reference<>()), "Return the encapsulated material.")
  .def("setMaterial", setEncapsulatedUniaxialMaterial, "Assigns the encapsulated material.")
  ;

class_<XC::HalfDiagramMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("HalfDiagramMaterial", no_init)
  ;
class_<XC::TensionOnlyMaterial, bases<XC::HalfDiagramMaterial>, boost::noncopyable >("TensionOnlyMaterial", no_init)
  ;
class_<XC::CompressionOnlyMaterial, bases<XC::HalfDiagramMaterial>, boost::noncopyable >("CompressionOnlyMaterial", no_init)
  ;

class_<XC::InvertMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("InvertMaterial", no_init)
  ;

class_<XC::InitStrainBaseMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("InitStrainBaseMaterial", no_init)
  ;

class_<XC::InitStrainMaterial, bases<XC::InitStrainBaseMaterial>, boost::noncopyable >("InitStrainMaterial", no_init);

class_<XC::InitStressMaterial, bases<XC::InitStrainBaseMaterial>, boost::noncopyable >("InitStressMaterial", no_init)
  .add_property("initialStress", &XC::InitStressMaterial::getInitialStress, &XC::InitStressMaterial::setInitialStress,"Get/set the initial stress.")
  .def("setInitialStress", &XC::InitStressMaterial::setInitialStress,"Set the initial stress.")
  .def("getInitialStress", &XC::InitStressMaterial::getInitialStress,"Get the initial stress.")
  ;

class_<XC::MinMaxMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("MinMaxMaterial", no_init);


class_<XC::MultiLinear, bases<XC::UniaxialMaterial>, boost::noncopyable >("MultiLinear", no_init)
  .def("setValues", &XC::MultiLinear::setupPyList,"Set values using a list of tuples [(u1, f1), (u2,f2),...].")
  .add_property("numPoints", &XC::MultiLinear::getNumPoints,"Return the number of points used to define the material.")
  .add_property("backboneStrains", &XC::MultiLinear::getBackboneStrainsPy, "Return the stresses that define the material backbone.")
  ;

class_<XC::PathIndependentMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("PathIndependentMaterial", no_init);

//class_<XC::BarSlipMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("BarSlipMaterial", no_init);

//class_<XC::BoucWenMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("BoucWenMaterial", no_init);


class_<XC::DrainMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("DrainMaterial", no_init);

//class_<XC::FatigueMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("FatigueMaterial", no_init);

class_<XC::FedeasMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("FedeasMaterial", no_init);

class_<XC::HardeningMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("HardeningMaterial", no_init);

class_<XC::HystereticMaterial, bases<XC::UniaxialMaterial>, boost::noncopyable >("HystereticMaterial", no_init);


//class_<XC::Pinching4Material , bases<XC::UniaxialMaterial>, boost::noncopyable >("Pinching4Material", no_init);

//class_<XC::ReinforcingSteel , bases<XC::UniaxialMaterial>, boost::noncopyable >("ReinforcingSteel", no_init);



class_<XC::ViscousMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("ViscousMaterial", no_init)
  .add_property("dampingCoeff", &XC::ViscousMaterial::getDampingCoeff, &XC::ViscousMaterial::setDampingCoeff,"damping coefficient.")
  .add_property("alpha", &XC::ViscousMaterial::getAlpha, &XC::ViscousMaterial::setAlpha,"power factor (=1 means linear damping).")
  ;

typedef std::deque<XC::UniaxialMaterial *> dq_ptr_uniaxial_material;
class_<dq_ptr_uniaxial_material, boost::noncopyable>("dq_ptr_uniaxial_material")
  .def(vector_indexing_suite<dq_ptr_uniaxial_material>() )
  ;

class_<XC::DqUniaxialMaterial, bases<CommandEntity,dq_ptr_uniaxial_material>, boost::noncopyable >("DqUniaxialMaterial", no_init);

class_<XC::ZeroLengthMaterials, bases<XC::DqUniaxialMaterial>, boost::noncopyable >("ZeroLengthMaterials", no_init);

class_<XC::UniaxialHistoryVars, bases<XC::MovableObject>, boost::noncopyable >("UniaxialHistoryVars", no_init);

class_<XC::UniaxialStateVars, bases<XC::MovableObject>, boost::noncopyable >("UniaxialStateVars", no_init);



#include "concrete/python_interface.tcc"
#include "connected/python_interface.tcc"
#include "drain/python_interface.tcc"
#include "fedeas/python_interface.tcc"
#include "soil_structure_interaction/python_interface.tcc"
#include "snap/python_interface.tcc"
#include "steel/python_interface.tcc"
