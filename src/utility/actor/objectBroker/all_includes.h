
#ifndef ALL_INCLUDES_H
#define ALL_INCLUDES_H

// ActorTypes
#include "domain/domain/subdomain/ActorSubdomain.h"

// Convergence tests
#include "solution/analysis/convergenceTest/convergence_tests.h"


// graph numbering schemes
#include "solution/graph/graph/Vertex.h"
#include "solution/graph/numberer/RCM.h"
#include "solution/graph/numberer/MyRCM.h"
#include "solution/graph/numberer/SimpleNumberer.h"


// uniaxial material model header files
#include "material/uniaxial/ElasticMaterial.h"
#include "material/uniaxial/ElasticPPMaterial.h"
#include "material/uniaxial/connected/ParallelMaterial.h"
#include "material/uniaxial/connected/SeriesMaterial.h"
#include "material/uniaxial/concrete/Concrete01.h"
#include "material/uniaxial/concrete/Concrete02.h"
#include "material/uniaxial/concrete/Concrete04.h"
#include "material/uniaxial/concrete/TDConcrete.h"
#include "material/uniaxial/concrete/TDConcreteMC10.h"
#include "material/uniaxial/concrete/TDConcreteMC10NL.h"
#include "material/uniaxial/steel/Steel01.h"
#include "material/uniaxial/steel/Steel02.h"
#include "material/uniaxial/HardeningMaterial.h"
#include "material/uniaxial/HystereticMaterial.h"
#include "material/uniaxial/EPPGapMaterial.h"
#include "material/uniaxial/ViscousMaterial.h"
#include "material/uniaxial/PathIndependentMaterial.h"
#include "material/uniaxial/CableMaterial.h"
#include "material/uniaxial/ENTMaterial.h"
#include "material/uniaxial/ENCMaterial.h"
#include "material/uniaxial/TensionOnlyMaterial.h"
#include "material/uniaxial/CompressionOnlyMaterial.h"
#include "material/uniaxial/InvertMaterial.h"
#include "material/uniaxial/InitStrainMaterial.h"
#include "material/uniaxial/InitStressMaterial.h"
#include "material/uniaxial/MinMaxMaterial.h"
#include "material/uniaxial/MultiLinear.h"

//PY springs: RWBoulanger and BJeremic
#include "material/uniaxial/soil_structure_interaction/PySimple1.h"
#include "material/uniaxial/soil_structure_interaction/TzSimple1.h"
#include "material/uniaxial/soil_structure_interaction/QzSimple1.h"
#include "material/uniaxial/soil_structure_interaction/PyLiq1.h"
#include "material/uniaxial/soil_structure_interaction/TzLiq1.h"
#include "material/uniaxial/soil_structure_interaction/generators/PySimple1Gen.h"
#include "material/uniaxial/soil_structure_interaction/generators/TzSimple1Gen.h"
#include "material/uniaxial/soil_structure_interaction/EyBasic.h"
//#include "material/uniaxial/soil_structure_interaction/PySimple2.h"
//#include "material/uniaxial/soil_structure_interaction/TzSimple2.h"
//#include "material/uniaxial/soil_structure_interaction/QzSimple2.h"


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

#include "material/uniaxial/drain/DrainBilinearMaterial.h"
#include "material/uniaxial/drain/DrainClough1Material.h"
#include "material/uniaxial/drain/DrainClough2Material.h"
#include "material/uniaxial/drain/DrainPinch1Material.h"

// Sections
#include "material/section/elastic_section/ElasticSection1d.h"
#include "material/section/elastic_section/ElasticSection2d.h"
#include "material/section/elastic_section/ElasticSection3d.h"
#include "material/section/elastic_section/ElasticShearSection2d.h"
#include "material/section/elastic_section/ElasticShearSection3d.h"
#include "material/section/GenericSection1d.h"
#include "material/section/PrismaticBarCrossSection.h"
#include "material/section/repres/CrossSectionProperties2d.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "material/section/repres/section_geometry/Segment.h"
#include "material/section/repres/section_geometry/Spot.h"
#include "material/section/repres/section_geometry/Axis.h"
#include "material/section/repres/section_geometry/SectionReferenceFrame.h"
#include "material/section/repres/section_geometry/SectionCartesianReferenceFrame.h"
#include "material/section/repres/section_geometry/region/QuadCellRegion.h"
#include "material/section/repres/section_geometry/region/QuadSectRegion.h"
#include "material/section/repres/section_geometry/region/CircularSectRegion.h"
#include "material/section/repres/section_geometry/region/PolygonSectRegion.h"
#include "material/section/repres/section_geometry/region/RegionContainer.h"
#include "material/section/repres/section_geometry/reinfBar/ReinfBar.h"
#include "material/section/repres/section_geometry/reinfBar/VectorReinfBar.h"
#include "material/section/repres/section_geometry/reinfLayer/SingleBar.h"
#include "material/section/repres/section_geometry/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/section_geometry/reinfLayer/CircReinfLayer.h"
#include "material/section/repres/section_geometry/reinfLayer/ListReinfLayer.h"
#include "material/section/repres/section/SectionRepres.h"
#include "material/section/repres/section/FiberSectionRepr.h"
#include "material/section/repres/cell/QuadCell.h"
#include "material/section/repres/cell/TriangCell.h"
#include "material/section/fiber_section/fiber/FiberPtrDeque.h"
#include "material/section/fiber_section/fiber/FiberSet.h"
#include "material/section/fiber_section/fiber/FiberContainer.h"
//#include "GenericSectionNd.h"
#include "material/section/SectionAggregator.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSectionShear2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "material/section/fiber_section/FiberSectionShear3d.h"
#include "material/section/plate_section/ElasticPlateSection.h"
#include "material/section/plate_section/ElasticMembranePlateSection.h"
#include "material/section/plate_section/MembranePlateFiberSection.h"
#include "material/section/plate_section/LayeredShellFiberSection.h"
#include "material/section/interaction_diagram/DeformationPlane.h"
#include "material/section/Bidirectional.h"
#include "material/section/interaction_diagram/InteractionDiagramData.h"
#include "material/section/interaction_diagram/InteractionDiagram.h"
#include "material/section/interaction_diagram/InteractionDiagram2d.h"
#include "material/section/interaction_diagram/ComputePivots.h"

// NDMaterials
#include "material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.h"
#include "material/nD/elastic_isotropic/ElasticIsotropicAxiSymm.h"
#include "material/nD/elastic_isotropic/ElasticIsotropic3D.h"

#include "material/nD/nd_adaptor/PlaneStressMaterial.h"
#include "material/nD/nd_adaptor/PlateFiberMaterial.h"

#include "material/nD/j2_plasticity/J2PlaneStrain.h"
#include "material/nD/j2_plasticity/J2PlaneStress.h"
#include "material/nD/j2_plasticity/J2PlateFiber.h"
#include "material/nD/j2_plasticity/J2PlateFibre.h"
#include "material/nD/j2_plasticity/J2AxiSymm.h"
#include "material/nD/j2_plasticity/J2ThreeDimensional.h"
//#include "material/nD/feap/FeapMaterial03.h"

// RC multilayer.
#include "material/nD/rc_multilayer/PlateRebarMaterial.h"
#include "material/nD/rc_multilayer/PlateFromPlaneStressMaterial.h"
#include "material/nD/rc_multilayer/PlasticDamageConcretePlaneStress.h"

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

// Fibers
#include "material/section/fiber_section/fiber/UniaxialFiber2d.h"
#include "material/section/fiber_section/fiber/UniaxialFiber3d.h"

// damage models
#include "material/damage/DamageModel.h"
#include "material/damage/HystereticEnergy.h"
#include "material/damage/Mehanny.h"
#include "material/damage/ParkAng.h"
#include "material/damage/DamageResponse.h"
#include "material/damage/Kratzig.h"
#include "material/damage/NormalizedPeak.h"

// friction models
#include "domain/mesh/element/special/frictionBearing/frictionModel/CoulombFriction.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VDependentFriction.h"
#include "domain/mesh/element/special/frictionBearing/frictionModel/VPDependentFriction.h"

// physical properties
#include "domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties.h"
#include "domain/mesh/element/utils/physical_properties/ElasticSection1dPhysicalProperties.h"
#include "domain/mesh/element/utils/physical_properties/ElasticSection2dPhysicalProperties.h"
#include "domain/mesh/element/utils/physical_properties/ElasticSection3dPhysicalProperties.h"

// surface pressures
#include "domain/mesh/element/plane/surface_pressures/QuadSurfaceLoad.h"
#include "domain/mesh/element/plane/surface_pressures/BrickSurfaceLoad.h"


// element header files
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/element/utils/ParticlePos3d.h"
#include "domain/mesh/element/utils/gauss_models/GaussPoint.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d01.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d02.h"
#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include "domain/mesh/element/truss_beam_column/truss/Spring.h"
#include "domain/mesh/element/truss_beam_column/truss/TrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTruss.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h"
#include "domain/mesh/element/zero_length/ZeroLength.h"
#include "domain/mesh/element/zero_length/ZeroLengthContact2D.h"
#include "domain/mesh/element/zero_length/ZeroLengthSection.h"
//#include "ZeroLengthND.h"
#include "domain/mesh/element/plane/triangle/Tri31.h"
#include "domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.h"
#include "domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h"
#include "domain/mesh/element/plane/UP-ucsd/Nine_Four_Node_QuadUP.h"

#include "domain/mesh/element/plane/shell/ShellMITC4.h"
#include "domain/mesh/element/plane/shell/ShellMITC9.h"
#include "domain/mesh/element/plane/shell/ShellNLDKGQ.h"
#include "domain/mesh/element/plane/shell/ASDShellQ4.h"

#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam3d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h"
#include "domain/mesh/element/uw_elements/BeamContact2D.h"
#include "domain/mesh/element/uw_elements/BeamContact3D.h"

#include "domain/mesh/element/volumetric/brick/Brick.h"
#include "domain/mesh/element/volumetric/8nbrick/EightNodeBrick.h"
#include "domain/mesh/element/volumetric/20nbrick/TwentyNodeBrick.h"
#include "domain/mesh/element/volumetric/20nbrick/Twenty_Node_Brick.h"
#include "domain/mesh/element/volumetric/27nbrick/TwentySevenNodeBrick.h"
#include "domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.h"
#include "domain/mesh/element/volumetric/brick/BbarBrick.h"
#include "domain/mesh/element/volumetric/UP-ucsd/BrickUP.h"
#include "domain/mesh/element/volumetric/upU/EightNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumetric/upU/TwentyNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumetric/UP-ucsd/TwentyEightNodeBrickUP.h"

#include "domain/mesh/element/special/joint/Joint2D.h"                // Arash

//Coordinate transformation.
#include "domain/mesh/element/utils/coordTransformation/LinearCrdTransf2d.h"
#include "domain/mesh/element/utils/coordTransformation/LinearCrdTransf3d.h"
#include "domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf2d.h"
#include "domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf3d.h"
#include "domain/mesh/element/utils/coordTransformation/CorotCrdTransf2d.h"
#include "domain/mesh/element/utils/coordTransformation/CorotCrdTransf3d.h"
#include "domain/mesh/element/utils/coordTransformation/ShellLinearCrdTransf3d.h"

#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn2d.h"
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn3d.h"
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"
#include "domain/mesh/element/utils/fvectors/FVectorQuad.h"
#include "domain/mesh/element/utils/fvectors/FVectorBrick.h"

// Damping.
#include "domain/mesh/element/utils/damping/Damping.h"
#include "domain/mesh/element/utils/damping/SecStifDamping.h"
#include "domain/mesh/element/utils/damping/UDampingBase.h"
#include "domain/mesh/element/utils/damping/UniformDamping.h"
#include "domain/mesh/element/utils/damping/URDDampingbeta.h"
#include "domain/mesh/element/utils/damping/URDDamping.h"

#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeEndpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/DistHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/TrapezoidalBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/FixedLocationBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LowOrderBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/MidDistanceBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/CompositeSimpsonBeamIntegration.h"

// node header files
#include "domain/mesh/node/Node.h"


#include "utility/handler/DataOutputStreamHandler.h"
#include "utility/handler/DataOutputFileHandler.h"
#include "utility/handler/DataOutputDatabaseHandler.h"

#include "utility/recorder/NodeRecorder.h"
#include "utility/recorder/ElementRecorder.h"
#include "utility/recorder/PropRecorder.h"
#include "utility/recorder/NodePropRecorder.h"
#include "utility/recorder/ElementPropRecorder.h"
#include "utility/recorder/EnvelopeNodeRecorder.h"
#include "utility/recorder/EnvelopeElementRecorder.h"
#include "utility/recorder/response/Response.h"
#include "utility/recorder/response/ElementResponse.h"


// sp_constraint header files
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/SFreedom_ConstraintIter.h"
#include "domain/constraints/single_freedom_constraints/ImposedMotionSP.h"
#include "domain/constraints/single_freedom_constraints/ImposedMotionSP1.h"

// mp_constraint header files
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/constraints/MFreedom_ConstraintIter.h"
#include "domain/constraints/single_retained_node_constraints/MFreedom_Joint2D.h"
#include "domain/constraints/single_retained_node_constraints/MFreedom_Joint3D.h"
#include "domain/constraints/single_retained_node_constraints/RigidBase.h"
#include "domain/constraints/single_retained_node_constraints/RigidBeam.h"
#include "domain/constraints/single_retained_node_constraints/RigidRod.h"
#include "domain/constraints/single_retained_node_constraints/RigidDiaphragm.h"
#include "domain/constraints/single_retained_node_constraints/EqualDOF.h"
#include "domain/constraints/MRMFreedom_Constraint.h"
#include "domain/constraints/MRMFreedom_ConstraintIter.h"
#include "domain/constraints/multiple_retained_node_constraints/GlueNodeToElement.h"
#include "domain/constraints/skew_constraints/Skew_Constraint.h"
#include "domain/constraints/skew_constraints/SkewPlane.h"
#include "domain/constraints/skew_constraints/SymmetryConstraint.h"
#include "domain/constraints/skew_constraints/AntiSymmetryConstraint.h"

#include "domain/constraints/MapActiveLoadPatterns.h"
#include "domain/constraints/ConstrContainer.h"

// nodal load header files
#include "domain/load/NodalLoad.h"
#include "domain/load/NodalLoadIter.h"

// elemental load header files
#include "domain/load/ElementalLoad.h"
#include "domain/load/ElementalLoadIter.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPartialUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "domain/load/plane/QuadRawLoad.h"
#include "domain/load/plane/QuadStrainLoad.h"
#include "domain/load/plane/ShellRawLoad.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "domain/load/volumetric/BrickStrainLoad.h"
#include "domain/load/volumetric/BrickRawLoad.h"
#include "domain/load/SurfaceLoad.h"
#include "domain/load/volumetric/BrickSelfWeight.h"
#include "domain/load/pattern/NodeLocker.h"
#include "domain/load/pattern/NodeLockerIter.h"
#include "domain/mesh/NodeLockers.h"

// matrix, vector & id header files
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/nDarray/nDarray.h"
#include "utility/matrix/nDarray/BJmatrix.h"
#include "utility/matrix/nDarray/BJvector.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/matrix/nDarray/straint.h"
#include "utility/matrix/nDarray/stresst.h"
#include "utility/matrix/nDarray/Cosseratstraint.h"
#include "utility/matrix/nDarray/Cosseratstresst.h"

// Paving algorithm
#include "utility/paving/Paver.h"


// subdomain header files
#include "domain/domain/subdomain/Subdomain.h"

// constraint handler header files
#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/handler/PlainHandler.h"
#include "solution/analysis/handler/PenaltyConstraintHandler.h"
#include "solution/analysis/handler/LagrangeConstraintHandler.h"
#include "solution/analysis/handler/TransformationConstraintHandler.h"
#include "solution/analysis/handler/AutoConstraintHandler.h"

// dof numberer header files
#include "solution/analysis/numberer/DOF_Numberer.h"
#include "solution/analysis/numberer/PlainNumberer.h"

// analysis model header files
#include "solution/analysis/model/AnalysisModel.h"
#include "solution/analysis/ModelWrapper.h"
#include "solution/analysis/analysis/Analysis.h"
#include "solution/analysis/numberer/DOF_Numberer.h"

// equi soln algo header files
#include "solution/analysis/algorithm/solution_algorithms.h"

//#include <KrylovAccelerator.h>
//#include <RaphsonAccelerator.h>

// domain decomp soln algo header files
#include "solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h"

// integrator header files
#include "solution/analysis/integrator/integrators.h"

// system of eqn header files
#include "solution/system_of_eqn/systems_of_equations.h"

// Analysis types
#include "solution/analysis/analysis/analysis.h"

// load patterns
#include "domain/load/pattern/LoadPattern.h"
#include "domain/load/pattern/LoadCombination.h"
#include "domain/load/pattern/LoadCombinationGroup.h"
#include "domain/load/pattern/load_patterns/UniformExcitation.h"
#include "domain/load/pattern/load_patterns/MultiSupportPattern.h"
#include "domain/load/pattern/load_patterns/PBowlLoading.h"
#include "domain/load/groundMotion/GroundMotion.h"
#include "domain/load/groundMotion/GroundMotionRecord.h"
#include "domain/load/groundMotion/InterpolatedGroundMotion.h"
#include "domain/component/Parameter.h"
#include "domain/component/MatParameter.h"

/* #include <MaterialStageParameter.h> */

// time series
#include "domain/load/pattern/time_series/LinearSeries.h"
#include "domain/load/pattern/time_series/PathSeries.h"
#include "domain/load/pattern/time_series/PathTimeSeries.h"
#include "domain/load/pattern/time_series/PathSeriesBase.h"
#include "domain/load/pattern/time_series/RectangularSeries.h"
#include "domain/load/pattern/time_series/ConstantSeries.h"
#include "domain/load/pattern/time_series/TrigSeries.h"
#include "domain/load/pattern/time_series/ConstantSeries.h"
#include "domain/load/pattern/time_series/DiscretizedRandomProcessSeries.h"
#include "domain/load/pattern/time_series/LinearSeries.h"
#include "domain/load/pattern/time_series/PulseSeries.h"
#include "domain/load/pattern/time_series/SimulatedRandomProcessSeries.h"
#include "domain/load/pattern/time_series/TriangleSeries.h"

// time series integrators
#include "domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.h"


#include "preprocessor/multi_block_topology/ReferenceFrame.h"
#include "preprocessor/multi_block_topology/CartesianReferenceFrame3d.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Line.h"
#include "preprocessor/multi_block_topology/entities/1d/DividedLine.h"
#include "preprocessor/multi_block_topology/entities/1d/CircularArc.h"
#include "preprocessor/multi_block_topology/entities/2d/QuadSurface.h"
#include "preprocessor/multi_block_topology/entities/2d/PolygonalFace.h"
#include "preprocessor/multi_block_topology/entities/3d/UniformGrid.h"
#include "preprocessor/multi_block_topology/entities/3d/Body.h"
#include "preprocessor/multi_block_topology/entities/3d/Block.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray.h"
#include "preprocessor/multi_block_topology/matrices/NodePtrArray.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/NodePtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/Framework2d.h"
#include "preprocessor/multi_block_topology/matrices/Framework3d.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "preprocessor/multi_block_topology/trf/Translation.h"
#include "preprocessor/multi_block_topology/trf/Reflection.h"
#include "preprocessor/multi_block_topology/trf/Scaling.h"
#include "preprocessor/multi_block_topology/trf/Rotation.h"
#include "preprocessor/set_mgmt/Set.h"
#include "preprocessor/set_mgmt/IRowSet.h"
#include "preprocessor/set_mgmt/JRowSet.h"
#include "preprocessor/set_mgmt/KRowSet.h"


#ifdef _PARALLEL_PROCESSING
#include "solution/analysis/numberer/ParallelNumberer.h"
#include "solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h"
#include "solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h"
#endif

#include "utility/package/packages.h"
#include "utility/matrix/ID.h"
#include "utility/xc_python_utils.h"
#include "utility/database/SQLiteDatastore.h"
#include "utility/database/PyDictDatastore.h"
#include "utility/database/OracleDatastore.h"
#include "utility/database/BerkeleyDbDatastore.h"
#include "utility/database/NEESData.h"
#include "utility/database/MySqlDatastore.h"
#include "utility/database/FileDatastore.h"

#endif
