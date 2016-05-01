//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MaterialLoader.cc

#include "MaterialLoader.h"
#include "domain/domain/Domain.h"


//Uniaxial material.
#include "material/uniaxial/BarSlipMaterial.h"
//#include "material/uniaxial/BoucWenMaterial.h"
#include "material/uniaxial/CableMaterial.h"
#include "material/uniaxial/concrete/Concrete01.h"
#include "material/uniaxial/concrete/Concrete04.h"
#include "material/uniaxial/ENTMaterial.h"
#include "material/uniaxial/EPPGapMaterial.h"
#include "material/uniaxial/ElasticMaterial.h"
#include "material/uniaxial/ElasticPPMaterial.h"
#include "material/uniaxial/FatigueMaterial.h"
#include "material/uniaxial/HardeningMaterial.h"
#include "material/uniaxial/HystereticMaterial.h"
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

#include "material/uniaxial/PY/PySimple1.h"
#include "material/uniaxial/PY/QzSimple1.h"
#include "material/uniaxial/PY/TzSimple1.h"
#include "material/uniaxial/PY/PyLiq1.h"
#include "material/uniaxial/PY/TzLiq1.h"

#include "material/uniaxial/snap/Bilinear.h"
#include "material/uniaxial/snap/Clough.h"
#include "material/uniaxial/snap/CloughDamage.h"
#include "material/uniaxial/snap/Pinching.h"
#include "material/uniaxial/snap/PinchingDamage.h"


//nDMaterial
#include "material/nD/NDMaterial.h"
#include "material/nD/Template3Dep/Template3Dep.h"

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

//Section
#include "material/section/Bidirectional.h"
#include "material/section/elastic_section/ElasticSection2d.h"
#include "material/section/elastic_section/ElasticShearSection2d.h"
#include "material/section/elastic_section/ElasticSection3d.h"
#include "material/section/elastic_section/ElasticShearSection3d.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "material/section/fiber_section/FiberSectionShear3d.h"
#include "material/section/GenericSection1d.h"
#include "material/section/GenericSectionNd.h"
#include "material/section/Isolator2spring.h"
#include "material/section/SectionAggregator.h"
#include "material/section/yieldSurface/YS_Section2D02.h"
#include "material/section/yieldSurface/YS_Section2D01.h"
#include "material/section/diag_interaccion/DatosDiagInteraccion.h"
#include "material/section/repres/geom_section/GeomSection.h"
#include "material/section/diag_interaccion/DiagInteraccion.h"
#include "material/section/diag_interaccion/DiagInteraccion2d.h"

//Plate section.
#include "material/section/plate_section/ElasticPlateSection.h"
#include "material/section/plate_section/ElasticMembranePlateSection.h"
#include "material/section/plate_section/MembranePlateFiberSection.h"

//Yield surface
#include "material/yieldSurface/plasticHardeningMaterial/ExponReducing.h"
#include "material/yieldSurface/plasticHardeningMaterial/MultiLinearKp.h"
#include "material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h"

//! @brief Constructor por defecto.
XC::MaterialLoader::MaterialLoader(Preprocessor *owr)
  : Loader(owr), tag_mat(0) {}

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

//! @brief Procesa los comandos que se emplean para definir
//! materiales uniaxiales para su empleo en el modelo de
//! elementos finitos. Interpreta los siguientes comandos:
//!
//! - elastic_material: Define un material uniaxial elástico lineal.
//! - elasticpp_material: Define un material uniaxial elastoplástico perfecto.
//! - epp_gap_material: Define una material uniaxial elastoplástico perfecto con «gap».
//! - cable_material: Define un material uniaxial de tipo «cable» sin resistencia a compresión.
//! - concrete01_material: Define un material uniaxial para representar el comportamiento del hormigón.
//! - concrete04_material: Define un material uniaxial para representar el comportamiento del hormigón.
//! - steel01: Define un material uniaxial para representar el comportamiento del acero.
//! - steel02: Define un material uniaxial para representar el comportamiento del acero.
//! - steel03: Define un material uniaxial para representar el comportamiento del acero.
XC::Material *load_uniaxial_material(int tag_mat,const std::string &cmd)
  {
    XC::Material *retval= nullptr;
    if(cmd == "bar_slip_material")
      retval= new XC::BarSlipMaterial(tag_mat);
    else if(cmd == "cable_material")
      retval= new XC::CableMaterial(tag_mat);
    else if(cmd == "concrete01_material")
      retval= new XC::Concrete01(tag_mat);
    else if(cmd == "concrete04_material")
      retval= new XC::Concrete04(tag_mat);
    else if(cmd == "elast_no_trac_material")
      retval= new XC::ENTMaterial(tag_mat);
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
    else if(cmd == "viscous_material")
      retval= new XC::ViscousMaterial(tag_mat);
    return retval;
  }

//! @brief Procesa los comandos que se emplean para definir
//! materiales nD para su empleo en el modelo de
//! elementos finitos. Interpreta los siguientes comandos:
//!
//! - elastic_cross_anisotropic: Define un material elástico anisótropo.
//! - elastic_isotropic_3d: Define un material elástico isótropo.
//! - elastic_isotropic_plane_stress_2d: Define un material
//!   elástico isótropo para problemas de tensión plana.
//! - elastic_isotropic_plane_strain_2d: Define un material
//!   elástico isótropo para problemas de deformación plana.
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

//! @brief Procesa los comandos que se emplean para definir
//! materiales para láminas. Interpreta los siguientes comandos:
//!
//! - elastic_membrane_plate_section: Define un material elástico para problemas de láminas.
//! - elastic_plate_section: Define un material elástico para problemas de placas.
XC::Material *load_plate_section_material(int tag_mat,const std::string &cmd,XC::MaterialLoader *mloader)
  {
    XC::Material *retval= nullptr;
    if(cmd == "elastic_membrane_plate_section")
      retval= new XC::ElasticMembranePlateSection(tag_mat);
    else if(cmd == "elastic_plate_section")
      retval= new XC::ElasticPlateSection(tag_mat);
    else if(cmd == "membrane_plate_fiber_section")
      retval= new XC::MembranePlateFiberSection(tag_mat);
    return retval;
  }

//! @brief Procesa los comandos que se emplean para definir
//! materiales de tipo sección para su empleo en el modelo de
//! elementos finitos. Interpreta los siguientes comandos:
//!
//! - elastic_section_2d: Define una sección de material elástico para problemas planos.
//! - elastic_shear_section_2d: Define una sección de material elástico
//!   con rigidez a cortante para problemas planos.
//! - elastic_section_3d: Define una sección de material elástico para problemas tridimensionales.
//! - elastic_shear_section_3d: Define una sección de material elástico
//!   con rigidez a cortante para problemas tridimensionales.
//! - fiber_section_2d: Define un modelo de fibras para una sección (problemas planos).
//! - fiber_section_3d: Define un modelo de fibras para una sección (problemas tridimensionales).
//! - fiber_section_GJ: Define un modelo de fibras para una sección con rigidez
//!    a torsión (problemas tridimensionales).
//! - section_aggregator: Agrega propiedades mecánicas a una sección.
XC::Material *load_section_material(int tag_mat,const std::string &cmd,XC::MaterialLoader *mloader)
  {
    XC::Material *retval= nullptr;
    if(cmd == "bidirectional_section")
      retval= new XC::Bidirectional(tag_mat);
    else if(cmd == "elastic_section_2d")
      retval= new XC::ElasticSection2d(tag_mat,mloader);
    else if(cmd == "elasticShearSection2d")
      retval= new XC::ElasticShearSection2d(tag_mat,mloader);
    else if(cmd == "elastic_section_3d")
      retval= new XC::ElasticSection3d(tag_mat,mloader);
    else if(cmd == "elasticShearSection3d")
      retval= new XC::ElasticShearSection3d(tag_mat,mloader);
    else if(cmd == "fiber_section_2d")
      retval= new XC::FiberSection2d(tag_mat,mloader);
    else if(cmd == "fiber_section_3d")
      retval= new XC::FiberSection3d(tag_mat,mloader);
    else if(cmd == "fiber_section_GJ")
      retval= new XC::FiberSectionGJ(tag_mat,mloader);
    else if(cmd == "fiberSectionShear3d")
      retval= new XC::FiberSectionShear3d(tag_mat,mloader);
    else if(cmd == "generic_section_1d")
      retval= new XC::GenericSection1d(tag_mat);
    else if(cmd == "isolator2spring")
      retval= new XC::Isolator2spring(tag_mat);
    else if(cmd == "section_aggregator")
      retval= new XC::SectionAggregator(tag_mat,mloader);
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
XC::Material *load_material(int tag_mat,const std::string &cmd,XC::MaterialLoader *mloader)
  {
    XC::Material *retval= load_uniaxial_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_uniaxial_fedeas_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_uniaxial_py_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_uniaxial_snap_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_nD_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_nD_finite_deformation_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_nD_cyclic_soil_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_nD_soil_material(tag_mat,cmd);
    if(retval)
      return retval;
    retval= load_section_material(tag_mat,cmd,mloader);
    if(retval)
      return retval;
    retval= load_plate_section_material(tag_mat,cmd,mloader);
    if(retval)
      return retval;
    retval= load_yield_surf_plastic_hardening(tag_mat,cmd);
    if(retval)
      return retval;
    return retval;
  }

//! @brief Define un nuevo material.
XC::Material *XC::MaterialLoader::nuevoMaterial(const std::string &cmd,const std::string &cod_mat)
  {
    Material *retval= load_material(tag_mat,cmd,this);
    if(retval)
      {
        retval->set_owner(this);
        if(materials.find(cod_mat)!=materials.end()) //El material existe.
          {
	    std::clog << "MaterialLoader::nuevo_material; ¡ojo! se redefine el material: '"
                      << cod_mat << "'." << std::endl;
            delete materials[cod_mat];
          }
        materials[cod_mat]= retval;
        tag_mat++;
      }
    return retval;
  }

//! @brief Define un nuevo material.
XC::GeomSection *XC::MaterialLoader::newSectionGeometry(const std::string &cod)
  {
    XC::GeomSection *retval= nullptr;
    if(geom_secciones.find(cod)!=geom_secciones.end()) //La geometria de la sección existe.
      {
	std::cerr << "MaterialLoader::newSectionGeometry; warning! section: '"
                      << cod << "' already exists. "<< '.' << std::endl;
        retval= geom_secciones[cod];
       }
    else
      {
        retval= new GeomSection(this);
        retval->set_owner(this);
        geom_secciones[cod]= retval;
      }
    return retval;
  }

//! @brief New interaction diagram
XC::DiagInteraccion *XC::MaterialLoader::newInteractionDiagram(const std::string &cod_diag)
  {
    DiagInteraccion *retval= nullptr;
    if(diagramas_interaccion.find(cod_diag)!=diagramas_interaccion.end()) //El diagrama existe.
      {
         std::clog << "MaterialLoader::newInteractionDiagram; ¡ojo! el diagrama de interacción de nombre: '"
                   << cod_diag << "' ya existe. " << std::endl;
         retval= diagramas_interaccion[cod_diag];
      }
    else
      {
        retval= new DiagInteraccion();
        diagramas_interaccion[cod_diag]= retval;
      }
    return retval;
  }

//! @brief New 2d interaction diagram
XC::DiagInteraccion2d *XC::MaterialLoader::new2DInteractionDiagram(const std::string &cod_diag)
  {
    DiagInteraccion2d *retval= nullptr;
    if(diagramas_interaccion2d.find(cod_diag)!=diagramas_interaccion2d.end()) //El diagrama existe.
      {
         std::clog << "MaterialLoader::new2DInteractionDiagram; ¡ojo! el diagrama de interacción de nombre: '"
                   << cod_diag << "' ya existe. " << std::endl;
         retval= diagramas_interaccion2d[cod_diag];
      }
    else
      {
        retval= new DiagInteraccion2d();
        diagramas_interaccion2d[cod_diag]= retval;
      }
    return retval;
  }

//! @brief New interaction diagram
XC::DiagInteraccion *XC::MaterialLoader::calcInteractionDiagram(const std::string &cod_scc,const DatosDiagInteraccion &datos_diag)
  {
    iterator mat= materials.find(cod_scc);
    DiagInteraccion *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagInt"+cod_scc;
            if(diagramas_interaccion.find(cod_diag)!=diagramas_interaccion.end()) //Diagram exists.
              {
	        std::clog << "MaterialLoader::calcInteractionDiagram; ¡ojo! se redefine el diagrama de interacción de nombre: '"
                          << cod_diag << "'." << std::endl;
                delete diagramas_interaccion[cod_diag];
              }
            else
              {
                diagI= new DiagInteraccion(calc_diag_interaccion(*tmp,datos_diag));
                diagramas_interaccion[cod_diag]= diagI;
              }
          }
        else
          std::cerr << "El material: '" << cod_scc
                    << "' no corresponde a una sección de fibras." << std::endl;
      }
    else
      std::cerr << "No se encontró el material : '"
                      << cod_scc << "' se ignora la entrada.\n";
    return diagI;     
  }

//! @brief New 2D interaction diagram (N-My)
XC::DiagInteraccion2d *XC::MaterialLoader::calcInteractionDiagramNMy(const std::string &cod_scc,const DatosDiagInteraccion &datos_diag)
  {
    iterator mat= materials.find(cod_scc);
    DiagInteraccion2d *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagIntNMy"+cod_scc;
            if(diagramas_interaccion2d.find(cod_diag)!=diagramas_interaccion2d.end()) //Diagram exists.
              {
	        std::clog << "MaterialLoader::calcInteractionDiagramNMy; ¡ojo! se redefine el diagrama de interacción de nombre: '"
                          << cod_diag << "'." << std::endl;
                delete diagramas_interaccion2d[cod_diag];
              }
            else
              {
                diagI= new DiagInteraccion2d(calc_diag_interaccionNMy(*tmp,datos_diag));
                diagramas_interaccion2d[cod_diag]= diagI;
              }
          }
        else
          std::cerr << "El material: '" << cod_scc
                    << "' no corresponde a una sección de fibras." << std::endl;
      }
    else
      std::cerr << "No se encontró el material : '"
                      << cod_scc << "' se ignora la entrada.\n";
    return diagI;     
  }

//! @brief New 2D interaction diagram (N-Mz)
XC::DiagInteraccion2d *XC::MaterialLoader::calcInteractionDiagramNMz(const std::string &cod_scc,const DatosDiagInteraccion &datos_diag)
  {
    iterator mat= materials.find(cod_scc);
    DiagInteraccion2d *diagI= nullptr;
    if(mat!=materials.end())
      {
        const FiberSectionBase *tmp= dynamic_cast<const FiberSectionBase *>(mat->second);
        if(tmp)
          {
            const std::string cod_diag= "diagIntNMz"+cod_scc;
            if(diagramas_interaccion2d.find(cod_diag)!=diagramas_interaccion2d.end()) //Diagram exists.
              {
	        std::clog << "MaterialLoader::calcInteractionDiagramNMz; ¡ojo! se redefine el diagrama de interacción de nombre: '"
                          << cod_diag << "'." << std::endl;
                delete diagramas_interaccion2d[cod_diag];
              }
            else
              {
                diagI= new DiagInteraccion2d(calc_diag_interaccionNMz(*tmp,datos_diag));
                diagramas_interaccion2d[cod_diag]= diagI;
              }
          }
        else
          std::cerr << "El material: '" << cod_scc
                    << "' no corresponde a una sección de fibras." << std::endl;
      }
    else
      std::cerr << "No se encontró el material : '"
                      << cod_scc << "' se ignora la entrada.\n";
    return diagI;     
  }

void XC::MaterialLoader::clearAll(void)
  {
    for(iterator i= begin();i!= end();i++)
      delete (*i).second;
    materials.erase(begin(),end());
    for(geom_secc_iterator i= geom_secciones.begin();i!= geom_secciones.end();i++)
      delete (*i).second;
    geom_secciones.erase(geom_secciones.begin(),geom_secciones.end());
    tag_mat= 0;
  }

XC::MaterialLoader::~MaterialLoader(void)
  { clearAll(); }

//! @brief Devuelve una referencia al mapa de materials.
const XC::MaterialLoader::map_materials &XC::MaterialLoader::Map(void) const
  { return materials; }

//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::MaterialLoader::const_iterator XC::MaterialLoader::begin(void) const
  { return materials.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::MaterialLoader::const_iterator XC::MaterialLoader::end(void) const
  { return materials.end(); }
//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::MaterialLoader::iterator XC::MaterialLoader::begin(void)
  { return materials.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::MaterialLoader::iterator XC::MaterialLoader::end(void)
  { return materials.end(); }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::MaterialLoader::const_iterator XC::MaterialLoader::find(const std::string &nmb) const
  { return materials.find(nmb); }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::MaterialLoader::iterator XC::MaterialLoader::find(const std::string &nmb)
  { return materials.find(nmb); }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::Material *XC::MaterialLoader::find_ptr(const std::string &nmb)
  {
    XC::MaterialLoader::iterator i= find(nmb);
    if(i!= materials.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::Material *XC::MaterialLoader::find_ptr(const std::string &nmb) const
  {
    XC::MaterialLoader::const_iterator i= find(nmb);
    if(i!= materials.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::Material *XC::MaterialLoader::find_ptr(const int &tag)
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

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::Material *XC::MaterialLoader::find_ptr(const int &tag) const
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

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::GeomSection *XC::MaterialLoader::find_ptr_geom_section(const std::string &nmb)
  {
    geom_secc_iterator i= geom_secciones.find(nmb);
    if(i!= geom_secciones.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::GeomSection *XC::MaterialLoader::find_ptr_geom_section(const std::string &nmb) const
  {
    const_geom_secc_iterator i= geom_secciones.find(nmb);
    if(i!= geom_secciones.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::DiagInteraccion *XC::MaterialLoader::find_ptr_diag_interaccion(const std::string &nmb)
  {
    diag_interacc_iterator i= diagramas_interaccion.find(nmb);
    if(i!= diagramas_interaccion.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::DiagInteraccion *XC::MaterialLoader::find_ptr_diag_interaccion(const std::string &nmb) const
  {
    const_diag_interacc_iterator i= diagramas_interaccion.find(nmb);
    if(i!= diagramas_interaccion.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::DiagInteraccion2d *XC::MaterialLoader::find_ptr_diag_interaccion2d(const std::string &nmb)
  {
    diag_interacc2d_iterator i= diagramas_interaccion2d.find(nmb);
    if(i!= diagramas_interaccion2d.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el material cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::DiagInteraccion2d *XC::MaterialLoader::find_ptr_diag_interaccion2d(const std::string &nmb) const
  {
    const_diag_interacc2d_iterator i= diagramas_interaccion2d.find(nmb);
    if(i!= diagramas_interaccion2d.end())
      return (*i).second;
    else
      return nullptr;
  }

//! @brief Devuelve una referencia al material cuyo nombre se pasa como parámetro.
XC::Material &XC::MaterialLoader::getMaterial(const std::string &nmb)
  {
    Material *retval= find_ptr(nmb);
    assert(retval);
    return *retval;
  }

//! @brief Devuelve una referencia a la geometría de sección cuyo nombre se pasa como parámetro.
XC::GeomSection &XC::MaterialLoader::getGeomSection(const std::string &nmb)
  {
    GeomSection *retval= find_ptr_geom_section(nmb);
    assert(retval);
    return *retval;
  }

//! @brief Devuelve una referencia al diagrama de interacción cuyo nombre se pasa como parámetro.
XC::DiagInteraccion &XC::MaterialLoader::getDiagInteraccion(const std::string &nmb)
  {
    DiagInteraccion *retval= find_ptr_diag_interaccion(nmb);
    assert(retval);
    return *retval;
  }

//! @brief True if material exists.
bool XC::MaterialLoader::existeMaterial(const std::string &nmb) const
  { return (materials.find(nmb)!=materials.end()); }

//! @brief Devuelve verdadero si existe la geometría de la sección cuyo código se pasa como parámetro.
bool XC::MaterialLoader::existeGeomSection(const std::string &nmb) const
  { return (geom_secciones.find(nmb)!=geom_secciones.end()); }

//! @brief Devuelve verdadero si existe el diagrama de interacción cuyo código se pasa como parámetro.
bool XC::MaterialLoader::existeDiagInteraccion(const std::string &nmb) const
  { return (diagramas_interaccion.find(nmb)!=diagramas_interaccion.end()); }

//! @brief Devuelve verdadero si existe el diagrama de interacción cuyo código se pasa como parámetro.
bool XC::MaterialLoader::existeDiagInteraccion2d(const std::string &nmb) const
  { return (diagramas_interaccion2d.find(nmb)!=diagramas_interaccion2d.end()); }

