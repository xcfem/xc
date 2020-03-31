cmake_minimum_required(VERSION 2.6)
#cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_VERSION_STRING ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})
IF (${CMAKE_VERSION_STRING} GREATER 2.8.4)
  cmake_policy(SET CMP0017 NEW) 
  MESSAGE ("CMake version is greater than 2.8.4")
ELSE (${CMAKE_VERSION_STRING} GREATER 2.8.4)
  MESSAGE ("CMake version is not greater than 2.8.4")
ENDIF (${CMAKE_VERSION_STRING} GREATER 2.8.4)
# Project name.
PROJECT(LIBXC)

SET(build_setup_dir $ENV{HOME}/.xc_build)
SET(build_setup_file ${build_setup_dir}/xc_dirs.cmake)

#xc_utils
SET(lcmd_setup_file ${build_setup_dir}/lcmd_dirs.cmake)
INCLUDE(${lcmd_setup_file})
SET(xc_utils_DIR ${lcmd_src_dir}/..)
message(STATUS "lcmd project dir: " ${xc_utils_DIR})
SET(xc_utils_INC ${lcmd_inc_dir})
message(STATUS "lcmd include dir: " ${xc_utils_INC})
INCLUDE_DIRECTORIES(${xc_utils_INC})
SET(xc_utils_LIB ${lcmd_lib_dir})
message(STATUS "lcmd lib dir: " ${xc_utils_LIB})

#XC
string(REGEX REPLACE "xc/src.*" "xc/" DIR_FUENTES_XC ${CMAKE_SOURCE_DIR})
message(STATUS "xc project dir: " ${DIR_FUENTES_XC})
set(libXC_INC ${CMAKE_SOURCE_DIR}/)
message(STATUS "xc include dir: " ${libXC_INC})
set(libXC_LIB ${DIR_FUENTES_XC}lib)
message(STATUS "xc lib dir: " ${libXC_LIB})
file(WRITE ${build_setup_file} "SET(xc_src_dir ${DIR_FUENTES_XC})\n")
file(APPEND ${build_setup_file} "SET(xc_inc_dir ${libXC_INC})\n")
file(APPEND ${build_setup_file} "SET(xc_lib_dir ${libXC_LIB})\n")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${xc_utils_DIR}/CMake")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${DIR_FUENTES_XC}/CMake")

SET(LIBXC_SOURCE_DIR ${DIR_FUENTES_XC}/src)
SET(LIBRARY_OUTPUT_PATH ${DIR_FUENTES_XC}/lib)

INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)

INCLUDE(${DIR_FUENTES_XC}CMake/GetGitRevisionDescription.cmake)
git_describe(VERSION --tags --dirty=-d)

#parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file(${DIR_FUENTES_XC}CMake/version.cc.in
                ${DIR_FUENTES_XC}src/version.cc)
set(version_file "${DIR_FUENTES_XC}src/version.cc")

enable_language(Fortran)
INCLUDE(${DIR_FUENTES_XC}CMake/find_packages.cmake)
MESSAGE(STATUS "Boost_LIBRARIES: " ${Boost_LIBRARIES})

#HDF5 library
include_directories(${HDF5_HEADER_INCLUDE_DIR})


#VTK library
#vtk
IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  message(STATUS "vtk file: " ${USE_VTK_FILE})
  SET(VTK_FOUND 1)
  SET(VTK_BIB vtkRendering vtkIO vtkHybrid)
ENDIF(USE_VTK_FILE)
include_directories(${VTK_INCLUDE_DIRS})

#CGAL library
IF(CGAL_USE_FILE)
  INCLUDE(${CGAL_USE_FILE})
  message(STATUS "cgal file: " ${CGAL_USE_FILE})
ENDIF(CGAL_USE_FILE)

#GLIB (para gts) library
INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})

#CGAL library
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

#MySql++ library
INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES(${MYSQLPP_INCL_DIR})
message(STATUS "MySQL libraries: ${MYSQL_LIBRARY}")

#SqLiteWrapped library
INCLUDE_DIRECTORIES(${SQLITEWP_INCL_DIR})

#MPI library
INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})

#TCL library
INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})

#Portable, Extensible Toolkit for Scientific Computation
SET(PETSC_DIR /usr/lib/petscdir/3.0.0)
SET(PETSC_ARCH linux-gnu-c-opt)
find_package(Petsc)
SET(PETSC_LIB_DIR ${PETSC_DIR}/${PETSC_ARCH}/lib)
INCLUDE_DIRECTORIES(${PETSC_INCLUDE_DIR})

#Python
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

#XC library
INCLUDE_DIRECTORIES(${LIBXC_SOURCE_DIR})

#Para DEBUG
#ADD_DEFINITIONS(-Wall -O0 -frounding-math -g)
#Para RELEASE
ADD_DEFINITIONS(-Wall -O3 -march=native -frounding-math -pedantic -Wno-unused-but-set-variable)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
#Errores en arpack++
set_source_files_properties(solution/system_of_eqn/eigenSOE/BandArpackppSolver.cc PROPERTIES COMPILE_FLAGS -fpermissive)

# Source files.
SET(actor utility/actor/actor/Actor utility/actor/actor/DistributedBase utility/actor/actor/DistributedObj utility/actor/actor/MovableObject utility/actor/actor/CommMetaData utility/actor/actor/PtrCommMetaData utility/actor/actor/BrokedPtrCommMetaData utility/actor/actor/ArrayCommMetaData utility/actor/actor/MatrixCommMetaData utility/actor/actor/TensorCommMetaData utility/actor/actor/DbTagData utility/actor/actor/CommParameters utility/actor/actor/MovableMap utility/actor/actor/MovableDeque utility/actor/actor/MovableVector utility/actor/actor/MovableBJTensor utility/actor/actor/MovableString utility/actor/actor/MovableVectors utility/actor/actor/MovableMatrix utility/actor/actor/MovableID utility/actor/actor/MovableMatrices utility/actor/actor/MovableContainer utility/actor/actor/MovableStrings utility/actor/address/ChannelAddress utility/actor/address/SocketAddress utility/actor/channel/ChannelQueue utility/actor/channel/Channel utility/actor/channel/TCP_Socket utility/actor/channel/UDP_Socket utility/actor/channel/mySocket utility/actor/machineBroker/MachineBroker utility/actor/message/Message utility/actor/objectBroker/FEM_ObjectBroker utility/actor/objectBroker/FEM_ObjectBrokerAllClasses utility/actor/objectBroker/ObjectBroker utility/actor/ObjectWithObjBroker utility/actor/ShadowActorBase utility/actor/shadow/Shadow utility/xc_python_utils)

SET(mpi utility/actor/address/MPI_ChannelAddress utility/actor/channel/MPI_Channel utility/actor/machineBroker/MPI_MachineBroker)

SET(alpha_broker utility/actor/machineBroker/AlphaBaseMachineBroker utility/actor/machineBroker/AlphaMachineBroker utility/actor/machineBroker/MillMachineBroker utility/actor/machineBroker/DecMachineBroker)

SET(tcp utility/actor/channel/TCP_SocketNoDelay)

SET(database utility/database/FE_Datastore utility/database/FileDatastore utility/database/DBDatastore utility/database/BerkeleyDbDatastore utility/database/MySqlDatastore utility/database/SQLiteDatastore utility/database/NEESData )

IF(ORACLE_FOUND)
SET(database ${database} utility/database/OracleDatastore)
ENDIF(ORACLE_FOUND)

SET(handler utility/handler/DataOutputDatabaseHandler utility/handler/DataOutputFileHandler utility/handler/DataOutputHandler utility/handler/DataOutputStreamHandler utility/handler/FileStream utility/handler/OPS_Stream utility/handler/StandardStream)

SET(package utility/package/packages)

SET(recorder utility/recorder/DomainRecorderBase utility/recorder/response/ElementResponse utility/recorder/response/CompositeResponse utility/recorder/response/FiberResponse utility/recorder/response/MaterialResponse utility/recorder/response/Response utility/recorder/AlgorithmIncrements utility/recorder/DamageRecorder utility/recorder/DatastoreRecorder utility/recorder/HandlerRecorder utility/recorder/DriftRecorder utility/recorder/MeshCompRecorder utility/recorder/ElementRecorderBase utility/recorder/ElementRecorder utility/recorder/EnvelopeData utility/recorder/EnvelopeElementRecorder utility/recorder/NodeRecorderBase utility/recorder/NodeRecorder utility/recorder/EnvelopeNodeRecorder utility/recorder/FilePlotter utility/recorder/GSA_Recorder utility/recorder/MaxNodeDispRecorder utility/recorder/PatternRecorder utility/recorder/Recorder utility/recorder/PropRecorder utility/recorder/NodePropRecorder utility/recorder/ElementPropRecorder utility/recorder/RecorderContainer utility/recorder/ObjWithRecorders)

SET(remote utility/remote/remote)

SET(tagged utility/tagged/storage/TaggedObjectStorage utility/tagged/storage/ArrayOfTaggedObjects utility/tagged/storage/ArrayOfTaggedObjectsIter utility/tagged/storage/MapOfTaggedObjects utility/tagged/storage/MapOfTaggedObjectsIter utility/tagged/TaggedObject)

SET(nDarray utility/matrix/nDarray/basics utility/matrix/nDarray/BJtensor utility/matrix/nDarray/Cosseratstresst utility/matrix/nDarray/stress_strain_tensor utility/matrix/nDarray/stresst utility/matrix/nDarray/BJvector utility/matrix/nDarray/nDarray utility/matrix/nDarray/BJmatrix utility/matrix/nDarray/Cosseratstraint utility/matrix/nDarray/straint)

SET(matrix utility/matrix/ID utility/matrix/IDVarSize utility/matrix/IntPtrWrapper utility/matrix/AuxMatrix utility/matrix/Matrix utility/matrix/DqMatrices utility/matrix/Vector utility/matrix/DqVectors utility/matrix/util_matrix ${nDarray})

SET(utility ${actor} ${mpi} ${alpha_broker} ${database} ${handler} ${package} ${recorder} ${remote} ${tagged} ${matrix}  utility/Timer)

SET(post_process post_process/FieldInfo post_process/MapFields)

SET(static_integrators solution/analysis/integrator/static/IntegratorVectors solution/analysis/integrator/static/ProtoArcLength solution/analysis/integrator/static/ArcLength1 solution/analysis/integrator/static/BaseControl solution/analysis/integrator/static/DispBase solution/analysis/integrator/static/DisplacementControl solution/analysis/integrator/static/LoadControl solution/analysis/integrator/static/ArcLengthBase solution/analysis/integrator/static/DistributedDisplacementControl solution/analysis/integrator/static/LoadPath solution/analysis/integrator/static/ArcLength solution/analysis/integrator/static/EQPath solution/analysis/integrator/static/HSConstraint solution/analysis/integrator/static/MinUnbalDispNorm)

SET(transient_newmark_integrators solution/analysis/integrator/transient/NewmarkBase solution/analysis/integrator/transient/newmark/NewmarkBase2 solution/analysis/integrator/transient/newmark/Newmark solution/analysis/integrator/transient/newmark/NewmarkHybridSimulation solution/analysis/integrator/transient/newmark/Newmark1 solution/analysis/integrator/transient/newmark/NewmarkExplicit)

SET(transient_integrators solution/analysis/integrator/transient/ResponseQuantities solution/analysis/integrator/transient/CentralDifferenceBase solution/analysis/integrator/transient/CentralDifferenceAlternative solution/analysis/integrator/transient/HHT1 solution/analysis/integrator/transient/DampingFactorsIntegrator ${transient_newmark_integrators} solution/analysis/integrator/transient/CentralDifferenceNoDamping solution/analysis/integrator/transient/RayleighBase solution/analysis/integrator/transient/rayleigh/AlphaOSBase solution/analysis/integrator/transient/rayleigh/CentralDifference solution/analysis/integrator/transient/rayleigh/HHTRayleighBase solution/analysis/integrator/transient/rayleigh/HHTBase solution/analysis/integrator/transient/rayleigh/HHT solution/analysis/integrator/transient/rayleigh/HHTGeneralizedExplicit solution/analysis/integrator/transient/rayleigh/AlphaOS solution/analysis/integrator/transient/rayleigh/Collocation solution/analysis/integrator/transient/rayleigh/HHTExplicit solution/analysis/integrator/transient/rayleigh/HHTHybridSimulation solution/analysis/integrator/transient/rayleigh/AlphaOSGeneralized solution/analysis/integrator/transient/rayleigh/CollocationHybridSimulation solution/analysis/integrator/transient/rayleigh/HHTGeneralized solution/analysis/integrator/transient/rayleigh/WilsonTheta)

SET(eigen_integrators solution/analysis/integrator/eigen/LinearBucklingIntegrator solution/analysis/integrator/eigen/KEigenIntegrator)

SET(integrators solution/analysis/integrator/EigenIntegrator solution/analysis/integrator/Integrator solution/analysis/integrator/TransientIntegrator solution/analysis/integrator/IncrementalIntegrator solution/analysis/integrator/StaticIntegrator ${eigen_integrators} ${static_integrators} ${transient_integrators})

SET(analysis_eigen_algo solution/analysis/algorithm/eigenAlgo/EigenAlgorithm solution/analysis/algorithm/eigenAlgo/FrequencyAlgo solution/analysis/algorithm/eigenAlgo/StandardEigenAlgo solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo solution/analysis/algorithm/eigenAlgo/KEigenAlgo)

SET(analysis_line_search solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch)

SET(analysis_algorithm solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo solution/analysis/algorithm/SolutionAlgorithm solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase solution/analysis/algorithm/equiSolnAlgo/BFGS  solution/analysis/algorithm/equiSolnAlgo/Broyden solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo solution/analysis/algorithm/equiSolnAlgo/EquiSolnConvAlgo solution/analysis/algorithm/equiSolnAlgo/KrylovNewton solution/analysis/algorithm/equiSolnAlgo/Linear solution/analysis/algorithm/equiSolnAlgo/ModifiedNewton solution/analysis/algorithm/equiSolnAlgo/NewtonLineSearch solution/analysis/algorithm/equiSolnAlgo/NewtonBased solution/analysis/algorithm/equiSolnAlgo/NewtonRaphson solution/analysis/algorithm/equiSolnAlgo/PeriodicNewton ${analysis_line_search} ${analysis_eigen_algo})

SET(analysis_handlers  solution/analysis/handler/ConstraintHandler solution/analysis/handler/FactorsConstraintHandler solution/analysis/handler/LagrangeConstraintHandler solution/analysis/handler/PenaltyConstraintHandler solution/analysis/handler/PlainHandler solution/analysis/handler/TransformationConstraintHandler)

SET(analysis solution/analysis/analysis/Analysis solution/analysis/analysis/DirectIntegrationAnalysis solution/analysis/analysis/DomainDecompositionAnalysis solution/analysis/analysis/EigenAnalysis solution/analysis/analysis/ModalAnalysis solution/analysis/analysis/LinearBucklingEigenAnalysis solution/analysis/analysis/ModalAnalysis solution/analysis/analysis/IllConditioningAnalysis solution/analysis/analysis/LinearBucklingAnalysis solution/analysis/analysis/StaticAnalysis solution/analysis/analysis/StaticDomainDecompositionAnalysis solution/analysis/analysis/SubstructuringAnalysis solution/analysis/analysis/TransientAnalysis solution/analysis/analysis/TransientDomainDecompositionAnalysis solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis solution/analysis/model/dof_grp/DOF_Group solution/analysis/model/dof_grp/LagrangeDOF_Group solution/analysis/model/dof_grp/TransformationDOF_Group solution/analysis/model/fe_ele/MPSPBaseFE solution/analysis/model/fe_ele/SFreedom_FE solution/analysis/model/fe_ele/MPBase_FE solution/analysis/model/fe_ele/MFreedom_FE solution/analysis/model/fe_ele/MRMFreedom_FE  solution/analysis/model/fe_ele/lagrange/Lagrange_FE solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE solution/analysis/UnbalAndTangentStorage solution/analysis/UnbalAndTangent solution/analysis/model/fe_ele/FE_Element solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE solution/analysis/model/fe_ele/penalty/PenaltyMRMFreedom_FE  solution/analysis/model/fe_ele/penalty/PenaltySFreedom_FE solution/analysis/model/fe_ele/transformation/TransformationFE solution/analysis/model/AnalysisModel solution/analysis/model/DOF_GrpIter solution/analysis/model/DOF_GrpConstIter solution/analysis/model/FE_EleIter solution/analysis/model/FE_EleConstIter solution/analysis/numberer/DOF_Numberer solution/analysis/numberer/ParallelNumberer solution/analysis/numberer/PlainNumberer ${analysis_handlers} ${analysis_algorithm} ${integrators})

SET(convergenceTest solution/analysis/convergenceTest/CTestEnergyIncr solution/analysis/convergenceTest/CTestFixedNumIter solution/analysis/convergenceTest/CTestNormDispIncr solution/analysis/convergenceTest/CTestNormUnbalance solution/analysis/convergenceTest/CTestRelativeEnergyIncr solution/analysis/convergenceTest/CTestRelativeNormDispIncr solution/analysis/convergenceTest/CTestRelativeNormUnbalance solution/analysis/convergenceTest/CTestRelativeTotalNormDispIncr solution/analysis/convergenceTest/ConvergenceTest solution/analysis/convergenceTest/ConvergenceTestTol solution/analysis/convergenceTest/ConvergenceTestNorm)

SET(coordTransformation domain/mesh/element/utils/coordTransformation/CrdTransf domain/mesh/element/utils/coordTransformation/CrdTransf2d domain/mesh/element/utils/coordTransformation/CrdTransf3d domain/mesh/element/utils/coordTransformation/LinearCrdTransf2d domain/mesh/element/utils/coordTransformation/SmallDispCrdTransf3d domain/mesh/element/utils/coordTransformation/LinearCrdTransf3d domain/mesh/element/utils/coordTransformation/SmallDispCrdTransf2d domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf2d domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf3d domain/mesh/element/utils/coordTransformation/CorotCrdTransf2d domain/mesh/element/utils/coordTransformation/CorotCrdTransf3d)

SET(damage material/damage/DamageModel material/damage/DamageResponse material/damage/HystereticEnergy material/damage/Kratzig material/damage/Mehanny material/damage/NormalizedPeak material/damage/ParkAng)

SET(domain_load domain/load/beam_loads/BeamLoad domain/load/beam_loads/BeamMecLoad domain/load/beam_loads/BeamUniformLoad domain/load/beam_loads/BeamStrainLoad domain/load/beam_loads/BeamPointLoad domain/load/beam_loads/Beam2dPointLoad domain/load/beam_loads/TrussStrainLoad domain/load/beam_loads/Beam2dUniformLoad domain/load/beam_loads/Beam2dPartialUniformLoad domain/load/beam_loads/Beam3dPointLoad domain/load/beam_loads/Beam3dUniformLoad domain/load/plane/BidimLoad domain/load/plane/BidimStrainLoad domain/load/plane/ShellStrainLoad domain/load/plane/BidimMecLoad domain/load/plane/ShellMecLoad  domain/load/plane/ShellUniformLoad domain/load/volumetric/SelfWeight domain/load/volumetric/BrickSelfWeight domain/load/elem_load domain/load/ElementalLoad domain/load/ElementBodyLoad domain/load/ElementalLoadIter domain/load/ElementPtrs domain/load/Load domain/load/NodalLoad domain/load/NodalLoadIter)

SET(domain_pattern_time_series domain/load/pattern/time_series/CFactorSeries domain/load/pattern/time_series/ConstantSeries domain/load/pattern/time_series/DiscretizedRandomProcessSeries domain/load/pattern/time_series/SimulatedRandomProcessSeries domain/load/pattern/time_series/PathSeriesBase domain/load/pattern/time_series/PathTimeSeries domain/load/pattern/time_series/PulseBaseSeries domain/load/pattern/time_series/PeriodSeries domain/load/pattern/time_series/PulseSeries domain/load/pattern/time_series/RectangularSeries domain/load/pattern/time_series/LinearSeries domain/load/pattern/time_series/PathSeries domain/load/pattern/time_series/TriangleSeries domain/load/pattern/time_series/TrigSeries)

SET(domain_pattern_load_patterns domain/load/pattern/load_patterns/EQBasePattern domain/load/pattern/load_patterns/EarthquakePattern domain/load/pattern/load_patterns/PBowlLoading domain/load/pattern/load_patterns/UniformExcitation domain/load/pattern/load_patterns/MultiSupportPattern)

SET(domain_pattern_time_series_integ domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator)

SET(domain_pattern domain/load/pattern/NodeLocker domain/load/pattern/NodeLockerIter domain/load/pattern/LoadContainer domain/load/pattern/LoadPattern domain/load/pattern/MapLoadPatterns domain/load/pattern/LoadPatternCombination domain/load/pattern/LoadCombination domain/load/pattern/LoadCombinationGroup domain/load/pattern/LoadPatternIter domain/load/pattern/TimeSeries domain/load/pattern/TimeSeriesIntegrator ${domain_pattern_time_series_integ} ${domain_pattern_time_series} ${domain_pattern_load_patterns})

SET(domain_ground_motion domain/load/groundMotion/MotionHistory domain/load/groundMotion/GroundMotion domain/load/groundMotion/DqGroundMotions domain/load/groundMotion/GroundMotionRecord domain/load/groundMotion/InterpolatedGroundMotion)

SET(domain_constraints  domain/constraints/ImposedMotionBase domain/constraints/ImposedMotionSP domain/constraints/MFreedom_ConstraintBase domain/constraints/MFreedom_Constraint domain/constraints/MRMFreedom_Constraint domain/constraints/GlueNodeToElement domain/constraints/MFreedom_Joint domain/constraints/MFreedom_Joint2D domain/constraints/MFreedom_Joint3D domain/constraints/Constraint domain/constraints/SFreedom_Constraint domain/constraints/SFreedom_ConstraintIter domain/constraints/MFreedom_ConstraintIter domain/constraints/MRMFreedom_ConstraintIter domain/constraints/ImposedMotionSP1 domain/constraints/RigidBase domain/constraints/RigidRod domain/constraints/RigidDiaphragm domain/constraints/RigidBeam domain/constraints/EqualDOF domain/constraints/ConstrContainer)

SET(domain_component domain/component/DomainComponent  domain/component/ForceReprComponent domain/component/ContinuaReprComponent domain/component/Parameter domain/component/MatParameter)

SET(domain_subdomain_modelbuilder domain/domain/subdomain/modelbuilder/ModelBuilder domain/domain/subdomain/modelbuilder/PartitionedModelBuilder)

SET(domain_subdomain ${domain_subdomain_modelbuilder} domain/domain/subdomain/ActorSubdomain domain/domain/subdomain/ShadowSubdomain domain/domain/subdomain/Subdomain domain/domain/subdomain/SubdomainNodIter)

SET(domain ${domain_component} domain/domain/PseudoTimeTracker domain/domain/partitioned/PartitionedDomain domain/domain/partitioned/PartitionedDomainEleIter domain/domain/partitioned/PartitionedDomainSubIter domain/domain/Domain domain/domain/single/SingleDomAllSFreedom_Iter domain/domain/single/SingleDomEleIter domain/domain/single/SingleDomLC_Iter domain/domain/single/SingleDomMFreedom_Iter domain/domain/single/SingleDomMRMFreedom_Iter domain/domain/single/SingleDomNodIter domain/domain/single/SingleDomParamIter domain/domain/single/SingleDomSFreedom_Iter ${domain_ground_motion} ${domain_load} domain/mesh/MeshComponentContainer domain/mesh/Mesh domain/mesh/MeshEdge domain/mesh/MeshEdges domain/mesh/NodeLockers domain/mesh/MeshComponent domain/mesh/node/DummyNode domain/mesh/node/NodeVectors domain/mesh/node/NodeDispVectors domain/mesh/node/NodeVelVectors domain/mesh/node/NodeAccelVectors domain/mesh/node/Node domain/mesh/node/Node domain/mesh/node/KDTreeNodes domain/mesh/node/NodeTopology domain/partitioner/NodeLocations domain/partitioner/DomainPartitioner domain/partitioner/loadBalancer/LoadBalancer domain/partitioner/loadBalancer/ReleaseHeavierToLighterNeighbours domain/partitioner/loadBalancer/ShedHeaviest domain/partitioner/loadBalancer/SwapHeavierToLighterNeighbours ${domain_pattern} domain/mesh/region/DqMeshRegion domain/mesh/region/MeshRegion ${domain_subdomain} ${domain_constraints})

SET(trusses domain/mesh/element/truss_beam_column/truss/ProtoTruss domain/mesh/element/truss_beam_column/truss/TrussBase domain/mesh/element/truss_beam_column/truss/Truss domain/mesh/element/truss_beam_column/truss/CorotTrussBase domain/mesh/element/truss_beam_column/truss/CorotTruss domain/mesh/element/truss_beam_column/truss/CorotTrussSection domain/mesh/element/truss_beam_column/truss/TrussSection domain/mesh/element/truss_beam_column/truss/Spring )

SET(beam_integration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/DistHingeIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration3d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegrationBase domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegrationBase domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration2d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration3d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration3d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration2d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration3d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration2d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration2d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration2d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeBeamIntegration3d domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeEndpointBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/TrapezoidalBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/ParameterIDBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/FixedLocationBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/MidDistanceBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LowOrderBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/CompositeSimpsonBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/PlasticLengthsBeamIntegration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/IntegrationPointsCoords)

SET(cyclic_model domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/CyclicModel domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/LinearCyclic domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/BilinearCyclic domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/QuadraticCyclic)

SET(fvectors domain/mesh/element/utils/fvectors/FVector domain/mesh/element/utils/fvectors/FVectorBeamColumn2d domain/mesh/element/utils/fvectors/FVectorBeamColumn3d domain/mesh/element/utils/fvectors/FVectorShell)

SET(gauss_models domain/mesh/element/utils/gauss_models/GaussPoint domain/mesh/element/utils/gauss_models/GaussModel)

SET(beams ${fvectors} domain/mesh/element/truss_beam_column/SectionMatrices domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d domain/mesh/element/truss_beam_column/NLForceBeamColumn3dBase domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d  domain/mesh/element/truss_beam_column/EsfBeamColumn3d domain/mesh/element/truss_beam_column/ProtoBeam2d domain/mesh/element/truss_beam_column/ProtoBeam3d domain/mesh/element/truss_beam_column/beam2d/beam2d domain/mesh/element/truss_beam_column/beam2d/beam2d02 domain/mesh/element/truss_beam_column/beam2d/beam2d03 domain/mesh/element/truss_beam_column/beam2d/beam2d04 domain/mesh/element/truss_beam_column/beam3d/beam3dBase domain/mesh/element/truss_beam_column/beam3d/beam3d01 domain/mesh/element/truss_beam_column/beam3d/beam3d02 domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2dBase domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumnNL2d domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussLobattoQuadRule1d01 domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d01 domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule1d domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule1d01  domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Elastic2DGNL domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS01   domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS02 domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS03 domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL  domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D)

SET(volumetric_elements domain/mesh/element/volumetric/BrickBase domain/mesh/element/volumetric/20nbrick/TwentyNodeBrick domain/mesh/element/volumetric/20nbrick/Twenty_Node_Brick domain/mesh/element/volumetric/8nbrick/EightNodeBrick domain/mesh/element/volumetric/27nbrick/TwentySevenNodeBrick domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD8NodeBrick domain/mesh/element/volumetric/UP-ucsd/BrickUP domain/mesh/element/volumetric/UP-ucsd/TwentyEightNodeBrickUP domain/mesh/element/volumetric/upU/EightNodeBrick_u_p_U domain/mesh/element/volumetric/upU/TwentyNodeBrick_u_p_U domain/mesh/element/volumetric/brick/BbarBrick domain/mesh/element/volumetric/brick/Brick domain/mesh/element/volumetric/brick/shp3d domain/mesh/element/volumetric/UP-ucsd/shp3dv)

SET(plane_element domain/mesh/element/plane/PlaneElement domain/mesh/element/plane/TriBase3N domain/mesh/element/plane/triangle/Tri31 domain/mesh/element/plane/QuadBase9N domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad domain/mesh/element/plane/fourNodeQuad/EnhancedQuad domain/mesh/element/plane/fourNodeQuad/FourNodeQuad domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP domain/mesh/element/plane/UP-ucsd/Nine_Four_Node_QuadUP domain/mesh/element/plane/shell/R3vectors domain/mesh/element/plane/shell/ShellCrdTransf3dBase domain/mesh/element/plane/shell/ShellLinearCrdTransf3d domain/mesh/element/plane/shell/ShellNLCrdTransf3d domain/mesh/element/plane/shell/ShellUpBasisCrdTransf3d domain/mesh/element/plane/shell/ShellBData domain/mesh/element/plane/shell/ShellMITC4Base domain/mesh/element/plane/shell/Shell4NBase domain/mesh/element/plane/shell/ShellMITC4 domain/mesh/element/plane/shell/ShellMITC9 domain/mesh/element/plane/shell/ShellNLDKGQ)

SET(frictionBearing domain/mesh/element/special/frictionBearing/frictionModel/FrictionModel domain/mesh/element/special/frictionBearing/frictionModel/CoulombFriction domain/mesh/element/special/frictionBearing/frictionModel/VDependentFriction domain/mesh/element/special/frictionBearing/frictionModel/VPDependentFriction domain/mesh/element/special/frictionBearing/frictionModel/FrictionResponse domain/mesh/element/special/frictionBearing/FrictionElementBase domain/mesh/element/special/frictionBearing/FlatSliderSimple2d domain/mesh/element/special/frictionBearing/FlatSliderSimple3d domain/mesh/element/special/frictionBearing/SingleFPSimple3d domain/mesh/element/special/frictionBearing/SingleFPSimple2d)

SET(physical_properties domain/mesh/element/utils/physical_properties/PhysicalProperties domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties domain/mesh/element/utils/physical_properties/SolidMech2D domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties domain/mesh/element/utils/physical_properties/BeamColumnJointPhysicalProperties domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties domain/mesh/element/utils/physical_properties/Joint2DPhysicalProperties domain/mesh/element/utils/physical_properties/Joint3DPhysicalProperties domain/mesh/element/utils/physical_properties/SpringModels domain/mesh/element/utils/physical_properties/DamageModels domain/mesh/element/utils/physical_properties/DamageModelVector)

SET(body_forces domain/mesh/element/utils/body_forces/BodyForces domain/mesh/element/utils/body_forces/BodyForces2D domain/mesh/element/utils/body_forces/BodyForces3D)

SET(element ${physical_properties}  ${body_forces} domain/mesh/element/Element domain/mesh/element/utils/ParticlePos3d domain/mesh/element/utils/KDTreeElements domain/mesh/element/utils/ElementEdge domain/mesh/element/utils/ElementEdges domain/mesh/element/utils/RayleighDampingFactors domain/mesh/element/Element0D domain/mesh/element/Element1D domain/mesh/element/utils/NodePtrs domain/mesh/element/utils/NodePtrsWithIDs domain/mesh/element/utils/Information domain/mesh/element/NewElement ${beams} ${beam_integration} ${volumetric_elements} ${plane_element} domain/mesh/element/special/joint/BeamColumnJoint2d domain/mesh/element/special/joint/BeamColumnJoint3d domain/mesh/element/special/joint/Joint2D domain/mesh/element/special/joint/Joint3D  ${trusses} domain/mesh/element/zeroLength/ZeroLength domain/mesh/element/zeroLength/ZeroLengthContact domain/mesh/element/zeroLength/ZeroLengthContact2D domain/mesh/element/zeroLength/ZeroLengthContact3D domain/mesh/element/zeroLength/ZeroLengthSection ${frictionBearing})

SET(element_feap domain/mesh/element/feap/fElement domain/mesh/element/feap/fElmt02 domain/mesh/element/feap/fElmt05)

SET(graph solution/graph/graph/ModelGraph solution/graph/graph/ArrayGraph solution/graph/graph/ArrayVertexIter solution/graph/graph/DOF_Graph solution/graph/graph/DOF_GroupGraph solution/graph/graph/Graph solution/graph/graph/Vertex solution/graph/graph/VertexIter solution/graph/numberer/GraphNumberer solution/graph/numberer/MyRCM solution/graph/numberer/RCM solution/graph/numberer/BaseNumberer solution/graph/numberer/SimpleNumberer solution/graph/partitioner/Metis)

SET(graph2 solution/graph/graph/FE_VertexIter solution/graph/numberer/MetisNumberer)

SET(feap_material material/nD/FeapMaterial material/nD/feap/FeapMaterial01 material/nD/feap/FeapMaterial02 material/nD/feap/FeapMaterial03)

SET(geom_section_repres material/section/repres/SectionMassProperties material/section/repres/geom_section/region/RegionContainer material/section/repres/geom_section/region/SectRegion material/section/repres/geom_section/region/QuadCellRegion material/section/repres/geom_section/region/PolygonSectRegion material/section/repres/geom_section/region/CircularSectRegion material/section/repres/geom_section/region/QuadSectRegion material/section/repres/geom_section/reinfBar/ReinfBar  material/section/repres/geom_section/reinfBar/VectorReinfBar material/section/repres/geom_section/reinfLayer/ListReinfLayer material/section/repres/geom_section/reinfLayer/ReinfLayer material/section/repres/geom_section/reinfLayer/CircReinfLayer material/section/repres/geom_section/reinfLayer/SingleBar material/section/repres/geom_section/reinfLayer/StraightReinfLayer material/section/repres/geom_section/Spot material/section/repres/geom_section/Axis material/section/repres/geom_section/Segment material/section/repres/geom_section/EntGeomSection material/section/repres/geom_section/SectionReferenceFrame material/section/repres/geom_section/SectionCartesianReferenceFrame material/section/repres/geom_section/GeomSection)

SET(section_material_repres material/section/repres/cell/VectorCells material/section/repres/cell/Cell material/section/repres/cell/QuadCell material/section/repres/cell/TriangCell material/section/repres/DiscretBase material/section/repres/section/FiberSectionRepr material/section/repres/section/FiberData material/section/repres/section/SectionRepres material/section/repres/CrossSectionProperties2d material/section/repres/CrossSectionProperties3d ${geom_section_repres})

SET(section_plate_material material/section/plate_section/PlateBase material/section/plate_section/ElasticPlateBase material/section/plate_section/ElasticPlateProto material/section/plate_section/ElasticMembranePlateSection material/section/plate_section/ElasticPlateSection material/section/plate_section/MembranePlateFiberSection)

SET(fiber_section_material material/section/fiber_section/FiberSectionBase material/section/fiber_section/FiberSection2d material/section/fiber_section/FiberSection3dBase material/section/fiber_section/FiberSection3d material/section/fiber_section/FiberSectionGJ material/section/fiber_section/FiberSectionShear3d)

SET(elastic_section_material material/section/elastic_section/BaseElasticSection material/section/elastic_section/BaseElasticSection2d material/section/elastic_section/BaseElasticSection3d material/section/elastic_section/ElasticSection2d material/section/elastic_section/ElasticShearSection2d material/section/elastic_section/ElasticSection3d material/section/elastic_section/ElasticShearSection3d)

SET(section_material material/section/interaction_diagram/DeformationPlane material/section/interaction_diagram/PivotsUltimateStrains material/section/interaction_diagram/InteractionDiagramData material/section/interaction_diagram/NormalStressStrengthParameters material/section/interaction_diagram/NMPointCloud material/section/interaction_diagram/NMPointCloudBase material/section/interaction_diagram/NMyMzPointCloud material/section/interaction_diagram/Pivots material/section/interaction_diagram/ComputePivots material/section/interaction_diagram/ClosedTriangleMesh material/section/interaction_diagram/InteractionDiagram2d material/section/interaction_diagram/InteractionDiagram material/section/fiber_section/fiber/Fiber material/section/fiber_section/fiber/FiberSet material/section/fiber_section/fiber/FiberPtrDeque material/section/fiber_section/fiber/FiberSets material/section/fiber_section/fiber/FiberContainer material/section/fiber_section/fiber/UniaxialFiber material/section/fiber_section/fiber/UniaxialFiber2d material/section/fiber_section/fiber/UniaxialFiber3d material/section/Bidirectional ${elastic_section_material} ${fiber_section_material} material/section/GenericSection1d material/section/GenericSectionNd material/section/Isolator2spring material/section/AggregatorAdditions material/section/SectionAggregator material/section/ResponseId material/section/CrossSectionKR material/section/PrismaticBarCrossSectionsVector material/section/SectionForceDeformation material/section/PrismaticBarCrossSection  ${section_material_repres} material/section/yieldSurface/YS_Section2D01 material/section/yieldSurface/YS_Section2D02 material/section/yieldSurface/YieldSurfaceSection2d ${section_plate_material})

SET(nD_elastic_isotropic material/nD/elastic_isotropic/ElasticIsotropic3D material/nD/elastic_isotropic/ElasticIsotropicAxiSymm material/nD/elastic_isotropic/ElasticIsotropicBeamFiber material/nD/elastic_isotropic/ElasticIsotropicMaterial material/nD/elastic_isotropic/ElasticIsotropic2D material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D material/nD/elastic_isotropic/ElasticIsotropicPlateFiber  material/nD/elastic_isotropic/PressureDependentElastic3D)

SET(nD_j2_plasticity material/nD/j2_plasticity/J2AxiSymm material/nD/j2_plasticity/J2PlaneStrain material/nD/j2_plasticity/J2PlaneStress material/nD/J2Plasticity material/nD/j2_plasticity/J2PlateFiber material/nD/j2_plasticity/J2ThreeDimensional)

SET(nD_uvmaterial material/nD/UWmaterials/DruckerPrager material/nD/UWmaterials/DruckerPragerPlaneStrain material/nD/UWmaterials/DruckerPrager3D)


SET(nd_adaptor_material material/nD/NDAdaptorMaterial material/nD/nd_adaptor/PlaneStressMaterial material/nD/nd_adaptor/PlateFiberMaterial material/nD/nd_adaptor/BeamFiberMaterial)

SET(WEnergy material/nD/FiniteDeformation/WEnergy/WEnergy material/nD/FiniteDeformation/WEnergy/LogWEnergy material/nD/FiniteDeformation/WEnergy/MooneyRivlinSimoWEnergy material/nD/FiniteDeformation/WEnergy/MooneyRivlinWEnergy  material/nD/FiniteDeformation/WEnergy/NeoHookeanWEnergy material/nD/FiniteDeformation/WEnergy/OgdenSimoWEnergy material/nD/FiniteDeformation/WEnergy/OgdenWEnergyBase material/nD/FiniteDeformation/WEnergy/OgdenWEnergy material/nD/FiniteDeformation/WEnergy/SimoPisterWEnergy)

SET(fdEvolution material/nD/FiniteDeformation/fdEvolution/fdEvolution_S material/nD/FiniteDeformation/fdEvolution/fdEvolution_SLS material/nD/FiniteDeformation/fdEvolution/fdEvolution_T material/nD/FiniteDeformation/fdEvolution/fdEvolution_TL)

SET(fdFlow material/nD/FiniteDeformation/fdFlow/fdFlow material/nD/FiniteDeformation/fdFlow/fdFlowDP material/nD/FiniteDeformation/fdFlow/fdFlowVM)

SET(fdYield material/nD/FiniteDeformation/fdYield/fdYield material/nD/FiniteDeformation/fdYield/fdYieldDP material/nD/FiniteDeformation/fdYield/fdYieldVM)

SET(finiteDeformation material/nD/FiniteDeformation/FiniteDeformationMaterial  material/nD/FiniteDeformation/NeoHookeanCompressible3D material/nD/FiniteDeformation/FDEPState material/nD/FiniteDeformation/FDdecoupledElastic3D material/nD/FiniteDeformation/FiniteDeformationEP3D material/nD/FiniteDeformation/FiniteDeformationElastic3D  ${fdEvolution} ${fdFlow} ${fdYield} ${WEnergy})

SET(nD_soil material/nD/soil/FluidSolidPorousMaterial material/nD/soil/MultiYieldSurface material/nD/soil/PressureMultiYieldBase material/nD/soil/PressureDependMultiYieldBase material/nD/soil/PressureDependMultiYield material/nD/soil/PressureDependMultiYield02 material/nD/soil/PressureIndependMultiYield material/nD/soil/T2Vector material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity3D material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityAxiSymm material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityPlaneStrain)

SET(nD_material material/nD/Template3Dep/CamClayPotentialSurface material/nD/Template3Dep/CamClayYieldSurface material/nD/Template3Dep/DruckerPragerPotentialSurface material/nD/Template3Dep/DruckerPragerYieldSurface material/nD/Template3Dep/DruckerPragerYieldSurface01 material/nD/Template3Dep/EvolutionLaw_LEeq material/nD/Template3Dep/EvolutionLaw_LEij material/nD/Template3Dep/EvolutionLaw_NLEeq material/nD/Template3Dep/EvolutionLaw_NLEij material/nD/Template3Dep/EvolutionLaw_NLEijMD material/nD/Template3Dep/EvolutionLaw_NLEp material/nD/Template3Dep/EvolutionLaw_S material/nD/Template3Dep/EvolutionLaw_T material/nD/Template3Dep/EPState material/nD/Template3Dep/ManzariDafaliasPotentialSurface material/nD/Template3Dep/ManzariDafaliasEvolutionLaw material/nD/Template3Dep/ManzariDafaliasPotentialSurface01 material/nD/Template3Dep/ManzariDafaliasYieldSurface material/nD/Template3Dep/MatPoint3D material/nD/Template3Dep/PotentialSurface material/nD/Template3Dep/RoundedMohrCoulomb01 material/nD/Template3Dep/RoundedMohrCoulomb01PotentialSurface material/nD/Template3Dep/RoundedMohrCoulomb01YieldSurface material/nD/Template3Dep/Template3Dep material/nD/Template3Dep/Tri_a_fail_crit_YieldSurface material/nD/Template3Dep/VonMisesPotentialSurface material/nD/Template3Dep/VonMisesYieldSurface material/nD/Template3Dep/YieldSurface material/nD/ElasticCrossAnisotropic ${nd_adaptor_material} ${nD_elastic_isotropic} ${nD_j2_plasticity} ${nD_uvmaterial} material/nD/NDMaterial  ${finiteDeformation} ${nD_soil})

SET(uniaxial_steel_material material/uniaxial/steel/SteelBase material/uniaxial/steel/SteelBase0103 material/uniaxial/steel/Steel01 material/uniaxial/steel/Steel02 material/uniaxial/steel/Steel03)

SET(uniaxial_concrete_material material/uniaxial/concrete/RawConcrete material/uniaxial/concrete/ConcreteBase material/uniaxial/concrete/Concrete01 material/uniaxial/concrete/Concrete02 material/uniaxial/concrete/Concrete04)

SET(uniaxial_py_material material/uniaxial/PY/InternalParamsA material/uniaxial/PY/InternalParamsIn material/uniaxial/PY/InternalParamsLR material/uniaxial/PY/InternalParamsLRIn material/uniaxial/PY/PYBase material/uniaxial/PY/PQyzBase material/uniaxial/PY/PyLiq1 material/uniaxial/PY/PySimple1 material/uniaxial/PY/generators/Simple1GenBase material/uniaxial/PY/generators/PySimple1Gen material/uniaxial/PY/QzSimple1 material/uniaxial/PY/TzLiq1 material/uniaxial/PY/TzSimple1 material/uniaxial/PY/generators/TzSimple1Gen)

SET(uniaxial_connected_material material/uniaxial/connected/ConnectedMaterial material/uniaxial/connected/SeriesMaterial material/uniaxial/connected/ParallelMaterial)

SET(drainmater material/uniaxial/drain/DrainMaterial material/uniaxial/drain/DrainBilinearMaterial material/uniaxial/drain/DrainClough1Material material/uniaxial/drain/DrainClough2Material material/uniaxial/drain/DrainHardeningMaterial material/uniaxial/drain/DrainPinch1Material material/uniaxial/drain/common00.f material/uniaxial/drain/anal00.f)

SET(fedeas material/uniaxial/FedeasMaterial material/uniaxial/fedeas/FedeasBondMaterial material/uniaxial/fedeas/FedeasBond1Material material/uniaxial/fedeas/FedeasBond2Material material/uniaxial/fedeas/FedeasConcrMaterial material/uniaxial/fedeas/FedeasConcr1Material material/uniaxial/fedeas/FedeasConcr2Material material/uniaxial/fedeas/FedeasConcr3Material material/uniaxial/fedeas/FedeasHardeningMaterial material/uniaxial/fedeas/FedeasHyster1Material material/uniaxial/fedeas/FedeasHyster2Material material/uniaxial/fedeas/FedeasSteel1Material material/uniaxial/fedeas/FedeasSteel2Material)

SET(uniaxial_material material/uniaxial/DqUniaxialMaterial material/uniaxial/ZeroLengthMaterials material/uniaxial/snap/Bilinear material/uniaxial/snap/Clough material/uniaxial/snap/CloughDamage material/uniaxial/snap/Pinching material/uniaxial/snap/PinchingDamage material/uniaxial/BarSlipMaterial material/uniaxial/BoucWenMaterial material/uniaxial/CableMaterial material/uniaxial/ENTMaterial material/uniaxial/EPPBaseMaterial material/uniaxial/EPPGapMaterial material/uniaxial/ElasticBaseMaterial material/uniaxial/ElasticMaterial material/uniaxial/ElasticPPMaterial material/uniaxial/FatigueMaterial material/uniaxial/HardeningMaterial material/uniaxial/HystereticMaterial material/uniaxial/EncapsulatedMaterial material/uniaxial/MinMaxMaterial material/uniaxial/NewUniaxialMaterial  material/uniaxial/PathIndependentMaterial material/uniaxial/Pinching4Material material/uniaxial/ReinforcingSteel material/uniaxial/UniaxialHistoryVars  material/uniaxial/UniaxialStateVars material/uniaxial/UniaxialMaterial material/uniaxial/ViscousMaterial   ${fedeas} ${uniaxial_py_material} ${uniaxial_concrete_material} ${uniaxial_steel_material} ${uniaxial_connected_material} ${drainmater})

SET(yield_sfc_material material/yieldSurface/evolution/BkStressLimSurface2D material/yieldSurface/evolution/BoundingSurface2D material/yieldSurface/evolution/CombinedIsoKin2D01 material/yieldSurface/evolution/CombinedIsoKin2D02 material/yieldSurface/evolution/Isotropic2D01 material/yieldSurface/evolution/Kinematic2D01 material/yieldSurface/evolution/Kinematic2D02 material/yieldSurface/evolution/NullEvolution material/yieldSurface/evolution/PeakOriented2D01 material/yieldSurface/evolution/PeakOriented2D02 material/yieldSurface/evolution/PlasticHardening2D material/yieldSurface/evolution/YS_Evolution material/yieldSurface/evolution/YS_Evolution2D material/yieldSurface/plasticHardeningMaterial/ExponReducing material/yieldSurface/plasticHardeningMaterial/MultiLinearKp material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial material/yieldSurface/plasticHardeningMaterial/PlasticHardeningMaterial material/yieldSurface/yieldSurfaceBC/Attalla2D material/yieldSurface/yieldSurfaceBC/ElTawil2D material/yieldSurface/yieldSurfaceBC/ElTawil2DUnSym material/yieldSurface/yieldSurfaceBC/Hajjar2D material/yieldSurface/yieldSurfaceBC/NullYS2D material/yieldSurface/yieldSurfaceBC/Orbison2D material/yieldSurface/yieldSurfaceBC/YieldSurface_BC material/yieldSurface/yieldSurfaceBC/YieldSurface_BC2D)

SET(material material/Material material/MaterialVector ${uniaxial_material} ${nD_material} ${section_material} ${yield_sfc_material}) 

SET(reliability reliability/FEsensitivity/NewmarkSensitivityIntegrator reliability/FEsensitivity/SensitivityAlgorithm reliability/FEsensitivity/SensitivityIntegrator reliability/FEsensitivity/StaticSensitivityIntegrator reliability/domain/components/CorrelationCoefficient reliability/domain/components/LimitStateFunction reliability/domain/components/Positioner reliability/domain/components/ParameterPositioner reliability/domain/components/RandomVariable reliability/domain/components/RandomVariablePositioner reliability/domain/components/ReliabilityDomain reliability/domain/components/ReliabilityDomainComponent reliability/domain/distributions/BetaRV reliability/domain/distributions/ChiSquareRV reliability/domain/distributions/ExponentialRV reliability/domain/distributions/GammaRV reliability/domain/distributions/GumbelRV reliability/domain/distributions/LaplaceRV reliability/domain/distributions/LognormalRV reliability/domain/distributions/NormalRV reliability/domain/distributions/ParetoRV reliability/domain/distributions/RayleighRV reliability/domain/distributions/ShiftedExponentialRV reliability/domain/distributions/ShiftedRayleighRV reliability/domain/distributions/Type1LargestValueRV reliability/domain/distributions/Type1SmallestValueRV reliability/domain/distributions/Type2LargestValueRV reliability/domain/distributions/Type3SmallestValueRV reliability/domain/distributions/UniformRV reliability/domain/distributions/UserDefinedRV reliability/domain/distributions/WeibullRV reliability/domain/filter/Filter reliability/domain/filter/KooFilter reliability/domain/filter/StandardLinearOscillatorAccelerationFilter reliability/domain/filter/StandardLinearOscillatorDisplacementFilter reliability/domain/filter/StandardLinearOscillatorVelocityFilter reliability/domain/modulatingFunction/ConstantModulatingFunction reliability/domain/modulatingFunction/GammaModulatingFunction reliability/domain/modulatingFunction/KooModulatingFunction reliability/domain/modulatingFunction/ModulatingFunction reliability/domain/modulatingFunction/TrapezoidalModulatingFunction reliability/domain/spectrum/JonswapSpectrum reliability/domain/spectrum/NarrowBandSpectrum reliability/domain/spectrum/PointsSpectrum reliability/domain/spectrum/Spectrum reliability/analysis/misc/MatrixOperations reliability/analysis/analysis/ParametricReliabilityAnalysis reliability/analysis/analysis/FOSMAnalysis reliability/analysis/analysis/SamplingAnalysis reliability/analysis/analysis/GFunVisualizationAnalysis reliability/analysis/analysis/FragilityAnalysis reliability/analysis/analysis/SystemAnalysis reliability/analysis/analysis/MVFOSMAnalysis reliability/analysis/analysis/FORMAnalysis reliability/analysis/analysis/ReliabilityAnalysis reliability/analysis/analysis/SORMAnalysis reliability/analysis/analysis/OutCrossingAnalysis reliability/analysis/designPoint/FindDesignPointAlgorithm reliability/analysis/designPoint/SearchWithStepSizeAndStepDirection reliability/analysis/rootFinding/RootFinding reliability/analysis/rootFinding/SecantRootFinding reliability/analysis/rootFinding/ModNewtonRootFinding reliability/analysis/stepSize/ArmijoStepSizeRule reliability/analysis/stepSize/FixedStepSizeRule reliability/analysis/stepSize/StepSizeRule reliability/analysis/sensitivity/GradGEvaluator reliability/analysis/sensitivity/OpenSeesGradGEvaluator reliability/analysis/sensitivity/FiniteDifferenceGradGEvaluator reliability/analysis/transformation/ProbabilityTransformation reliability/analysis/transformation/NatafProbabilityTransformation reliability/analysis/direction/SearchDirection reliability/analysis/direction/PolakHeSearchDirectionAndMeritFunction reliability/analysis/direction/SQPsearchDirectionMeritFunctionAndHessian reliability/analysis/direction/HLRFSearchDirection reliability/analysis/direction/GradientProjectionSearchDirection reliability/analysis/meritFunction/MeritFunctionCheck reliability/analysis/meritFunction/AdkZhangMeritFunctionCheck reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck reliability/analysis/hessianApproximation/HessianApproximation reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck reliability/analysis/convergenceCheck/OptimalityConditionReliabilityConvergenceCheck reliability/analysis/convergenceCheck/StandardReliabilityConvergenceCheck reliability/analysis/gFunction/TclGFunEvaluator reliability/analysis/gFunction/BasicGFunEvaluator reliability/analysis/gFunction/GFunEvaluator reliability/analysis/gFunction/OpenSeesGFunEvaluator reliability/analysis/randomNumber/RandomNumberGenerator reliability/analysis/randomNumber/CStdLibRandGenerator reliability/analysis/curvature/FirstPrincipalCurvature reliability/analysis/curvature/CurvaturesBySearchAlgorithm reliability/analysis/curvature/FindCurvatures)

SET(siseq_linear_distributed solution/system_of_eqn/linearSOE/DistributedLinSOE solution/system_of_eqn/linearSOE/DistributedBandLinSOE solution/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE solution/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE  solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver solution/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver) 

SET(siseq_linear solution/system_of_eqn/linearSOE/LinearSOEData solution/system_of_eqn/linearSOE/BJsolvers/profmatr solution/system_of_eqn/linearSOE/BJsolvers/skymatr solution/system_of_eqn/linearSOE/DomainSolver solution/system_of_eqn/linearSOE/LinearSOE solution/system_of_eqn/linearSOE/LinearSOESolver solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver   solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver  solution/system_of_eqn/linearSOE/cg/ConjugateGradientSolver solution/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver solution/system_of_eqn/linearSOE/diagonal/DiagonalSOE solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver solution/system_of_eqn/linearSOE/itpack/ItpackLinSOE solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBase solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver solution/system_of_eqn/linearSOE/FactoredSOEBase solution/system_of_eqn/linearSOE/SparseSOEBase solution/system_of_eqn/linearSOE/sparseGEN/SparseGenSOEBase solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver solution/system_of_eqn/linearSOE/sparseGEN/SuperLU solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSOE solution/system_of_eqn/linearSOE/sparseSYM/nmat solution/system_of_eqn/linearSOE/sparseSYM/symbolic solution/system_of_eqn/linearSOE/sparseSYM/nest solution/system_of_eqn/linearSOE/sparseSYM/utility solution/system_of_eqn/linearSOE/sparseSYM/grcm solution/system_of_eqn/linearSOE/sparseSYM/newordr  solution/system_of_eqn/linearSOE/sparseSYM/nnsim  solution/system_of_eqn/linearSOE/sparseSYM/tim solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver ${siseq_linear_distributed})

SET(siseq_eigen solution/system_of_eqn/eigenSOE/ArpackSOE solution/system_of_eqn/eigenSOE/BandArpackSOE solution/system_of_eqn/eigenSOE/BandArpackSolver solution/system_of_eqn/eigenSOE/EigenSOE solution/system_of_eqn/eigenSOE/EigenSolver solution/system_of_eqn/eigenSOE/SymArpackSOE solution/system_of_eqn/eigenSOE/SymArpackSolver solution/system_of_eqn/eigenSOE/SymBandEigenSOE solution/system_of_eqn/eigenSOE/SymBandEigenSolver solution/system_of_eqn/eigenSOE/BandArpackppSOE solution/system_of_eqn/eigenSOE/BandArpackppSolver solution/system_of_eqn/eigenSOE/FullGenEigenSOE solution/system_of_eqn/eigenSOE/FullGenEigenSolver)

SET(siseq_petsc solution/system_of_eqn/linearSOE/petsc/PetscSolver solution/system_of_eqn/linearSOE/petsc/PetscSOE solution/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver)

SET(siseq_petsc_no solution/system_of_eqn/linearSOE/petsc/ActorPetscSOE solution/system_of_eqn/linearSOE/petsc/badPetscSOE solution/system_of_eqn/linearSOE/petsc/ShadowPetscSOE)

SET(siseq solution/system_of_eqn/Solver solution/system_of_eqn/SystemOfEqn ${siseq_linear} ${siseq_eigen} ${siseq_petsc})

SET(siseq_no solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinThreadSolver solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver  solution/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU solution/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU  solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver)

SET(unittest unittest/unittest)

SET(preprocessor_mbt_trf preprocessor/multi_block_topology/trf/TrfGeom preprocessor/multi_block_topology/trf/MapTrfGeom preprocessor/multi_block_topology/trf/Rotation preprocessor/multi_block_topology/trf/Scaling preprocessor/multi_block_topology/trf/Reflection preprocessor/multi_block_topology/trf/Translation)

SET(preprocessor_mbt_0d_entities preprocessor/multi_block_topology/entities/0d/Pnt)

SET(preprocessor_mbt_1d_entities preprocessor/multi_block_topology/entities/1d/Edge preprocessor/multi_block_topology/entities/1d/LineBase preprocessor/multi_block_topology/entities/1d/Line preprocessor/multi_block_topology/entities/1d/DividedLine preprocessor/multi_block_topology/entities/1d/CircularArc preprocessor/multi_block_topology/entities/1d/CmbEdge)

SET(preprocessor_mbt_2d_entities preprocessor/multi_block_topology/entities/2d/Face preprocessor/multi_block_topology/entities/2d/QuadSurface)

SET(preprocessor_mbt_3d_entities preprocessor/multi_block_topology/entities/3d/Body preprocessor/multi_block_topology/entities/3d/Block preprocessor/multi_block_topology/entities/3d/UniformGrid)

SET(preprocessor_mbt_entities preprocessor/multi_block_topology/entities/EntMdlr ${preprocessor_mbt_0d_entities} ${preprocessor_mbt_1d_entities} ${preprocessor_mbt_2d_entities} ${preprocessor_mbt_3d_entities})

SET(preprocessor_mbt_entities_containers preprocessor/multi_block_topology/entities/containers/PntMap preprocessor/multi_block_topology/entities/containers/LineMap preprocessor/multi_block_topology/entities/containers/SurfaceMap preprocessor/multi_block_topology/entities/containers/BodyMap preprocessor/multi_block_topology/entities/containers/UniformGridMap)

SET(preprocessor_mbt_matrices preprocessor/multi_block_topology/matrices/PntPtrArray preprocessor/multi_block_topology/matrices/Framework2d preprocessor/multi_block_topology/matrices/PntPtrArray3d preprocessor/multi_block_topology/matrices/Framework3d preprocessor/multi_block_topology/matrices/NodePtrArray preprocessor/multi_block_topology/matrices/ElemPtrArray preprocessor/multi_block_topology/matrices/NodePtrArray3d preprocessor/multi_block_topology/matrices/ElemPtrArray3d)

SET(preprocessor_mbt preprocessor/multi_block_topology/ModelComponentContainerBase preprocessor/multi_block_topology/ReferenceFrame preprocessor/multi_block_topology/ReferenceFrameMap preprocessor/multi_block_topology/CartesianReferenceFrame3d preprocessor/multi_block_topology/MultiBlockTopology preprocessor/multi_block_topology/aux_meshing ${preprocessor_mbt_trf} ${preprocessor_mbt_entities} ${preprocessor_mbt_entities_containers} ${preprocessor_mbt_matrices} )

SET(preprocessor_prep_handlers preprocessor/PreprocessorContainer preprocessor/prep_handlers/PrepHandler preprocessor/prep_handlers/NodeHandler preprocessor/prep_handlers/ElementHandler preprocessor/prep_handlers/ProtoElementHandler preprocessor/prep_handlers/MaterialHandler preprocessor/prep_handlers/BeamIntegratorHandler preprocessor/prep_handlers/TransfCooHandler preprocessor/prep_handlers/LoadHandlerMember preprocessor/prep_handlers/LoadHandler preprocessor/prep_handlers/BoundaryCondHandler)

SET(preprocessor_set_mgmt  preprocessor/set_mgmt/DqPtrsKDTree preprocessor/set_mgmt/DqPtrsNode preprocessor/set_mgmt/DqPtrsElem preprocessor/set_mgmt/DqPtrsConstraint preprocessor/set_mgmt/SetMeshComp preprocessor/set_mgmt/SetBase preprocessor/set_mgmt/SetEstruct preprocessor/set_mgmt/SetEntities preprocessor/set_mgmt/Set preprocessor/set_mgmt/IRowSet preprocessor/set_mgmt/JRowSet preprocessor/set_mgmt/KRowSet preprocessor/set_mgmt/MapSetBase preprocessor/set_mgmt/MapSet)

SET(preprocessor preprocessor/EntMdlrBase preprocessor/MeshingParams ${preprocessor_mbt} ${preprocessor_set_mgmt} ${preprocessor_prep_handlers} preprocessor/Preprocessor)

SET(solution solution/analysis/ModelWrapper solution/AnalysisAggregation solution/AnalysisAggregationMap solution/analysis/MapModelWrapper solution/ProcSoluControl solution/ProcSolu)

# Build our library
add_library(XcBib SHARED ${utility} ${material} ${siseq} ${analysis} ${convergenceTest} ${coordTransformation} ${damage} ${domain} ${gauss_models} ${cyclic_model} ${element} ${graph} ${modelbuilder} ${reliability} ${unitest} ${preprocessor} ${solution} ${post_process} version FEProblem)

#Python interface
TARGET_LINK_LIBRARIES(XcBib xc_utils xc_basic_utils ${VTK_BIB} ${CGAL_LIBRARIES} ${Plot_LIBRARY} ${MPFR_LIBRARIES} ${GMP_LIBRARY} ${MYSQL_LIBRARY} ${MySQLpp_LIBRARIES} ${SQLITE3_LIBRARY} ${GNUGTS_LIBRARIES} ${BerkeleyDB_LIBRARIES} ${ARPACK_LIB} ${ARPACKPP_LIB} ${LAPACK_LIBRARIES} ${SUPERLU_LIBRARIES} ${BLAS_LIBRARIES} ${PETSC_LIB_PETSC} ${METIS_LIBRARIES} ${TCL_LIBRARY} boost_python ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
add_definitions(-fno-strict-aliasing)
# Define the wrapper library that wraps our library
add_library(xc SHARED utility/export_utility material/export_material_base material/uniaxial/export_material_uniaxial material/nD/export_material_nD material/section/export_material_section material/section/export_material_fiber_section  material/damage/export_material_damage domain/export_domain domain/mesh/export_domain_mesh preprocessor/export_preprocessor_handlers preprocessor/export_preprocessor_build_model  preprocessor/export_preprocessor_sets preprocessor/export_preprocessor_main solution/export_solution python_interface)
target_link_libraries(xc ${Boost_LIBRARIES} XcBib)
# don't prepend wrapper library name with lib
set_target_properties(xc PROPERTIES PREFIX "" )


INSTALL(TARGETS XcBib DESTINATION lib)
#INSTALL(DIRECTORY ${DIR_FUENTES_XC}/macros/ DESTINATION lib/macros_xc)

