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
  .add_property("mrhoBar", &XC::DruckerPrager::getFailureSurfaceRho,  &XC::DruckerPrager::setFailureSurfaceRho,"Failure surface and associativity. Get/set nonassociative flow term.")
  .add_property("Kinf", &XC::DruckerPrager::getIsotropicHardeningKinf,  &XC::DruckerPrager::setIsotropicHardeningKinf,"Isotropic hardening. Get/set Kinf.")
  .add_property("Ko", &XC::DruckerPrager::getIsotropicHardeningKo,  &XC::DruckerPrager::setIsotropicHardeningKo,"Isotropic hardening. Get/set Ko.")
  .add_property("delta1", &XC::DruckerPrager::getIsotropicHardeningDelta,  &XC::DruckerPrager::setIsotropicHardeningDelta,"Isotropic hardening. Get/set delta.")
  .add_property("Hard", &XC::DruckerPrager::getKinematicHardeningH,  &XC::DruckerPrager::setKinematicHardeningH,"Kinematic hardening. Get/set H.")
  .add_property("theta", &XC::DruckerPrager::getKinematicHardeningTheta,  &XC::DruckerPrager::setKinematicHardeningTheta,"Kinematic hardening. Get/set Theta.")
  .add_property("delta2", &XC::DruckerPrager::getTensionSofteningDelta,  &XC::DruckerPrager::setTensionSofteningDelta,"Get/set tension softening delta.")
  .add_property("mTo", &XC::DruckerPrager::getMTo,  &XC::DruckerPrager::setMTo,"Get/set mTo.")
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
