
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
#include "material/uniaxial/steel/Steel01.h"
#include "material/uniaxial/steel/Steel02.h"
#include "material/uniaxial/HardeningMaterial.h"
#include "material/uniaxial/HystereticMaterial.h"
#include "material/uniaxial/EPPGapMaterial.h"
#include "material/uniaxial/ViscousMaterial.h"
#include "material/uniaxial/PathIndependentMaterial.h"
#include "material/uniaxial/CableMaterial.h"
#include "material/uniaxial/ENTMaterial.h"
#include "material/uniaxial/MinMaxMaterial.h"

//PY springs: RWBoulanger and BJeremic
#include "material/uniaxial/PY/PySimple1.h"
#include "material/uniaxial/PY/TzSimple1.h"
#include "material/uniaxial/PY/QzSimple1.h"
#include "material/uniaxial/PY/PyLiq1.h"
#include "material/uniaxial/PY/TzLiq1.h"
//#include "material/uniaxial/PY/PySimple2.h"
//#include "material/uniaxial/PY/TzSimple2.h"
//#include "material/uniaxial/PY/QzSimple2.h"


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
#include "material/section/elastic_section/ElasticSection2d.h"
#include "material/section/elastic_section/ElasticSection3d.h"
#include "material/section/elastic_section/ElasticShearSection2d.h"
#include "material/section/elastic_section/ElasticShearSection3d.h"
#include "material/section/GenericSection1d.h"
#include "material/section/SeccionBarraPrismatica.h"
#include "material/section/repres/ConstantesSecc2d.h"
#include "material/section/repres/ConstantesSecc3d.h"
#include "material/section/repres/geom_section/GeomSection.h"
#include "material/section/repres/geom_section/Segment.h"
#include "material/section/repres/geom_section/Spot.h"
#include "material/section/repres/geom_section/Eje.h"
#include "material/section/repres/geom_section/SisRefScc.h"
#include "material/section/repres/geom_section/SisRefSccCartesianas2d.h"
#include "material/section/repres/geom_section/region/RgQuadCell.h"
#include "material/section/repres/geom_section/region/RgSccCuadrilatero.h"
#include "material/section/repres/geom_section/region/RgSccCirc.h"
#include "material/section/repres/geom_section/region/RgSccPoligono.h"
#include "material/section/repres/geom_section/region/ListRegiones.h"
#include "material/section/repres/geom_section/reinfBar/ReinfBar.h"
#include "material/section/repres/geom_section/reinfBar/VectorReinfBar.h"
#include "material/section/repres/geom_section/reinfLayer/BarraSuelta.h"
#include "material/section/repres/geom_section/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/CircReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/ListReinfLayer.h"
#include "material/section/repres/section/SectionRepres.h"
#include "material/section/repres/section/FiberSectionRepr.h"
#include "material/section/repres/cell/QuadCell.h"
#include "material/section/repres/cell/TriangCell.h"
#include "material/section/fiber_section/fiber/DqFibras.h"
//#include "GenericSectionNd.h"
#include "material/section/SectionAggregator.h"
#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "material/section/fiber_section/FiberSectionShear3d.h"
#include "material/section/plate_section/ElasticPlateSection.h"
#include "material/section/plate_section/ElasticMembranePlateSection.h"
#include "material/section/plate_section/MembranePlateFiberSection.h"
#include "material/section/diag_interaccion/PlanoDeformacion.h"
#include "material/section/Bidirectional.h"
#include "material/section/diag_interaccion/DatosDiagInteraccion.h"
#include "material/section/diag_interaccion/DiagInteraccion.h"
#include "material/section/diag_interaccion/DiagInteraccion2d.h"
#include "material/section/diag_interaccion/CalcPivotes.h"

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
#include "material/nD/j2_plasticity/J2AxiSymm.h"
#include "material/nD/j2_plasticity/J2ThreeDimensional.h"
//#include "material/nD/feap/FeapMaterial03.h"

#include "material/nD/soil/FluidSolidPorousMaterial.h"
#include "material/nD/soil/PressureDependMultiYield.h"
#include "material/nD/soil/PressureIndependMultiYield.h"

// Fibers
#include "material/section/fiber_section/fiber/UniaxialFiber2d.h"
#include "material/section/fiber_section/fiber/UniaxialFiber3d.h"

// friction models
#include "domain/mesh/element/frictionBearing/frictionModel/CoulombFriction.h"
#include "domain/mesh/element/frictionBearing/frictionModel/VDependentFriction.h"
#include "domain/mesh/element/frictionBearing/frictionModel/VPDependentFriction.h"

// element header files
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/element/ParticlePos3d.h"
#include "domain/mesh/element/gauss_models/GaussPoint.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d01.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d02.h"
#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include "domain/mesh/element/truss_beam_column/truss/Muelle.h"
#include "domain/mesh/element/truss_beam_column/truss/TrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTruss.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h"
#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include "domain/mesh/element/zeroLength/ZeroLengthContact2D.h"
#include "domain/mesh/element/zeroLength/ZeroLengthSection.h"
//#include "ZeroLengthND.h"
#include "domain/mesh/element/plane/triangle/Tri31.h"
#include "domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.h"
#include "domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h"
#include "domain/mesh/element/plane/UP-ucsd/NineFourNodeQuadUP.h"

#include "domain/mesh/element/plane/shell/ShellLinearCrdTransf3d.h"
#include "domain/mesh/element/plane/shell/ShellCorotCrdTransf3d.h"
#include "domain/mesh/element/plane/shell/ShellMITC4.h"
#include "domain/mesh/element/plane/shell/CorotShellMITC4.h"
#include "domain/mesh/element/plane/shell/ShellNL.h"

#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h"

#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h"

#include "domain/mesh/element/volumen/brick/Brick.h"
#include "domain/mesh/element/volumen/brick/BbarBrick.h"
#include "domain/mesh/element/volumen/UP-ucsd/BrickUP.h"
#include "domain/mesh/element/volumen/upU/EightNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/upU/TwentyNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/UP-ucsd/TwentyEightNodeBrickUP.h"

#include "domain/mesh/element/joint/Joint2D.h"                // Arash

//Transformaciones de coordenadas.
#include "domain/mesh/element/coordTransformation/LinearCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/LinearCrdTransf3d.h"
#include "domain/mesh/element/coordTransformation/PDeltaCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/PDeltaCrdTransf3d.h"
#include "domain/mesh/element/coordTransformation/CorotCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/CorotCrdTransf3d.h"

#include "domain/mesh/element/fvectors/FVectorBeamColumn2d.h"
#include "domain/mesh/element/fvectors/FVectorBeamColumn3d.h"
#include "domain/mesh/element/fvectors/FVectorShell.h"


#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeEndpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/DistHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration.h"

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

#include "utility/med_xc/MEDBaseInfo.h"
#include "utility/med_xc/MEDCellBaseInfo.h"
#include "utility/med_xc/MEDCellInfo.h"
#include "utility/med_xc/MEDDblFieldInfo.h"
#include "utility/med_xc/MEDFieldInfo.h"
#include "utility/med_xc/MEDGaussModel.h"
#include "utility/med_xc/MEDGroupInfo.h"
#include "utility/med_xc/MEDIntFieldInfo.h"
#include "utility/med_xc/MEDMapConectividad.h"
#include "utility/med_xc/MEDMapIndices.h"
#include "utility/med_xc/MEDMapNumCeldasPorTipo.h"
#include "utility/med_xc/MEDMesh.h"
#include "utility/med_xc/MEDMeshing.h"
#include "utility/med_xc/MEDObject.h"
#include "utility/med_xc/MEDTFieldInfo.h"
#include "utility/med_xc/MEDVertexInfo.h"


// sp_constraint header files
#include "domain/constraints/SP_Constraint.h"
#include "domain/constraints/ImposedMotionSP.h"
#include "domain/constraints/ImposedMotionSP1.h"

// mp_constraint header files
#include "domain/constraints/MP_Constraint.h"
#include "domain/constraints/MP_Joint2D.h"
#include "domain/constraints/MP_Joint3D.h"
#include "domain/constraints/RigidBase.h"
#include "domain/constraints/RigidBeam.h"
#include "domain/constraints/RigidRod.h"
#include "domain/constraints/RigidDiaphragm.h"
#include "domain/constraints/EqualDOF.h"
#include "domain/constraints/MRMP_Constraint.h"
#include "domain/constraints/GlueNodeToElement.h"

#include "domain/constraints/MapCasosActivos.h"
#include "domain/constraints/CondContorno.h"

// nodal load header files
#include "domain/load/NodalLoad.h"
#include "domain/load/NodalLoadIter.h"

// elemental load header files
#include "domain/load/ElementalLoad.h"
#include "domain/load/ElementalLoadIter.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "domain/load/volumen/BrickSelfWeight.h"
#include "domain/load/edge_loads/ElementEdge2dUniformLoad.h"
#include "domain/load/edge_loads/ElementEdge3dUniformLoad.h"

// matrix, vector & id header files
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/nDarray/nDarray.h"
#include "utility/matrix/nDarray/BJmatrix.h"
#include "utility/matrix/nDarray/BJvector.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/matrix/nDarray/Cosseratstraint.h"
#include "utility/matrix/nDarray/Cosseratstresst.h"


// subdomain header files
#include "domain/domain/subdomain/Subdomain.h"

// constraint handler header files
#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/handler/PlainHandler.h"
#include "solution/analysis/handler/PenaltyConstraintHandler.h"
#include "solution/analysis/handler/LagrangeConstraintHandler.h"
#include "solution/analysis/handler/TransformationConstraintHandler.h"

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

// Tipos de análisis.
#include "solution/analysis/analysis/analysis.h"

// load patterns
#include "domain/load/pattern/LoadPattern.h"
#include "domain/load/pattern/Combinacion.h"
#include "domain/load/pattern/GrupoCombinaciones.h"
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


#include "modelador/cad/SisRef.h"
#include "modelador/cad/SisRefCartesianas3d.h"
#include "modelador/cad/entidades/Pnt.h"
#include "modelador/cad/entidades/Linea.h"
#include "modelador/cad/entidades/LineaTramos.h"
#include "modelador/cad/entidades/ArcoCircunf.h"
#include "modelador/cad/entidades/SupCuadrilatera.h"
#include "modelador/cad/entidades/UniformGrid.h"
#include "modelador/cad/entidades/Body.h"
#include "modelador/cad/entidades/Block.h"
#include "modelador/cad/matrices/MatrizPtrPnt.h"
#include "modelador/cad/matrices/MatrizPtrNod.h"
#include "modelador/cad/matrices/MatrizPtrElem.h"
#include "modelador/cad/matrices/TritrizPtrPnt.h"
#include "modelador/cad/matrices/TritrizPtrNod.h"
#include "modelador/cad/matrices/TritrizPtrElem.h"
#include "modelador/cad/matrices/MapEsquemas2d.h"
#include "modelador/cad/matrices/MapEsquemas3d.h"
#include "modelador/cad/trf/TrfGeom.h"
#include "modelador/cad/trf/Traslacion.h"
#include "modelador/cad/trf/Reflexion.h"
#include "modelador/cad/trf/Escalado.h"
#include "modelador/cad/trf/Rotacion.h"
#include "modelador/cad/trf/MapTrfGeom.h"
#include "modelador/set_mgmt/Set.h"
#include "modelador/set_mgmt/SetFilaI.h"
#include "modelador/set_mgmt/SetFilaJ.h"
#include "modelador/set_mgmt/SetFilaK.h"


#ifdef _PARALLEL_PROCESSING
#include "solution/analysis/numberer/ParallelNumberer.h"
#include "solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h"
#include "solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h"
#endif

#include "utility/package/packages.h"
#include "utility/matrix/ID.h"
#include "utility/xc_python_utils.h"
#include "utility/database/SQLiteDatastore.h"
#include "utility/database/OracleDatastore.h"
#include "utility/database/BerkeleyDbDatastore.h"
#include "utility/database/NEESData.h"
#include "utility/database/MySqlDatastore.h"
#include "utility/database/FileDatastore.h"

#endif
