//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
#ifndef nd_material_class_names_h
#define nd_material_class_names_h

#include <set>

namespace XC {

  const std::set<std::string> nd_material_class_names= {"XC::NDMaterial", "XC::ContactMaterialBase", "XC::ContactMaterial2D", "XC::ContactMaterial3D", "XC::DruckerPrager", "XC::DruckerPrager3D", "XC::DruckerPragerPlaneStrain", "XC::ElasticCrossAnisotropic", "XC::ElasticIsotropicMaterial", "XC::ElasticIsotropic2D", "XC::ElasticIsotropicPlaneStrain2D", "XC::ElasticIsotropicPlaneStress2D", "XC::ElasticIsotropic3D", "XC::ElasticIsotropicAxiSymm", "XC::ElasticIsotropicBeamFiber", "XC::ElasticIsotropicPlateFiber", "XC::PressureDependentElastic3D", "XC::FeapMaterial", "XC::FeapMaterial01", "XC::FeapMaterial02", "XC::FeapMaterial03", "XC::FiniteDeformationMaterial", "XC::FiniteDeformationEP3D", "XC::FiniteDeformationElastic3D", "XC::FDdecoupledElastic3D", "XC::NeoHookeanCompressible3D", "XC::InitialStateAnalysisWrapper", "XC::J2Plasticity", "XC::J2AxiSymm", "XC::J2PlaneStrain", "XC::J2PlaneStress", "XC::J2PlateFiber", "XC::J2ThreeDimensional", "XC::J2PlateFibre", "XC::MultiaxialCyclicPlasticity", "XC::MultiaxialCyclicPlasticity3D", "XC::MultiaxialCyclicPlasticityAxiSymm", "XC::MultiaxialCyclicPlasticityPlaneStrain", "XC::NDAdaptorMaterial", "XC::BeamFiberMaterial", "XC::PlaneStressMaterial", "XC::PlateFiberMaterial", "XC::PlasticDamageConcretePlaneStress", "XC::PlateAdaptorMaterial<>", "XC::PlateFromPlaneStressMaterial", "XC::PlateRebarMaterial", "XC::SoilMaterialBase", "XC::FluidSolidPorousMaterial", "XC::PressureMultiYieldBase", "XC::PressureDependMultiYieldBase", "XC::PressureDependMultiYield", "XC::PressureDependMultiYield02", "XC::PressureIndependMultiYield", "XC::Template3Dep"};
  
//! @brief Return true if the given class name corresponds to a coordinate transformation.
bool is_nd_material(const std::string &className)
  { return (XC::nd_material_class_names.find(className)!= XC::nd_material_class_names.end()); }
  
} // end of XC namespace

#endif

