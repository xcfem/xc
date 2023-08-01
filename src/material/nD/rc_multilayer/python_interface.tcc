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
typedef XC::PlateAdaptorMaterial<XC::UniaxialMaterialWrapper> plate_adaptor_uniaxial_material;
XC::UniaxialMaterial *(plate_adaptor_uniaxial_material::*getUniaxialMaterial)(void)= &plate_adaptor_uniaxial_material::getMaterial;
void (plate_adaptor_uniaxial_material::*setUniaxialMaterialByValue)(const XC::UniaxialMaterial &)= &plate_adaptor_uniaxial_material::setMaterial;
void (plate_adaptor_uniaxial_material::*setUniaxialMaterialByName)(const std::string &)= &plate_adaptor_uniaxial_material::setMaterial;
class_<plate_adaptor_uniaxial_material, bases<XC::NDMaterial>, boost::noncopyable >("plate_adaptor_uniaxial_material", no_init)
  .add_property("encapsulatedMaterial", make_function(getUniaxialMaterial,return_internal_reference<>()),  setUniaxialMaterialByValue,"Get/set the underlying material.")
  .def("setEncapsulatedMaterial", setUniaxialMaterialByValue, "Set the underlying material.")
  .def("setEncapsulatedMaterial", setUniaxialMaterialByName, "Set the underlying material.")
  ;

class_<XC::PlateRebarMaterial, bases<plate_adaptor_uniaxial_material>, boost::noncopyable >("PlateRebarMaterial", no_init)
  .add_property("angle", &XC::PlateRebarMaterial::getAngle,  &XC::PlateRebarMaterial::setAngle,"set the angle of the rebars.")
  ;

typedef XC::PlateAdaptorMaterial<XC::NDMaterialWrapper> plate_adaptor_nd_material;
XC::NDMaterial *(plate_adaptor_nd_material::*getNDMaterial)(void)= &plate_adaptor_nd_material::getMaterial;
void (plate_adaptor_nd_material::*setNDMaterialByValue)(const XC::NDMaterial &)= &plate_adaptor_nd_material::setMaterial;
void (plate_adaptor_nd_material::*setNDMaterialByName)(const std::string &)= &plate_adaptor_nd_material::setMaterial;
class_<plate_adaptor_nd_material, bases<XC::NDMaterial>, boost::noncopyable >("plate_adaptor_nd_material", no_init)
  .add_property("encapsulatedMaterial", make_function(getNDMaterial,return_internal_reference<>()),  setNDMaterialByValue,"Get/set the underlying material.")
  .def("setEncapsulatedMaterial", setNDMaterialByValue, "Set the underlying material.")
  .def("setEncapsulatedMaterial", setNDMaterialByName, "Set the underlying material.")
  ;

class_<XC::PlateFromPlaneStressMaterial, bases<plate_adaptor_nd_material>, boost::noncopyable >("PlateFromPlaneStressMaterial", no_init)
  .add_property("outOfPlaneShearModulus", &XC::PlateFromPlaneStressMaterial::getGmod,  &XC::PlateFromPlaneStressMaterial::setGmod,"set the out of plane shear modulus.")
  ;

class_<XC::PlasticDamageConcretePlaneStress, bases<XC::NDMaterial>, boost::noncopyable >("PlasticDamageConcretePlaneStress", no_init)
  .add_property("E", &XC::PlasticDamageConcretePlaneStress::getE,  &XC::PlasticDamageConcretePlaneStress::setE,"Get/set the elasticModulus.")
  .add_property("nu", &XC::PlasticDamageConcretePlaneStress::getNu,  &XC::PlasticDamageConcretePlaneStress::setNu,"Get/set the Poisson's ratio.")
  .add_property("ft", &XC::PlasticDamageConcretePlaneStress::getFt,  &XC::PlasticDamageConcretePlaneStress::setFt,"Get/set tensile yield strength.")
  .add_property("fc", &XC::PlasticDamageConcretePlaneStress::getFc,  &XC::PlasticDamageConcretePlaneStress::setFc,"Get/set compressive yield strength.")
  .add_property("beta", &XC::PlasticDamageConcretePlaneStress::getBeta,  &XC::PlasticDamageConcretePlaneStress::setBeta,"Get/set plastic deformation rate. Parameter controlling plastic strain rate/post-yield hardening parameter.")
  .add_property("Ap", &XC::PlasticDamageConcretePlaneStress::getAp,  &XC::PlasticDamageConcretePlaneStress::setAp,"Get/set parameter controlling tensile fracture energy.")
  .add_property("An", &XC::PlasticDamageConcretePlaneStress::getAn,  &XC::PlasticDamageConcretePlaneStress::setAn,"Get/set parameter controlling ductility of the compressive response.")
  .add_property("Bn", &XC::PlasticDamageConcretePlaneStress::getBn,  &XC::PlasticDamageConcretePlaneStress::setBn,"Get/set parameter controlling ductility and peak strength of the compressive response.")
  .def("setup", &XC::PlasticDamageConcretePlaneStress::setup, "Update internal variables (call after setting or changing the material parameters.")
  ;
