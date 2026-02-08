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
class_<XC::RawConcrete, bases<XC::UniaxialMaterial>, boost::noncopyable >("RawConcrete", no_init)
  .add_property("epsc0", &XC::RawConcrete::getEpsc0,  &XC::RawConcrete::setEpsc0,"strain at compressive strength.")
  .add_property("epscu", &XC::RawConcrete::getEpscu,  &XC::RawConcrete::setEpscu,"strain at crushing strength.")
  .add_property("fpc", &XC::RawConcrete::getFpc,  &XC::RawConcrete::setFpc,"compressive strength.")
  ;

class_<XC::ConcreteBase, bases<XC::RawConcrete>, boost::noncopyable >("ConcreteBase", no_init)
  .def("getTangent", &XC::ConcreteBase::getTangent,"returns tangent stiffness.")
  ;

class_<XC::Concrete01, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete01", no_init)
  .add_property("fpcu", &XC::Concrete01::getFpcu,  &XC::Concrete01::setFpcu,"crushing strength.")
  ;

class_<XC::Concrete02, bases<XC::RawConcrete>, boost::noncopyable >("Concrete02", no_init)
  .add_property("fpcu", &XC::Concrete02::getFpcu,  &XC::Concrete02::setFpcu,"crushing strength.")
  .add_property("ft", &XC::Concrete02::getFt,  &XC::Concrete02::setFt,"tensile strength.")
  .add_property("Ets", &XC::Concrete02::getEts,  &XC::Concrete02::setEts,"tension softening stiffness (absolute value) (slope of the linear tension softening branch).")
  .add_property("ratioSlope", &XC::Concrete02::getLambda,  &XC::Concrete02::setLambda,"ratio between unloading slope at epscu and initial slope.")
  ;

class_<XC::Concrete02IS, bases<XC::Concrete02>, boost::noncopyable >("Concrete02IS", no_init)
  .add_property("Ec0", &XC::Concrete02IS::getInitialTangent,  &XC::Concrete02IS::setInitialTangent,"Get/set the value of the concrete initial tangent.")
  ;

class_<XC::Concrete04, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete04", no_init);

class_<XC::TDConcreteBase, bases<XC::RawConcrete>, boost::noncopyable >("TDConcreteBase", no_init)
  .add_property("ft", &XC::TDConcreteBase::getFt,  &XC::TDConcreteBase::setFt,"tensile strength.")
  .add_property("Ec", &XC::TDConcreteBase::getEc,  &XC::TDConcreteBase::setEc,"concrete stiffness.")
  .add_property("Et", &XC::TDConcreteBase::getEt,  &XC::TDConcreteBase::setEt,"concrete current stiffness.")
  .add_property("age", &XC::TDConcreteBase::getAge,  &XC::TDConcreteBase::setAge, "concrete age.")
  .add_property("beta", &XC::TDConcreteBase::getBeta,  &XC::TDConcreteBase::setBeta,"concrete beta parameter.")
  .add_property("tcast", &XC::TDConcreteBase::getTCast,  &XC::TDConcreteBase::setTCast,"tcast.")
  .def("setCreepOn", &XC::TDConcreteBase::setCreepOn,"Activate creep.").staticmethod("setCreepOn")
  .def("setCreepOff", &XC::TDConcreteBase::setCreepOff,"Deactivate creep.").staticmethod("setCreepOff")
  .def("isCreepOn", &XC::TDConcreteBase::isCreepOn,"Return true if creep is activen.").staticmethod("isCreepOn")
  .def("getCreepDt", &XC::TDConcreteBase::getCreepDt,"Get time increment for creep.").staticmethod("getCreepDt")
  .def("setCreepDt", &XC::TDConcreteBase::setCreepDt,"Set time increment for creep.").staticmethod("setCreepDt")
  ;

class_<XC::CreepShrinkageParameters, bases<CommandEntity> >("CreepShrinkageParameters")
  .def(init<const double &, const double &, const double &,const double &, const double &, const double &>())
  .def(init<XC::CreepShrinkageParameters>())
  .def("setup", &XC::CreepShrinkageParameters::setup_parameters,"Sets initial values for the concrete parameters; call after modifying any of the material properties.")
  .add_property("tcr", &XC::CreepShrinkageParameters::getCreepRelationshipAge,  &XC::CreepShrinkageParameters::setCreepRelationshipAge,"creep relationship age.")
  .add_property("epsshu", &XC::CreepShrinkageParameters::getUltimateShrinkage,  &XC::CreepShrinkageParameters::setUltimateShrinkage,"ultimate shrinkage.")
  .add_property("epssha", &XC::CreepShrinkageParameters::getShrinkageParameter,  &XC::CreepShrinkageParameters::setShrinkageParameter,"shrinkage parameter.")
  .add_property("epscru", &XC::CreepShrinkageParameters::getUltimateConcreteCreep,  &XC::CreepShrinkageParameters::setUltimateConcreteCreep,"ultimate concrete creep.")
  .add_property("epscra", &XC::CreepShrinkageParameters::getCreepExponentParameter,  &XC::CreepShrinkageParameters::setCreepExponentParameter,"creep exponent parameter.")
  .add_property("epscrd", &XC::CreepShrinkageParameters::getCreepDParameter,  &XC::CreepShrinkageParameters::setCreepDParameter,"creep d parameter.")
  ;

class_<XC::TDConcrete, bases<XC::TDConcreteBase>, boost::noncopyable >("TDConcrete", no_init)
  .def("setup", &XC::TDConcrete::setup_parameters,"Sets initial values for the concrete parameters; call after modifying any of the material properties.")
  .def("getCreepShrinkageParameters", make_function(&XC::TDConcrete::getCreepShrinkageParameters, return_internal_reference<>()))
  .def("setCreepShrinkageParameters",&XC::TDConcrete::setCreepShrinkageParameters)

  ;

class_<XC::TDConcreteMC10Base, bases<XC::TDConcreteBase>, boost::noncopyable >("TDConcreteMC10Base", no_init)
  .add_property("Ecm", &XC::TDConcreteMC10Base::getEcm, &XC::TDConcreteMC10Base::setEcm, "28-day modulus, necessary for normalizing creep coefficient.")

  .add_property("epsba", &XC::TDConcreteMC10Base::getEpsba, &XC::TDConcreteMC10Base::setEpsba, "ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010")
  .add_property("epsbb", &XC::TDConcreteMC10Base::getEpsbb, &XC::TDConcreteMC10Base::setEpsbb, "fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.")
  .add_property("epsda", &XC::TDConcreteMC10Base::getEpsda, &XC::TDConcreteMC10Base::setEpsda, "product of εcds,0 and βRH, as per Model Code 2010.")
  .add_property("epsdb", &XC::TDConcreteMC10Base::getEpsdb, &XC::TDConcreteMC10Base::setEpsdb, "fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.")

  .add_property("phiba", &XC::TDConcreteMC10Base::getPhiba, &XC::TDConcreteMC10Base::setPhiba, "parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.")
  .add_property("phibb", &XC::TDConcreteMC10Base::getPhibb, &XC::TDConcreteMC10Base::setPhibb, "fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.")
  .add_property("phida", &XC::TDConcreteMC10Base::getPhida, &XC::TDConcreteMC10Base::setPhida, "product of βdc(fcm) and β(RH), as per Model Code 2010.")
  .add_property("phidb", &XC::TDConcreteMC10Base::getPhidb, &XC::TDConcreteMC10Base::setPhidb, "fitting constant within the drying creep time evolution function as per Model Code 2010.")

  .add_property("cem", &XC::TDConcreteMC10Base::getCem, &XC::TDConcreteMC10Base::setCem, "coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.")
  ;

class_<XC::TDConcreteMC10, bases<XC::TDConcreteMC10Base>, boost::noncopyable >("TDConcreteMC10", no_init)
  .def("setup", &XC::TDConcreteMC10::setup_parameters,"Sets initial values for the concrete parameters; call after modifying any of the material properties.")

  ;

class_<XC::TDConcreteMC10NL, bases<XC::TDConcreteMC10>, boost::noncopyable >("TDConcreteMC10NL", no_init)
  .add_property("fcu", &XC::TDConcreteMC10NL::getFcu, &XC::TDConcreteMC10NL::setFcu, "stress at ultimate (crushing) strain.")
  ;

class_<XC::CreepMaterial, bases<XC::EncapsulatedUniaxialMaterial>, boost::noncopyable >("CreepMaterial", no_init)
  .def("setup", &XC::CreepMaterial::setup_parameters,"Sets initial values for the concrete parameters; call after modifying any of the material properties.")
  .def("getCreepShrinkageParameters", make_function(&XC::CreepMaterial::getCreepShrinkageParameters, return_internal_reference<>()))
  .def("setCreepShrinkageParameters",&XC::CreepMaterial::setCreepShrinkageParameters)
  ;
