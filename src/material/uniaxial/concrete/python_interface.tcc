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

class_<XC::Concrete04, bases<XC::ConcreteBase>, boost::noncopyable >("Concrete04", no_init);

class_<XC::TDConcrete, bases<XC::RawConcrete>, boost::noncopyable >("TDConcrete", no_init)
  .def("setup", &XC::TDConcrete::setup_parameters,"Sets initial values for the concrete parameters; call after modifying any of the material properties.")
  .add_property("ft", &XC::TDConcrete::getFt,  &XC::TDConcrete::setFt,"tensile strength.")
  .add_property("Ec", &XC::TDConcrete::getEc,  &XC::TDConcrete::setEc,"concrete stiffness.")
  .add_property("beta", &XC::TDConcrete::getBeta,  &XC::TDConcrete::setBeta,"concrete beta parameter.")
  .add_property("epsshu", &XC::TDConcrete::getUltimateShrinkage,  &XC::TDConcrete::setUltimateShrinkage,"ultimate shrinkage.")
  .add_property("epssha", &XC::TDConcrete::getShrinkageParameter,  &XC::TDConcrete::setShrinkageParameter,"shrinkage parameter.")
  .add_property("tcr", &XC::TDConcrete::getCreepRelationshipAge,  &XC::TDConcrete::setCreepRelationshipAge,"creep relationship age.")
  .add_property("epscru", &XC::TDConcrete::getUltimateConcreteCreep,  &XC::TDConcrete::setUltimateConcreteCreep,"ultimate concrete creep.")
  .add_property("epscra", &XC::TDConcrete::getCreepExponentParameter,  &XC::TDConcrete::setCreepExponentParameter,"creep exponent parameter.")
  .add_property("epscrd", &XC::TDConcrete::getCreepDParameter,  &XC::TDConcrete::setCreepDParameter,"creep d parameter.")
  .add_property("tcast", &XC::TDConcrete::getTCast,  &XC::TDConcrete::setTCast,"tcast.")
  .add_property("Et", &XC::TDConcrete::getEt,  &XC::TDConcrete::setEt,"concrete current stiffness.")
  .def("setCreepOn", &XC::TDConcrete::setCreepOn,"Activate creep.").staticmethod("setCreepOn")
  .def("setCreepOff", &XC::TDConcrete::setCreepOff,"Deactivate creep.").staticmethod("setCreepOff")
  .def("getCreepDt", &XC::TDConcrete::getCreepDt,"Get time increment for creep.").staticmethod("getCreepDt")
  .def("setCreepDt", &XC::TDConcrete::setCreepDt,"Set time increment for creep.").staticmethod("setCreepDt")
  ;


