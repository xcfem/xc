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
//MaterialHandler.cc

#include "MaterialHandler.h"
#include "domain/domain/Domain.h"


//Uniaxial material.
#include "material/uniaxial/BarSlipMaterial.h"
//#include "material/uniaxial/BoucWenMaterial.h"
#include "material/uniaxial/CableMaterial.h"
#include "material/uniaxial/concrete/Concrete01.h"
#include "material/uniaxial/concrete/Concrete02.h"
#include "material/uniaxial/concrete/Concrete04.h"
#include "material/uniaxial/concrete/TDConcrete.h"
#include "material/uniaxial/concrete/TDConcreteMC10.h"
#include "material/uniaxial/concrete/TDConcreteMC10NL.h"
#include "material/uniaxial/ENTMaterial.h"
#include "material/uniaxial/ENCMaterial.h"
#include "material/uniaxial/TensionOnlyMaterial.h"
#include "material/uniaxial/CompressionOnlyMaterial.h"
#include "material/uniaxial/InvertMaterial.h"
#include "material/uniaxial/EPPGapMaterial.h"
#include "material/uniaxial/ElasticMaterial.h"
#include "material/uniaxial/ElasticPPMaterial.h"
#include "material/uniaxial/FatigueMaterial.h"
#include "material/uniaxial/HardeningMaterial.h"
#include "material/uniaxial/HystereticMaterial.h"
#include "material/uniaxial/InitStrainMaterial.h"
#include "material/uniaxial/InitStressMaterial.h"
#include "material/uniaxial/MultiLinear.h"
#include "material/uniaxial/MinMaxMaterial.h"
#include "material/uniaxial/NewUniaxialMaterial.h"
#include "material/uniaxial/connected/ParallelMaterial.h"
#include "material/uniaxial/connected/SeriesMaterial.h"
#include "material/uniaxial/PathIndependentMaterial.h"
#include "material/uniaxial/Pinching4Material.h"
#include "material/uniaxial/ReinforcingSteel.h"
#include "material/uniaxial/steel/Steel01.h"
#include "material/uniaxial/steel/Steel02.h"
#include "material/uniaxial/steel/Steel03.h"
#include "material/uniaxial/ViscousMaterial.h"

#include "material/uniaxial/fedeas/FedeasBond1Material.h"
#include "material/uniaxial/fedeas/FedeasBond2Material.h"
#include "material/uniaxial/fedeas/FedeasConcr1Material.h"
#include "material/uniaxial/fedeas/FedeasConcr2Material.h"
#include "material/uniaxial/fedeas/FedeasConcr3Material.h"
#include "material/uniaxial/fedeas/FedeasHardeningMaterial.h"
#include "material/uniaxial/fedeas/FedeasHyster1Material.h"
#include "material/uniaxial/fedeas/FedeasHyster2Material.h"
#include "material/uniaxial/fedeas/FedeasSteel1Material.h"
#include "material/uniaxial/fedeas/FedeasSteel2Material.h"

#include "material/uniaxial/soil_structure_interaction/PySimple1.h"
#include "material/uniaxial/soil_structure_interaction/QzSimple1.h"
#include "material/uniaxial/soil_structure_interaction/TzSimple1.h"
#include "material/uniaxial/soil_structure_interaction/PyLiq1.h"
#include "material/uniaxial/soil_structure_interaction/TzLiq1.h"
#include "material/uniaxial/soil_structure_interaction/EyBasic.h"

#include "material/uniaxial/snap/Bilinear.h"
#include "material/uniaxial/snap/Clough.h"
#include "material/uniaxial/snap/CloughDamage.h"
#include "material/uniaxial/snap/Pinching.h"
#include "material/uniaxial/snap/PinchingDamage.h"


//nDMaterial
#include "material/nD/NDMaterial.h"
#include "material/nD/template_3d_ep/Template3Dep.h"

#include "material/nD/ElasticCrossAnisotropic.h"
#include "material/nD/elastic_isotropic/ElasticIsotropic3D.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h"
#include "material/nD/elastic_isotropic/PressureDependentElastic3D.h"

#include "material/nD/FeapMaterial.h"

#include "material/nD/j2_plasticity/J2AxiSymm.h"
#include "material/nD/j2_plasticity/J2PlaneStrain.h"
#include "material/nD/j2_plasticity/J2PlaneStress.h"
#include "material/nD/j2_plasticity/J2PlateFiber.h"
#include "material/nD/j2_plasticity/J2PlateFibre.h"
#include "material/nD/j2_plasticity/J2ThreeDimensional.h"

#include "material/nD/nd_adaptor/PlaneStressMaterial.h"
#include "material/nD/nd_adaptor/PlateFiberMaterial.h"
#include "material/nD/nd_adaptor/BeamFiberMaterial.h"

#include "material/nD/FiniteDeformation/FDdecoupledElastic3D.h"
#include "material/nD/FiniteDeformation/NeoHookeanCompressible3D.h"
#include "material/nD/FiniteDeformation/FiniteDeformationEP3D.h"

#include "material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity3D.h"
#include "material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityAxiSymm.h"
#include "material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityPlaneStrain.h"

#include "material/nD/soil/FluidSolidPorousMaterial.h"
#include "material/nD/soil/PressureDependMultiYield.h"
#include "material/nD/soil/PressureDependMultiYield02.h"
#include "material/nD/soil/PressureIndependMultiYield.h"

#include "material/nD/template_3d_ep/CamClayPotentialSurface.h"
#include "material/nD/template_3d_ep/CamClayYieldSurface.h"
#include "material/nD/template_3d_ep/DruckerPragerPotentialSurface.h"
#include "material/nD/template_3d_ep/DruckerPragerYieldSurface.h"
#include "material/nD/template_3d_ep/EPState.h"
#include "material/nD/template_3d_ep/EvolutionLaw_LEeq.h"
#include "material/nD/template_3d_ep/EvolutionLaw_LEij.h"
#include "material/nD/template_3d_ep/EvolutionLaw_NLEeq.h"
#include "material/nD/template_3d_ep/EvolutionLaw_NLEij.h"
#include "material/nD/template_3d_ep/EvolutionLaw_NLEijMD.h"
#include "material/nD/template_3d_ep/EvolutionLaw_NLEp.h"
#include "material/nD/template_3d_ep/EvolutionLaw_S.h"
#include "material/nD/template_3d_ep/EvolutionLaw_T.h"
#include "material/nD/template_3d_ep/ManzariDafaliasEvolutionLaw.h"
#include "material/nD/template_3d_ep/ManzariDafaliasPotentialSurface01.h"
#include "material/nD/template_3d_ep/ManzariDafaliasPotentialSurface.h"
#include "material/nD/template_3d_ep/ManzariDafaliasYieldSurface.h"
#include "material/nD/template_3d_ep/MatPoint3D.h"
#include "material/nD/template_3d_ep/PotentialSurface.h"
#include "material/nD/template_3d_ep/RoundedMohrCoulomb01.h"
#include "material/nD/template_3d_ep/RoundedMohrCoulomb01PotentialSurface.h"
#include "material/nD/template_3d_ep/RoundedMohrCoulomb01YieldSurface.h"
#include "material/nD/template_3d_ep/Template3Dep.h"
#include "material/nD/template_3d_ep/Tri_a_fail_crit_YieldSurface.h"
#include "material/nD/template_3d_ep/VonMisesPotentialSurface.h"
#include "material/nD/template_3d_ep/VonMisesYieldSurface.h"
#include "material/nD/template_3d_ep/YieldSurface.h"

#include "material/nD/uw_materials/DruckerPrager.h"
#include "material/nD/uw_materials/DruckerPragerPlaneStrain.h"
#include "material/nD/uw_materials/DruckerPrager3D.h"
#include "material/nD/uw_materials/InitialStateAnalysisWrapper.h"
#include "material/nD/uw_materials/ContactMaterial2D.h"
#include "material/nD/uw_materials/ContactMaterial3D.h"

// RC multilayer.
#include "material/nD/rc_multilayer/PlateRebarMaterial.h"
#include "material/nD/rc_multilayer/PlateFromPlaneStressMaterial.h"
#include "material/nD/rc_multilayer/PlasticDamageConcretePlaneStress.h"

//Section
#include "material/section/Bidirectional.h"
#include "material/section/elastic_section/ElasticSection1d.h"
#include "material/section/elastic_section/ElasticSection2d.h"
#include "material/section/elastic_section/ElasticShearSection2d.h"
#include "material/section/elastic_section/ElasticSection3d.h"
#include "material/section/elastic_section/ElasticShearSection3d.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSectionShear2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "material/section/fiber_section/FiberSectionShear3d.h"
#include "material/section/GenericSection1d.h"
#include "material/section/GenericSectionNd.h"
#include "material/section/Isolator2spring.h"
#include "material/section/SectionAggregator.h"
#include "material/section/yieldSurface/YS_Section2D02.h"
#include "material/section/yieldSurface/YS_Section2D01.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "material/section/interaction_diagram/InteractionDiagram2d.h"

//Plate section.
#include "material/section/plate_section/ElasticPlateSection.h"
#include "material/section/plate_section/ElasticMembranePlateSection.h"
#include "material/section/plate_section/MembranePlateFiberSection.h"
#include "material/section/plate_section/LayeredShellFiberSection.h"

//Yield surface
#include "material/yieldSurface/plasticHardeningMaterial/ExponReducing.h"
#include "material/yieldSurface/plasticHardeningMaterial/MultiLinearKp.h"
#include "material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h"

#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::MaterialHandler::MaterialHandler(Preprocessor *owr)
  : PrepHandler(owr), tag_mat(0) {}

XC::Material *load_uniaxial_py_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "py_simple1")
      retval= new XC::PySimple1(tag_mat);
    else if(cmd == "qz_simple1")
      retval= new XC::QzSimple1(tag_mat);
    else if(cmd == "tz_simple1")
      retval= new XC::TzSimple1(tag_mat);
    else if(cmd == "py_liq1")
      retval= new XC::PyLiq1(tag_mat);
    else if(cmd == "tz_liq1")
      retval= new XC::TzLiq1(tag_mat);
    else if((cmd == "ey_base") || (cmd == "EyBasic"))
      retval= new XC::EyBasic(tag_mat);
   return retval;
  }

XC::Material *load_uniaxial_snap_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "snap_bilinear")
      retval= new XC::Bilinear(tag_mat);
    else if(cmd == "snap_clough")
      retval= new XC::Clough(tag_mat);
    else if(cmd == "snap_pinching")
      retval= new XC::Pinching(tag_mat);
    else if(cmd == "snap_clough_damage")
      retval= new XC::Clough(tag_mat);
    else if(cmd == "snap_pinching_damage")
      retval= new XC::Pinching(tag_mat);
    return retval;
  }

XC::Material *load_uniaxial_fedeas_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "fedeas_bond1_material")
      retval= new XC::FedeasBond1Material(tag_mat);
    else if(cmd == "fedeas_bond2_material")
      retval= new XC::FedeasBond2Material(tag_mat);
    else if(cmd == "fedeas_concr1_material")
      retval= new XC::FedeasConcr1Material(tag_mat);
    else if(cmd == "fedeas_concr2_material")
      retval= new XC::FedeasConcr2Material(tag_mat);
    else if(cmd == "fedeas_concr3_material")
      retval= new XC::FedeasConcr3Material(tag_mat);
    else if(cmd == "fedeas_hardening_material")
      retval= new XC::FedeasHardeningMaterial(tag_mat);
    else if(cmd == "fedeas_hyster1_material")
      retval= new XC::FedeasHyster1Material(tag_mat);
    else if(cmd == "fedeas_hyster2_material")
      retval= new XC::FedeasHyster2Material(tag_mat);
    else if(cmd == "fedeas_steel1_material")
      retval= new XC::FedeasSteel1Material(tag_mat);
    else if(cmd == "fedeas_steel2_material")
      retval= new XC::FedeasSteel2Material(tag_mat);
    return retval;
  }

//! @brief Process the commands used to define uniaxial materials
//! to be used in the finite element model.
//! Interprets the following commands:
//!
//! - elastic_material: Linear elastic uniaxial material (ElasticMaterial).
//! - elasticpp_material: Elastic perfectly plastic uniaxial material (ElasticPPMaterial).
//! - epp_gap_material: Elastic perfectly plastic uniaxial material with "gap"(EPPGapMaterial).
//! - cable_material: CableMaterial (no compression).
//! - concrete01_material: Concrete01 uniaxial material.
//! - concrete02_material: Concrete02 uniaxial material.
//! - concrete04_material: Concrete04 uniaxial material.
//! - tdconcrete_material: TDConcrete uniaxial material.
//! - tdconcrete_mc10_material: TDConcreteMC10 uniaxial material.
//! - tdconcrete_mc10nl_material: TDConcreteMC10NL uniaxial material.
//! - steel01: Steel01 material.
//! - steel02: Steel02 material.
//! - steel03: Steel03 material.
//! - multi_linear: MultiLinear material.
XC::Material *load_uniaxial_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "bar_slip_material")
      retval= new XC::BarSlipMaterial(tag_mat);
    else if(cmd == "cable_material")
      retval= new XC::CableMaterial(tag_mat);
    else if(cmd == "concrete01_material")
      retval= new XC::Concrete01(tag_mat);
    else if(cmd == "concrete02_material")
      retval= new XC::Concrete02(tag_mat);
    else if(cmd == "concrete04_material")
      retval= new XC::Concrete04(tag_mat);
    else if(cmd == "tdconcrete_material")
      retval= new XC::TDConcrete(tag_mat);
    else if(cmd == "tdconcrete_mc10_material")
      retval= new XC::TDConcreteMC10(tag_mat);
    else if(cmd == "tdconcrete_mc10nl_material")
      retval= new XC::TDConcreteMC10NL(tag_mat);
    else if((cmd == "elastic_no_traction_material") or (cmd == "elastic_no_tension_material"))
      retval= new XC::ENTMaterial(tag_mat);
    else if(cmd == "elastic_no_compression_material")
      retval= new XC::ENCMaterial(tag_mat);
    else if(cmd == "tension_only_material")
      retval= new XC::TensionOnlyMaterial(tag_mat);
    else if(cmd == "compression_only_material")
      retval= new XC::CompressionOnlyMaterial(tag_mat);
    else if(cmd == "invert_material")
      retval= new XC::InvertMaterial(tag_mat);
    else if(cmd == "epp_gap_material")
      retval= new XC::EPPGapMaterial(tag_mat);
    else if(cmd == "elastic_material")
      retval= new XC::ElasticMaterial(tag_mat);
    else if(cmd == "elasticpp_material")
      retval= new XC::ElasticPPMaterial(tag_mat);
    else if(cmd == "fatigue_material")
      retval= new XC::FatigueMaterial(tag_mat);
    else if(cmd == "hardening_material")
      retval= new XC::HardeningMaterial(tag_mat);
    else if(cmd == "hysteretic_material")
      retval= new XC::HystereticMaterial(tag_mat);
    else if(cmd == "init_strain_material")
      retval= new XC::InitStrainMaterial(tag_mat);
    else if(cmd == "init_stress_material")
      retval= new XC::InitStressMaterial(tag_mat);
    else if(cmd == "min_max_material")
      retval= new XC::MinMaxMaterial(tag_mat);
    else if(cmd == "parallel_material")
      retval= new XC::ParallelMaterial(tag_mat);
    else if(cmd == "path_independent_material")
      retval= new XC::PathIndependentMaterial(tag_mat);
    else if(cmd == "pinching4_material")
      retval= new XC::Pinching4Material(tag_mat);
    else if(cmd == "series_material")
      retval= new XC::SeriesMaterial(tag_mat);
    else if(cmd == "reinforcing_steel")
      retval= new XC::ReinforcingSteel(tag_mat);
    else if(cmd == "steel01")
      retval= new XC::Steel01(tag_mat);
    else if(cmd == "steel02")
      retval= new XC::Steel02(tag_mat);
    else if(cmd == "steel03")
      retval= new XC::Steel03(tag_mat);
    else if((cmd == "viscous") or (cmd == "viscous_material"))
      retval= new XC::ViscousMaterial(tag_mat);
    else if(cmd == "multi_linear")
      retval= new XC::MultiLinear(tag_mat);
    return retval;
  }

//! @brief Process the commands used to define nD materials to
//! be used in the finite element model.
//! Interprets the following commands:
//!
//! - elastic_cross_anisotropic: ElasticCrossAnisotropic material.
//! - elastic_isotropic_3d: ElasticIsotropic3D material.
//! - elastic_isotropic_plane_stress_2d: ElasticIsotropicPlaneStress material.
//! - elastic_isotropic_plane_strain_2d: ElasticIsotropicPlaneStrain material.
XC::Material *load_nD_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "elastic_cross_anisotropic")
      retval= new XC::ElasticCrossAnisotropic(tag_mat);
    else if(cmd == "elastic_isotropic_3d")
      retval= new XC::ElasticIsotropic3D(tag_mat);
    else if(cmd == "elastic_isotropic_plate_fiber")
      retval= new XC::ElasticIsotropicPlateFiber(tag_mat);
    else if(cmd == "elastic_isotropic_plane_stress_2d")
      retval= new XC::ElasticIsotropicPlaneStress2D(tag_mat);
    else if(cmd == "elastic_isotropic_plane_strain_2d")
      retval= new XC::ElasticIsotropicPlaneStrain2D(tag_mat);
    else if(cmd == "elastic_isotropic_beam_fiber")
      retval= new XC::ElasticIsotropicBeamFiber(tag_mat);
    else if(cmd == "J2_axy_symm")
      retval= new XC::J2AxiSymm(tag_mat);
    else if(cmd == "J2_plane_strain")
      retval= new XC::J2PlaneStrain(tag_mat);
    else if(cmd == "J2_plane_stress")
      retval= new XC::J2PlaneStress(tag_mat);
    else if(cmd == "J2_plate_fiber")
      retval= new XC::J2PlateFiber(tag_mat);
    else if(cmd == "J2_plate_fibre")
      retval= new XC::J2PlateFibre(tag_mat);
    else if(cmd == "J2_three_dimensional")
      retval= new XC::J2ThreeDimensional(tag_mat);
    else if(cmd == "plane_stress_material")
      retval= new XC::PlaneStressMaterial(tag_mat);
    else if(cmd == "plate_fiber_material")
      retval= new XC::PlateFiberMaterial(tag_mat);
    else if(cmd == "pressure_dependent_elastic_3d")
      retval= new XC::PressureDependentElastic3D(tag_mat);
    else if(cmd == "template_3d_ep")
      retval= new XC::Template3Dep(tag_mat);
    else if(cmd == "drucker-prager_plane_strain")
      retval= new XC::DruckerPragerPlaneStrain(tag_mat);
    else if(cmd == "drucker-prager_3d")
      retval= new XC::DruckerPrager3D(tag_mat);
    else if(cmd == "initial_state_analysis_wrapper")
      retval= new XC::InitialStateAnalysisWrapper(tag_mat);
    else if(cmd == "contact_material_2d")
      retval= new XC::ContactMaterial2D(tag_mat);
    else if(cmd == "contact_material_3d")
      retval= new XC::ContactMaterial3D(tag_mat);
    else if(cmd == "plastic_damage_concrete_plane_stress")
      retval= new XC::PlasticDamageConcretePlaneStress(tag_mat);
    else if(cmd == "plate_rebar")
      retval= new XC::PlateRebarMaterial(tag_mat);
    else if(cmd == "plate_from_plane_stress")
      retval= new XC::PlateFromPlaneStressMaterial(tag_mat);
    return retval;
  }

XC::Material *load_nD_finite_deformation_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "neo_hookean_compressible_3d")
      retval= new XC::NeoHookeanCompressible3D(tag_mat);
    else if(cmd == "fd_decoupled_elastic_3d")
      retval= new XC::FDdecoupledElastic3D(tag_mat);
    else if(cmd == "finite_deformation_ep_3d")
      retval= new XC::FiniteDeformationEP3D(tag_mat);
    return retval;
  }

XC::Material *load_nD_cyclic_soil_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "multiaxial_cyclic_plasticity_3D")
      retval= new XC::MultiaxialCyclicPlasticity3D(tag_mat);
    else if(cmd == "multiaxial_cyclic_plasticity_axi_symm")
      retval= new XC::MultiaxialCyclicPlasticityAxiSymm(tag_mat);
    else if(cmd == "multiaxial_cyclic_plasticity_plane_strain")
      retval= new XC::MultiaxialCyclicPlasticityPlaneStrain(tag_mat);
    return retval;
  }

XC::Material *load_nD_soil_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "fluid_solid_porous_material")
      retval= new XC::FluidSolidPorousMaterial(tag_mat);
    else if(cmd == "pressure_depend_multi_yield")
      retval= new XC::PressureDependMultiYield(tag_mat);
    else if(cmd == "pressure_depend_multi_yield02")
      retval= new XC::PressureDependMultiYield02(tag_mat);
    else if(cmd == "pressure_independ_multi_yield")
      retval= new XC::PressureIndependMultiYield(tag_mat);
    return retval;
  }

//! @brief Process the commands used to define membrane/plate/shell materials to
//! be used in the finite element model.
//! Interprets the following commands:
//! - elastic_membrane_plate_section: Elastic material for membrane elements.
//! - elastic_plate_section: Elastic material for plate elements.
XC::Material *load_plate_section_material(int tag_mat,const std::string &cmd,XC::MaterialHandler *mhandler)
  {
    XC::Material *retval= nullptr;
    if(cmd == "elastic_membrane_plate_section")
      retval= new XC::ElasticMembranePlateSection(tag_mat);
    else if(cmd == "elastic_plate_section")
      retval= new XC::ElasticPlateSection(tag_mat);
    else if(cmd == "membrane_plate_fiber_section")
      retval= new XC::MembranePlateFiberSection(tag_mat);
    else if(cmd == "layered_shell_fiber_section")
      retval= new XC::LayeredShellFiberSection(tag_mat);
    return retval;
  }

//! @brief Process the commands used to define cross section materials to
//! be used in the finite element model.
//! Interprets the following commands:
//!
//! - elastic_section_2d: Elastic material for 1D elements in 2D problems.
//! - elastic_shear_section_2d: Elastic material with shear stiffness for 1D elements in 2D problems.
//! - elastic_section_3d: Elastic material for 1D elements in 3D problems.
//! - elastic_shear_section_3d: Elastic material with shear stiffness for 1D elements in 2D problems.
//! - fiber_section_2d: Cross-section fiber model for 1D elements in 2D problems.
//! - fiber_section_3d: Cross-section fiber model for 1D elements in 3D problems.
//! - fiber_section_GJ: Cross-section fiber model for 1D elements with shear and torsion stiffness (3D problems).
//! - section_aggregator: Agreggation of cross-section mechanical properties.
XC::Material *load_section_material(int tag_mat,const std::string &cmd,XC::MaterialHandler *mhandler)
  {
    XC::Material *retval= nullptr;
    if(cmd == "bidirectional_section")
      retval= new XC::Bidirectional(tag_mat);
    else if(cmd == "elastic_section_1d")
      retval= new XC::ElasticSection1d(tag_mat,mhandler);
    else if(cmd == "elastic_section_2d")
      retval= new XC::ElasticSection2d(tag_mat,mhandler);
    else if((cmd == "elasticShearSection2d") || (cmd == "elastic_shear_section_2d"))
      retval= new XC::ElasticShearSection2d(tag_mat,mhandler);
    else if(cmd == "elastic_section_3d")
      retval= new XC::ElasticSection3d(tag_mat,mhandler);
    else if((cmd == "elasticShearSection3d")  || (cmd == "elastic_shear_section_3d"))
      retval= new XC::ElasticShearSection3d(tag_mat,mhandler);
    else if(cmd == "fiber_section_2d")
      retval= new XC::FiberSection2d(tag_mat,mhandler);
    else if(cmd == "fiberSectionShear2d" || (cmd == "fiber_section_shear_2d"))
      retval= new XC::FiberSectionShear2d(tag_mat,mhandler);
    else if(cmd == "fiber_section_3d")
      retval= new XC::FiberSection3d(tag_mat,mhandler);
    else if(cmd == "fiber_section_GJ")
      retval= new XC::FiberSectionGJ(tag_mat,mhandler);
    else if((cmd == "fiberSectionShear3d") || (cmd == "fiber_section_shear_3d"))
      retval= new XC::FiberSectionShear3d(tag_mat,mhandler);
    else if(cmd == "generic_section_1d")
      retval= new XC::GenericSection1d(tag_mat);
    else if(cmd == "isolator2spring")
      retval= new XC::Isolator2spring(tag_mat);
    else if(cmd == "section_aggregator")
      retval= new XC::SectionAggregator(tag_mat,mhandler);
    else if(cmd == "ys_section_2d_02")
      retval= new XC::YS_Section2D02(tag_mat);
    else if(cmd == "ys_section_2d_01")
      retval= new XC::YS_Section2D01(tag_mat);
    return retval;
  }

XC::Material *load_yield_surf_plastic_hardening(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "expon_reducing")
      retval= new XC::ExponReducing(tag_mat);
    else if(cmd == "multi_linear_kp")
      retval= new XC::MultiLinearKp(tag_mat);
    else if(cmd == "null_plastic_material")
      retval= new XC::NullPlasticMaterial(tag_mat);
    return retval;
  }
XC::Material *load_material(int tag_mat,const std::string &cmd,XC::MaterialHandler *mhandler)
  {
    XC::Material *retval= load_uniaxial_material(tag_mat,cmd);
    if(!retval)
      retval= load_uniaxial_fedeas_material(tag_mat,cmd);
    if(!retval)
      retval= load_uniaxial_py_material(tag_mat,cmd);
    if(!retval)
      retval= load_uniaxial_snap_material(tag_mat,cmd);
    if(!retval)
      retval= load_nD_material(tag_mat,cmd);
    if(!retval)
      retval= load_nD_finite_deformation_material(tag_mat,cmd);
    if(!retval)
      retval= load_nD_cyclic_soil_material(tag_mat,cmd);
    if(!retval)
      retval= load_nD_soil_material(tag_mat,cmd);
    if(!retval)
      retval= load_section_material(tag_mat,cmd,mhandler);
    if(!retval)
      retval= load_plate_section_material(tag_mat,cmd,mhandler);
    if(!retval)
      retval= load_yield_surf_plastic_hardening(tag_mat,cmd);
    return retval;
  }

//! @brief Remove a material from this container.
void XC::MaterialHandler::removeMaterial(const std::string &cod_mat)
  {
    map_materials::iterator i= materials.find(cod_mat);
    if(i!=materials.end()) //Material exists.
      {
	delete materials[cod_mat];
	materials[cod_mat]= nullptr;
	materials.erase(cod_mat);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; material: '" << cod_mat << "' not found."
	        << Color::def << std::endl;
  }

//! @brief Defines a new material.
XC::Material *XC::MaterialHandler::newMaterial(const std::string &mat_type,const std::string &cod_mat)
  {
    
    std::string material_code= cod_mat;
    if((material_code.size()==0) || (material_code=="auto"))
      material_code= mat_type+'_'+std::to_string(tag_mat);
    Material *retval= load_material(tag_mat,mat_type,this);
    
    if(retval)
      {
        retval->set_owner(this);
        if(materials.find(material_code)!=materials.end()) //Material exists.
          {
	    std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		      << "; warning material: '"
		      << material_code << "' redefined."
	              << " Use 'auto' to let the system assign the name for you."
		      << Color::def << std::endl;
            delete materials[material_code];
          }
        materials[material_code]= retval;
        tag_mat++;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; material type: '" << mat_type << "' not found."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Defines a new material.
XC::SectionGeometry *XC::MaterialHandler::newSectionGeometry(const std::string &cod)
  {
    SectionGeometry *retval= nullptr;
    if(sections_geometry.find(cod)!=sections_geometry.end()) //Section geometry already exists.
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; warning! section: '"
                  << cod << "' already exists. "<< '.' << Color::def << std::endl;
        retval= sections_geometry[cod];
       }
    else
      {
        retval= new SectionGeometry(this);
        retval->set_owner(this);
        sections_geometry[cod]= retval;
      }
    return retval;
  }

//! @brief Remove a section geometry from this container.
void XC::MaterialHandler::removeSectionGeometry(const std::string &cod)
  {
    map_geom_secc::iterator i= sections_geometry.find(cod);
    if(i!=sections_geometry.end()) //Material exists.
      {
	delete sections_geometry[cod];
	sections_geometry[cod]= nullptr;
	sections_geometry.erase(cod);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; material: '" << cod << "' not found."
	        << Color::def << std::endl;
  }

//! @brief New interaction diagram
XC::InteractionDiagram *XC::MaterialHandler::newInteractionDiagram(const std::string &cod_diag)
  {
    InteractionDiagram *retval= nullptr;
    if(interaction_diagrams.find(cod_diag)!=interaction_diagrams.end()) //Diagrams exists.
      {
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; WARNING the interaction diagram named: '"
                   << cod_diag << "' already exists. " << Color::def << std::endl;
         retval= interaction_diagrams[cod_diag];
      }
    else
      {
        retval= new InteractionDiagram();
        interaction_diagrams[cod_diag]= retval;
      }
    return retval;
  }

//! @brief Remove an interaction diagram from this container.
void XC::MaterialHandler::removeInteractionDiagram(const std::string &cod)
  {
    map_interaction_diagram::iterator i= interaction_diagrams.find(cod);
    if(i!=interaction_diagrams.end()) //Material exists.
      {
	delete interaction_diagrams[cod];
	interaction_diagrams[cod]= nullptr;
	interaction_diagrams.erase(cod);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; interaction diagram: '" << cod << "' not found."
	        << Color::def << std::endl;
  }

//! @brief New 2d interaction diagram
XC::InteractionDiagram2d *XC::MaterialHandler::new2DInteractionDiagram(const std::string &cod_diag)
  {
    InteractionDiagram2d *retval= nullptr;
    if(interaction_diagrams2D.find(cod_diag)!=interaction_diagrams2D.end()) //Diagram already exists.
      {
	std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; WARNING interaction diagram named: '"
                   << cod_diag << "' already exists. " << Color::def << std::endl;
         retval= interaction_diagrams2D[cod_diag];
      }
    else
      {
        retval= new InteractionDiagram2d();
        interaction_diagrams2D[cod_diag]= retval;
      }
    return retval;
  }

//! @brief Remove a 2D interaction diagram from this container.
void XC::MaterialHandler::remove2DInteractionDiagram(const std::string &cod)
  {
    map_interaction_diagram2d::iterator i= interaction_diagrams2D.find(cod);
    if(i!=interaction_diagrams2D.end()) //Material exists.
      {
	delete interaction_diagrams2D[cod];
	interaction_diagrams2D[cod]= nullptr;
	interaction_diagrams2D.erase(cod);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; interaction diagram: '" << cod << "' not found."
	        << Color::def << std::endl;
  }

//! @brief New interaction diagram
XC::InteractionDiagram *XC::MaterialHandler::calcInteractionDiagram(const std::string &cod_scc,const InteractionDiagramData &diag_data)
  {
    iterator mat= materials.find(cod_scc);
    InteractionDiagram *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagInt"+cod_scc;
            if(interaction_diagrams.find(cod_diag)!=interaction_diagrams.end()) //Diagram exists.
              {
	        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
			  << "; warning! redefining interaction diagram named: '"
                          << cod_diag << "'."
			  << Color::def << std::endl;
                delete interaction_diagrams[cod_diag];
              }
            else
              {
                diagI= new InteractionDiagram(calc_interaction_diagram(*tmp,diag_data));
                interaction_diagrams[cod_diag]= diagI;
              }
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; material: '" << cod_scc
                    << "' is not a fiber section material." << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; material: '"
		<< cod_scc << "' not found. Ignored."
		<< Color::def << std::endl;
    return diagI;     
  }

//! @brief New 2D interaction diagram (N-My)
XC::InteractionDiagram2d *XC::MaterialHandler::calcInteractionDiagramNMy(const std::string &cod_scc,const InteractionDiagramData &diag_data)
  {
    iterator mat= materials.find(cod_scc);
    InteractionDiagram2d *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagIntNMy"+cod_scc;
            if(interaction_diagrams2D.find(cod_diag)!=interaction_diagrams2D.end()) //Diagram exists.
              {
	        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		          << "; ¡warning! interaction diagram: '"
                          << cod_diag << "' redefined." << Color::def << std::endl;
                delete interaction_diagrams2D[cod_diag];
              }
            else
              {
                diagI= new InteractionDiagram2d(calcNMyInteractionDiagram(*tmp,diag_data));
                interaction_diagrams2D[cod_diag]= diagI;
              }
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; material: '" << cod_scc
                    << "' is not a fiber section material."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; material : '" << cod_scc
		<< "' not found. Ignored."
		<< Color::def << std::endl;
    return diagI;     
  }

//! @brief New 2D interaction diagram (N-Mz)
XC::InteractionDiagram2d *XC::MaterialHandler::calcInteractionDiagramNMz(const std::string &cod_scc,const InteractionDiagramData &diag_data)
  {
    iterator mat= materials.find(cod_scc);
    InteractionDiagram2d *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagIntNMz"+cod_scc;
            if(interaction_diagrams2D.find(cod_diag)!=interaction_diagrams2D.end()) //Diagram exists.
              {
	        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
			  << ";  warning! interaction diagram : '"
                          << cod_diag
			  << "' is being redefined."
			  << Color::def << std::endl;
                delete interaction_diagrams2D[cod_diag];
              }
            else
              {
                diagI= new InteractionDiagram2d(calcNMzInteractionDiagram(*tmp,diag_data));
                interaction_diagrams2D[cod_diag]= diagI;
              }
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; material: '" << cod_scc
                    << "' is not a fiber section material."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; material: '"
		<< cod_scc << "' not found. Ignored."
		<< Color::def << std::endl;
    return diagI;     
  }

void XC::MaterialHandler::clearAll(void)
  {
    for(iterator i= begin();i!= end();i++)
      delete (*i).second;
    materials.erase(begin(),end());
    for(geom_secc_iterator i= sections_geometry.begin();i!= sections_geometry.end();i++)
      delete (*i).second;
    sections_geometry.erase(sections_geometry.begin(),sections_geometry.end());
    tag_mat= 0;
  }

XC::MaterialHandler::~MaterialHandler(void)
  { clearAll(); }

//! @brief Returns a reference to the material container.
const XC::MaterialHandler::map_materials &XC::MaterialHandler::Map(void) const
  { return materials; }

//! @brief Returns an iterator which points to principio de la lista.
XC::MaterialHandler::const_iterator XC::MaterialHandler::begin(void) const
  { return materials.begin(); }
//! @brief Returns an iterator apuntando después of the final de la lista.
XC::MaterialHandler::const_iterator XC::MaterialHandler::end(void) const
  { return materials.end(); }
//! @brief Returns an iterator which points to principio de la lista.
XC::MaterialHandler::iterator XC::MaterialHandler::begin(void)
  { return materials.begin(); }
//! @brief Returns an iterator apuntando después of the final de la lista.
XC::MaterialHandler::iterator XC::MaterialHandler::end(void)
  { return materials.end(); }

//! @brief Si encuentra el material which name is being passed as parameter returns an iterator which points to mismo.
XC::MaterialHandler::const_iterator XC::MaterialHandler::find(const std::string &nmb) const
  { return materials.find(nmb); }

//! @brief Si encuentra el material which name is being passed as parameter returns an iterator which points to mismo.
XC::MaterialHandler::iterator XC::MaterialHandler::find(const std::string &nmb)
  { return materials.find(nmb); }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::Material *XC::MaterialHandler::find_ptr(const std::string &nmb)
  {
    XC::MaterialHandler::iterator i= find(nmb);
    if(i!= materials.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::Material *XC::MaterialHandler::find_ptr(const std::string &nmb) const
  {
    XC::MaterialHandler::const_iterator i= find(nmb);
    if(i!= materials.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::Material *XC::MaterialHandler::find_ptr(const int &tag)
  {
    Material *retval= nullptr;
    iterator i= begin();
    for(;i!= end();i++)
      if((*i).second->getTag() == tag)
        {
          retval= (*i).second;
          break;
        }
    return retval;
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::Material *XC::MaterialHandler::find_ptr(const int &tag) const
  {
    const Material *retval= nullptr;
    const_iterator i= begin();
    for(;i!= end();i++)
      if((*i).second->getTag() == tag)
        {
          retval= (*i).second;
          break;
        }
    return retval;
  }

//! @brief Returns the name that corresponds to the material tag being passed as parameter.
std::string XC::MaterialHandler::getName(const int &tag) const
  {
    std::string retval= "";
    const_iterator i= begin();
    for(;i!= end();i++)
      if((*i).second->getTag() == tag)
        {
          retval= (*i).first;
          break;
        }
    return retval;
  }

//! @brief Returns the name that corresponds to the section geometry argument.
std::string XC::MaterialHandler::getSectionGeometryName(const SectionGeometry *sg) const
  {
    std::string retval= "nil";
    const_geom_secc_iterator i= sections_geometry.begin();
    for(;i!= sections_geometry.end();i++)
      {
	if(sg==(*i).second)
	  {
	    retval= (*i).first;
	    break;
	  }
      }
    return retval; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::SectionGeometry *XC::MaterialHandler::find_ptr_section_geometry(const std::string &nmb)
  {
    geom_secc_iterator i= sections_geometry.find(nmb);
    if(i!= sections_geometry.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::SectionGeometry *XC::MaterialHandler::find_ptr_section_geometry(const std::string &nmb) const
  {
    const_geom_secc_iterator i= sections_geometry.find(nmb);
    if(i!= sections_geometry.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::InteractionDiagram *XC::MaterialHandler::find_ptr_interaction_diagram(const std::string &nmb)
  {
    interaction_diagram_iterator i= interaction_diagrams.find(nmb);
    if(i!= interaction_diagrams.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::InteractionDiagram *XC::MaterialHandler::find_ptr_interaction_diagram(const std::string &nmb) const
  {
    const_interaction_diagram_iterator i= interaction_diagrams.find(nmb);
    if(i!= interaction_diagrams.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::InteractionDiagram2d *XC::MaterialHandler::find_ptr_interaction_diagram2d(const std::string &nmb)
  {
    interaction_diagram2d_iterator i= interaction_diagrams2D.find(nmb);
    if(i!= interaction_diagrams2D.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::InteractionDiagram2d *XC::MaterialHandler::find_ptr_interaction_diagram2d(const std::string &nmb) const
  {
    const_interaction_diagram2d_iterator i= interaction_diagrams2D.find(nmb);
    if(i!= interaction_diagrams2D.end())
      return (*i).second;
    else
      return nullptr;
  }

//! @brief Returns a reference to the material which identifier
//! is being passed as parameter.
XC::Material &XC::MaterialHandler::getMaterial(const std::string &nmb)
  {
    Material *retval= find_ptr(nmb);
    assert(retval);
    return *retval;
  }

//! @brief Returns a reference to the material which identifier
//! is being passed as parameter.
XC::Material &XC::MaterialHandler::getMaterial(const int &tag)
  {
    Material *retval= find_ptr(tag);
    assert(retval);
    return *retval;
  }

//! @brief Returns a reference to the section geometry which identifier
//! is being passed as parameter. 
XC::SectionGeometry &XC::MaterialHandler::getSectionGeometry(const std::string &nmb)
  {
    SectionGeometry *retval= find_ptr_section_geometry(nmb);
    assert(retval);
    return *retval;
  }

//! @brief Returns a reference to the interaction diagram which identifier
//! is being passed as parameter.
XC::InteractionDiagram &XC::MaterialHandler::getInteractionDiagram(const std::string &nmb)
  {
    InteractionDiagram *retval= find_ptr_interaction_diagram(nmb);
    assert(retval);
    return *retval;
  }

//! @brief True if material exists.
bool XC::MaterialHandler::materialExists(const std::string &nmb) const
  { return (materials.find(nmb)!=materials.end()); }

//! @brief True if material exists.
bool XC::MaterialHandler::materialExists(const int &tag) const
  { return (this->find_ptr(tag)!=nullptr); }

//! @brief Returns true if the section geometry identified by
//! the string being passed as parameter exists.
bool XC::MaterialHandler::sectionGeometryExists(const std::string &nmb) const
  { return (sections_geometry.find(nmb)!=sections_geometry.end()); }

//! @brief Returns true if the 3D interaction diagram identified by
//! the string being passed as parameter exists.
bool XC::MaterialHandler::InteractionDiagramExists(const std::string &nmb) const
  { return (interaction_diagrams.find(nmb)!=interaction_diagrams.end()); }

//! @brief Returns true if the 2D interaction diagram identified by
//! the string being passed as parameter exists.
bool XC::MaterialHandler::InteractionDiagramExists2d(const std::string &nmb) const
  { return (interaction_diagrams2D.find(nmb)!=interaction_diagrams2D.end()); }

//! @brief Return the names of the materials.
std::set<std::string> XC::MaterialHandler::getMaterialNames(void) const
  {
    std::set<std::string> retval;    
    for(const_iterator i= begin();i!= end();i++)
      retval.insert((*i).first);
    return retval;
  }
  
//! @brief Return the names of the materials in a Python list.
boost::python::list XC::MaterialHandler::getMaterialNamesPy(void) const
  {
    boost::python::list retval;
    std::set<std::string> tmp= getMaterialNames();
    for(std::set<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }

void  XC::MaterialHandler::setCreepOn(void)
  { TDConcrete::setCreepOn(); }

void  XC::MaterialHandler::setCreepOff(void)
  { TDConcrete::setCreepOff(); }

void  XC::MaterialHandler::setCreepDt(const double &d)
  { TDConcrete::setCreepDt(d); }

double  XC::MaterialHandler::getCreepDt(void)
  { return TDConcrete::getCreepDt(); }
