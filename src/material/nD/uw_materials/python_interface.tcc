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

class_<XC::DruckerPrager, bases<XC::NDMaterial>, boost::noncopyable >("DruckerPrager", no_init)
  .add_property("rho", &XC::DruckerPrager::getRho,  &XC::DruckerPrager::setRho,"Get/Set density.")
  .add_property("k", &XC::DruckerPrager::getBulkModulus,  &XC::DruckerPrager::setBulkModulus,"Get/Set bulk modulus.")
  .add_property("G", &XC::DruckerPrager::getShearModulus,  &XC::DruckerPrager::setShearModulus,"Get/Set shear modulus.")
  .add_property("sigY", &XC::DruckerPrager::getYieldStress,  &XC::DruckerPrager::setYieldStress,"Get/Set yield stress.")
  .add_property("mrho", &XC::DruckerPrager::getFailureSurfaceRho,  &XC::DruckerPrager::setFailureSurfaceRho,"Failure surface and associativity. Get/set volumetric term.")
  .add_property("mrhoBar", &XC::DruckerPrager::getFailureSurfaceRhoBar,  &XC::DruckerPrager::setFailureSurfaceRhoBar,"Failure surface and associativity. Get/set nonassociative flow term.")
  .add_property("Kinf", &XC::DruckerPrager::getIsotropicHardeningKinf,  &XC::DruckerPrager::setIsotropicHardeningKinf,"Isotropic hardening. Get/set Kinf.")
  .add_property("Ko", &XC::DruckerPrager::getIsotropicHardeningKo,  &XC::DruckerPrager::setIsotropicHardeningKo,"Isotropic hardening. Get/set Ko.")
  .add_property("delta1", &XC::DruckerPrager::getIsotropicHardeningDelta,  &XC::DruckerPrager::setIsotropicHardeningDelta,"Isotropic hardening. Get/set delta.")
  .add_property("Hard", &XC::DruckerPrager::getKinematicHardeningH,  &XC::DruckerPrager::setKinematicHardeningH,"Kinematic hardening. Get/set H.")
  .add_property("theta", &XC::DruckerPrager::getKinematicHardeningTheta,  &XC::DruckerPrager::setKinematicHardeningTheta,"Kinematic hardening. Get/set Theta.")
  .add_property("delta2", &XC::DruckerPrager::getTensionSofteningDelta,  &XC::DruckerPrager::setTensionSofteningDelta,"Get/set tension softening delta.")
  .add_property("mTo", &XC::DruckerPrager::getMTo,  &XC::DruckerPrager::setMTo,"Get/set mTo.")
.add_property("referencePressure", &XC::DruckerPrager::getReferencePressure, &XC::DruckerPrager::setReferencePressure, "Get/set reference pressure.")
  .add_property("stage", &XC::DruckerPrager::getMaterialStage,  &XC::DruckerPrager::updateMaterialStage,"variable that sets the stage of the material; 0:elastic, 1:plastic.")
  .def("updateMaterialStage", &XC::DruckerPrager::updateMaterialStage, "Update the stage of the material; 0:elastic, 1:plastic.")
  .def("setup", &XC::DruckerPrager::setup, "Compute values of derived parameters.")
  .def("initialize", &XC::DruckerPrager::initialize, "Initialize stiffness matrix.")
  .add_property("state", make_function(&XC::DruckerPrager::getState,return_internal_reference<>()), "Return vector of state variables for output.")
  ;

class_<XC::DruckerPragerPlaneStrain , bases<XC::DruckerPrager>, boost::noncopyable >("DruckerPragerPlaneStrain", no_init)
  ;

class_<XC::DruckerPrager3D , bases<XC::DruckerPrager>, boost::noncopyable >("DruckerPrager3D", no_init)
  ;

XC::NDMaterial *(XC::InitialStateAnalysisWrapper::*get_nd_encapsulated_material)(void)= &XC::InitialStateAnalysisWrapper::getMaterial;
class_<XC::InitialStateAnalysisWrapper, bases<XC::NDMaterial>, boost::noncopyable >("InitialStateAnalysisWrapper", no_init)
   .def("setup", &XC::InitialStateAnalysisWrapper::setup, "Set the dimension and wrapped material.")
  .add_property("encapsulatedMaterial", make_function(get_nd_encapsulated_material,return_internal_reference<>()), "Get the encapsulated material.")
  .def("getInitialStateAnalysisPhase", &XC::InitialStateAnalysisWrapper::getInitialStateAnalysisPhase,"Return the initial state analysis phase flag.").staticmethod("getInitialStateAnalysisPhase")
  .def("setInitialStateAnalysisPhase", &XC::InitialStateAnalysisWrapper::setInitialStateAnalysisPhase,"Set the initial state analysis phase flag.").staticmethod("setInitialStateAnalysisPhase")
//.add_property("initialStateAnalysisPhase", &XC::InitialStateAnalysisWrapper::getInitialStateAnalysisPhase, &XC::InitialStateAnalysisWrapper::setInitialStateAnalysisPhase, "Get/set the initialStateAnalysisPhase flag (if true, initial state phase is computed).")
  ;

class_<XC::ContactMaterialBase, bases<XC::NDMaterial>, boost::noncopyable >("ContactMaterialBase", no_init)
  .add_property("frictionFlag", &XC::ContactMaterialBase::getFrictionFlag, &XC::ContactMaterialBase::setFrictionFlag, "Activates/deactivates the friction in the contact.")
  .def("setFrictionFlag", &XC::ContactMaterialBase::setFrictionFlag, "Set the valou of the friction flag (if true activate the friction in the contact).")
  .def("getFrictionFlag", &XC::ContactMaterialBase::getFrictionFlag, "Get the valou of the friction flag.")
  .add_property("frictionCoeff", &XC::ContactMaterialBase::getFrictionCoeff, &XC::ContactMaterialBase::setFrictionCoeff, "Get/set the interface frictional coefficient: tan(phi).")
  .add_property("stiffness", &XC::ContactMaterialBase::getStiffness, &XC::ContactMaterialBase::setStiffness, "Get/set the interface cohesive intercept.")
  .add_property("cohesion", &XC::ContactMaterialBase::getcohesion, &XC::ContactMaterialBase::setcohesion, "Get/set the interface cohesive intercept.")
  .add_property("tensileStrength", &XC::ContactMaterialBase::getTensileStrength, &XC::ContactMaterialBase::setTensileStrength, "Get/set the interface tensile strength.")
  ;
class_<XC::ContactMaterial2D, bases<XC::ContactMaterialBase>, boost::noncopyable >("ContactMaterial2D", no_init)
  ;
class_<XC::ContactMaterial3D, bases<XC::ContactMaterialBase>, boost::noncopyable >("ContactMaterial3D", no_init)
  ;
