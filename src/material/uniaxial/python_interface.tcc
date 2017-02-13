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

class_<XC::UniaxialMaterial,XC::UniaxialMaterial *, bases<XC::Material>, boost::noncopyable >("UniaxialMaterial", no_init)
  .add_property("rho", &XC::UniaxialMaterial::getRho, &XC::UniaxialMaterial::setRho,"Material density.")
  .add_property("initialStrain", &XC::UniaxialMaterial::getInitialStrain, &XC::UniaxialMaterial::setInitialStrain)
  .def("setTrialStrain", &XC::UniaxialMaterial::setTrialStrain,"Assigns trial strain.")
  .def("getStrain", &XC::UniaxialMaterial::getStrain,"Return the deformación en el material.")
  .def("getStress", &XC::UniaxialMaterial::getStress,"Return the tensión en el material.")
  .def("getGeneralizedStress",make_function(&XC::UniaxialMaterial::getGeneralizedStress,return_internal_reference<>()))
  .def("getGeneralizedStrain",make_function(&XC::UniaxialMaterial::getGeneralizedStrain,return_internal_reference<>()))
  .def("setInitialGeneralizedStrain",&XC::UniaxialMaterial::setInitialGeneralizedStrain)
  .def("getInitialGeneralizedStrain",make_function(&XC::UniaxialMaterial::getInitialGeneralizedStrain,return_internal_reference<>()))
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

class_<XC::ENTMaterial , bases<XC::ElasticBaseMaterial>, boost::noncopyable >("ENTMaterial", no_init);

class_<XC::EPPBaseMaterial , bases<XC::ElasticBaseMaterial>, boost::noncopyable >("EPPBaseMaterial", no_init);

class_<XC::EPPGapMaterial , bases<XC::EPPBaseMaterial>, boost::noncopyable >("EPPGapMaterial", no_init);

class_<XC::ElasticPPMaterial , bases<XC::EPPBaseMaterial>, boost::noncopyable >("ElasticPPMaterial", no_init)
  .add_property("fyp", &XC::ElasticPPMaterial::get_fyp, &XC::ElasticPPMaterial::set_fyp,"Positive yield stress.")
  .add_property("eyp", &XC::ElasticPPMaterial::get_eyp,"Positive yield strain.")
  .add_property("fyn", &XC::ElasticPPMaterial::get_fyn, &XC::ElasticPPMaterial::set_fyn,"Negative yield stress.")
  .add_property("eyn", &XC::ElasticPPMaterial::get_eyn,"Negative yield strain.")
  ;


class_<XC::CableMaterial, bases<XC::ElasticBaseMaterial> >("CableMaterial")
    .add_property("prestress", &XC::CableMaterial::getPrestress, &XC::CableMaterial::setPrestress,"Cable's prestress.")
    .add_property("length", &XC::CableMaterial::getLength, &XC::CableMaterial::setLength)
       ;

class_<XC::EncapsulatedMaterial, bases<XC::UniaxialMaterial>, boost::noncopyable >("EncapsulatedMaterial", no_init);

class_<XC::MinMaxMaterial, bases<XC::EncapsulatedMaterial>, boost::noncopyable >("MinMaxMaterial", no_init);

class_<XC::PathIndependentMaterial, bases<XC::EncapsulatedMaterial>, boost::noncopyable >("PathIndependentMaterial", no_init);

//class_<XC::BarSlipMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("BarSlipMaterial", no_init);

//class_<XC::BoucWenMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("BoucWenMaterial", no_init);


class_<XC::DrainMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("DrainMaterial", no_init);

//class_<XC::FatigueMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("FatigueMaterial", no_init);

class_<XC::FedeasMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("FedeasMaterial", no_init);

class_<XC::HardeningMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("HardeningMaterial", no_init);

class_<XC::HystereticMaterial, bases<XC::UniaxialMaterial>, boost::noncopyable >("HystereticMaterial", no_init);


//class_<XC::Pinching4Material , bases<XC::UniaxialMaterial>, boost::noncopyable >("Pinching4Material", no_init);

//class_<XC::ReinforcingSteel , bases<XC::UniaxialMaterial>, boost::noncopyable >("ReinforcingSteel", no_init);



class_<XC::ViscousMaterial , bases<XC::UniaxialMaterial>, boost::noncopyable >("ViscousMaterial", no_init);

typedef std::deque<XC::UniaxialMaterial *> dq_ptr_uniaxial_material;
class_<dq_ptr_uniaxial_material, boost::noncopyable>("dq_ptr_uniaxial_material")
  .def(vector_indexing_suite<dq_ptr_uniaxial_material>() )
  ;

class_<XC::DqUniaxialMaterial, bases<EntCmd,dq_ptr_uniaxial_material>, boost::noncopyable >("DqUniaxialMaterial", no_init);

class_<XC::ZeroLengthMaterials, bases<XC::DqUniaxialMaterial>, boost::noncopyable >("ZeroLengthMaterials", no_init);

class_<XC::UniaxialHistoryVars, bases<XC::MovableObject>, boost::noncopyable >("UniaxialHistoryVars", no_init);

class_<XC::UniaxialStateVars, bases<XC::MovableObject>, boost::noncopyable >("UniaxialStateVars", no_init);



#include "concrete/python_interface.tcc"
#include "connected/python_interface.tcc"
#include "drain/python_interface.tcc"
#include "fedeas/python_interface.tcc"
#include "PY/python_interface.tcc"
#include "snap/python_interface.tcc"
#include "steel/python_interface.tcc"
