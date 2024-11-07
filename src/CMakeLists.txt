cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_BUILD_TYPE to "Release")
#cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_VERSION_STRING ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})
IF (${CMAKE_VERSION_STRING} GREATER 3.20.0)
  cmake_policy(SET CMP0017 NEW) 
  cmake_policy(SET CMP0115 NEW) 
  MESSAGE ("CMake version is not greater than 3.20.0")
ELSE (${CMAKE_VERSION_STRING} GREATER 3.20.0)
  MESSAGE ("CMake version is not greater than 3.20.0")
ENDIF (${CMAKE_VERSION_STRING} GREATER 3.20.0)
set(OpenGL_GL_PREFERENCE "GLVND")

# Replaces -fPIC for all the targets.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Colorize output.
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# Project name.
PROJECT(LIBXC)

SET(build_setup_dir $ENV{HOME}/.xc_build)
SET(build_setup_file ${build_setup_dir}/xc_dirs.cmake)

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

# parse the version information into pieces.
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
#MESSAGE(STATUS "Boost_LIBRARIES: " ${Boost_LIBRARIES})

# Check GMSH version
if(GMSH_FOUND)
   if(GMSH_VERSION VERSION_LESS "4.8")
      message(STATUS "${BoldRed}GMSH 4.8 or later required (found version: ${GMSH_VERSION}).")
      message(FATAL_ERROR "See: https://github.com/xcfem/xc/blob/master/install/gmsh_install.md")
      message(FATAL_ERROR "to install a more recent version.${ColourReset}")
   endif(GMSH_VERSION VERSION_LESS "4.8")
endif(GMSH_FOUND)

# GMSH library
include_directories(${GMSH_INCLUDE_DIRS})

# HDF5 library
include_directories(${HDF5_HEADER_INCLUDE_DIR})

# Python libraries
SET(XC_UTILS_BOOST_LIBRARIES ${Boost_LIBRARIES})
list(APPEND XC_UTILS_BOOST_LIBRARIES ${XC_UTILS_BOOST_PYTHON_LIBRARIES})
list(REMOVE_DUPLICATES XC_UTILS_BOOST_LIBRARIES)
#MESSAGE(STATUS "Boost_LIBRARIES = ${XC_UTILS_BOOST_LIBRARIES}")

# VTK library
IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  message(STATUS "vtk file: " ${USE_VTK_FILE})
  SET(VTK_FOUND 1)
  SET(VTK_BIB vtkRendering vtkIO vtkHybrid)
ENDIF(USE_VTK_FILE)
include_directories(${VTK_INCLUDE_DIRS})
MESSAGE(STATUS "VTK include directories: ${VTK_INCLUDE_DIRS}")
MESSAGE(STATUS "VTK Libraries: ${VTK_LIBRARIES}")

# CGAL library
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

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
message(STATUS "MPI version: ${MPI_CXX_VERSION}")

#TCL library
INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})

# Portable, Extensible Toolkit for Scientific Computation
find_package(Petsc)
SET(PETSC_LIB_DIR ${PETSC_DIR}/${PETSC_ARCH}/lib)
INCLUDE_DIRECTORIES(${PETSC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PETSCCONF_INCLUDE_DIR})

# Eigen3 C++ template library for linear algebra.
find_package(Eigen3 NO_MODULE)
#set_package_properties(Eigen3 PROPERTIES TYPE REQUIRED PURPOSE "C++ vector data structures")
message(STATUS "Found Eigen3 Version: ${Eigen3_VERSION} path: ${Eigen3_DIR}")
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
message(STATUS "Eigen3 ${Eigen3_VERSION} include files path: ${EIGEN3_INCLUDE_DIR}")

find_package(Spectra NO_MODULE)
message(STATUS "Found Spectra path: ${SPECTRA_INCLUDE_DIR}")


#Python
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})

#XC library
INCLUDE_DIRECTORIES(${LIBXC_SOURCE_DIR})

# For DEBUG
#ADD_DEFINITIONS(-Wall -O0 -frounding-math -g)
# For RELEASE
# C++ flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 ${CMAKE_CXX_FLAGS_RELEASE} -Wall -march=native -pedantic -Wno-unused-but-set-variable ${MPI_CXX_COMPILE_OPTIONS} ${OpenMP_CXX_FLAGS}")
# Avoid pragma message about Boost placeholders
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_BIND_GLOBAL_PLACEHOLDERS")
# ignore BOOST deprecated headers
add_definitions("-DBOOST_ALLOW_DEPRECATED_HEADERS")
# Fortran flags.
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_RELEASE} -std=f95")
# C flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} ${OpenMP_C_FLAGS}")
# Linker flags.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS}")
MESSAGE(STATUS "**** compiler fortran flags: ${CMAKE_Fortran_FLAGS}")
MESSAGE(STATUS "**** compiler c flags: ${CMAKE_C_FLAGS}")
MESSAGE(STATUS "**** compiler cxx: flags ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "**** linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
#set(CMAKE_VERBOSE_MAKEFILE ON)

#Errores en arpack++
set_source_files_properties(solution/system_of_eqn/eigenSOE/BandArpackppSolver.cc PROPERTIES COMPILE_FLAGS -fpermissive)

# Source files.

## Utility
SET(text_utils utility/utils/text/text_string.cc utility/utils/text/LabelContainer.cc utility/utils/text/en_letra.cc utility/utils/text/StrTok.cc utility/utils/text/tab_cod.cc utility/utils/text/latex.cc utility/utils/text/StringFormatter.cc utility/utils/text/FixedWidthReader.cc)

SET(stream_utils utility/utils/streams/imanip.cc utility/utils/streams/prep.cc)

SET(misc_utils utility/utils/misc_utils/cmd_args.cc utility/utils/misc_utils/matem.cc utility/utils/misc_utils/memoria.cc utility/utils/misc_utils/inertia.cc utility/utils/misc_utils/print_trace.cc)

SET(SQLITE utility/sqlite/SqLiteObject.cc utility/sqlite/SqLiteQuery.cc utility/sqlite/SqLiteDatabase.cc)

SET(sqlitepp_utils utility/utils/sqlitepp/Database.cpp utility/utils/sqlitepp/Query.cpp utility/utils/sqlitepp/StderrLog.cpp utility/utils/sqlitepp/SysLog.cpp)

SET(matrices utility/matrices/MMatrix.cc utility/matrices/vcolM.cc utility/matrices/RangoIndice.cc utility/matrices/MatrixRange.cc utility/matrices/BoxConstRef.cc utility/matrices/ExpressionMatrix.cc utility/matrices/MExpressionMatrix.cc utility/matrices/PolynomialMatrix.cc utility/matrices/m_double.cc utility/matrices/m_bool.cc utility/matrices/m_double.cc utility/matrices/m_int.cc utility/matrices/m_sizet.cc utility/matrices/m_string.cc utility/matrices/giros.cc utility/matrices/op_tensor.cc utility/matrices/solvers/solver.cc utility/matrices/IndicesMatrix.cc)

SET(three_d_arrays utility/matrices/3d_arrays/Array3dBase.cc utility/matrices/3d_arrays/Array3dBoxRefBase.cc utility/matrices/3d_arrays/Array3dRange.cc utility/matrices/3d_arrays/BoolArray3d.cc utility/matrices/3d_arrays/Indices3dArray.cc)

SET(skypack utility/skypack/skypri.f utility/skypack/skypds.f utility/skypack/skyscf.f utility/skypack/skyssb.f utility/skypack/mtrxys.f utility/skypack/skyssf.f utility/skypack/skypsb.f utility/skypack/skypdc.f utility/skypack/skypf1.f utility/skypack/skysdc.f utility/skypack/skypsp.f utility/skypack/skypcf.f utility/skypack/skymef.f utility/skypack/skyssd.f utility/skypack/skyss.f utility/skypack/skysf1.f utility/skypack/skypf2.f utility/skypack/mtrxy.f utility/skypack/mtrxyd.f utility/skypack/skysf2.f utility/skypack/skypci.f)

SET(itpack utility/itpack/dsrc2c.f utility/itpack/jcg.f utility/itpack/jsi.f utility/itpack/rssi.f utility/itpack/rscg.f utility/itpack/ssorcg.f utility/itpack/sor.f utility/itpack/ssorsi.f)

SET(algebra utility/functions/algebra/TablaLiterales.cc utility/functions/algebra/ExprBase.cc utility/functions/algebra/ArbolExpr.cc utility/functions/algebra/ExprAlgebra.cc utility/functions/algebra/Expresion.cc utility/functions/algebra/ExprInfija.cc utility/functions/algebra/ExprPostfija.cc utility/functions/algebra/LexAlgebra.cc utility/functions/algebra/Lexico.cc utility/functions/algebra/Literal.cc utility/functions/algebra/OpBinario.cc utility/functions/algebra/OpComparacion.cc utility/functions/algebra/OpBooleano.cc utility/functions/algebra/Macaulay.cc utility/functions/algebra/Operando.cc utility/functions/algebra/OpUnario.cc utility/functions/algebra/ProtoExpr.cc utility/functions/algebra/Rama.cc utility/functions/algebra/Variable.cc utility/functions/algebra/Operador.cc utility/functions/algebra/Segnal.cc utility/functions/algebra/ExprFunctor.cc utility/functions/algebra/NamedOperand.cc utility/functions/algebra/OpTrigonometricas.cc utility/functions/algebra/ConjVariables.cc utility/functions/algebra/MapValores.cc utility/functions/algebra/num_integration.cc)

SET(functions_misc utility/functions/statistics/statlib.cc utility/functions/statistics/Sample.cc utility/functions/statistics/combinatorics.cc utility/functions/max_min/max_min.cc utility/functions/macaulay/Macaulay.cc utility/functions/polynomials/Polynomial.cc utility/functions/polynomials/IndPol.cc utility/functions/polynomials/NmbVars.cc utility/functions/roots/NewtonRaphson.cc utility/functions/roots/root.cc utility/functions/shape_functions/ShapeFunction.cc utility/functions/shape_functions/IntegrationPoints.cc)

SET(functions ${algebra} ${functions_misc})

SET(KERNEL utility/kernel/EntityWithProperties.cc utility/kernel/CommandEntity.cc utility/kernel/NamedEntity.cc utility/kernel/EntityWithOwner.cc utility/kernel/StandardStreamWrapper.cc utility/kernel/StandardOutputWrapper.cc utility/kernel/python_utils.cc utility/kernel/broke_property.cc)

#SET(VtkAux vtk_aux/vtkShStrArray vtk_aux/vtkLabeledShStrMapper vtk_aux/vtkTakePhotoDefs vtk_aux/vtkTakePhoto vtk_aux/vtkXOpenGLOffScreenRenderWindow vtk_aux/vtkDoubleHeadedArrowSource vtk_aux/vtkArcSource)

### Geometry
SET(geomTrfs utility/geom/trf/Trf.cc utility/geom/trf/Trf2d.cc utility/geom/trf/Rotation2d.cc utility/geom/trf/Trf3d.cc utility/geom/trf/Translation2d.cc utility/geom/trf/Scaling3d.cc utility/geom/trf/Scaling2d.cc utility/geom/trf/Identity2d.cc utility/geom/trf/Identity3d.cc utility/geom/trf/Translation3d.cc utility/geom/trf/Reflection2d.cc utility/geom/trf/Rotation3d.cc utility/geom/trf/Reflection3d.cc utility/geom/trf/Revolution3d.cc)

SET(geomRefCooSys utility/geom/coo_sys/CooSys.cc utility/geom/coo_sys/Rect1d3dCooSys.cc utility/geom/coo_sys/Rect2d3dCooSys.cc utility/geom/coo_sys/Rect3d3dCooSys.cc utility/geom/coo_sys/Rect1d2dCooSys.cc utility/geom/coo_sys/Rect2d2dCooSys.cc utility/geom/coo_sys/Xd3dCooSys.cc utility/geom/coo_sys/Xd2dCooSys.cc utility/geom/coo_sys/ref_sys/Ref1d2d.cc utility/geom/coo_sys/ref_sys/Ref3d3d.cc utility/geom/coo_sys/ref_sys/Ref2d2d.cc utility/geom/coo_sys/ref_sys/Ref1d3d.cc utility/geom/coo_sys/ref_sys/Ref2d3d.cc utility/geom/coo_sys/ref_sys/PrincipalAxesOfInertia2D.cc utility/geom/coo_sys/FrenetTrihedron.cc)

SET(geomD0 utility/geom/d0/Point3d.cc utility/geom/d0/Point2d.cc)

SET(geomD1 utility/geom/d1/Line3d.cc utility/geom/d1/Line2d.cc utility/geom/d1/Ray3d.cc utility/geom/d1/Segment2d.cc utility/geom/d1/Ray2d.cc utility/geom/d1/Segment3d.cc utility/geom/d1/Polyline2d.cc utility/geom/d1/PlanePolyline3d.cc utility/geom/d1/Polyline3d.cc)

SET(geomD2 utility/geom/d2/GeomGroup2d.cc utility/geom/d2/Surface2d.cc utility/geom/d2/Surface3d.cc utility/geom/d2/Plane.cc utility/geom/d2/BND2d.cc utility/geom/d2/Circle2d.cc utility/geom/d2/CircularSector2d.cc utility/geom/d2/CircularSector3d.cc utility/geom/d2/AnnulusSector2d.cc utility/geom/d2/GeomObj2d.cc utility/geom/d2/Polygon3d.cc utility/geom/d2/Quadrilateral3d.cc utility/geom/d2/2d_polygons/Polygon2d.cc utility/geom/d2/2d_polygons/PolygonWithHoles2d.cc utility/geom/d2/2d_polygons/PolygonalSurface2d.cc utility/geom/d2/2d_polygons/Triangle2d.cc utility/geom/d2/2d_polygons/carsten_steger.cc utility/geom/d2/2d_polygons/LineSegmentArrangement.cc utility/geom/d2/2d_polygons/plane_sweep.cc utility/geom/d2/2d_polygons/mark_bayazit.cc utility/geom/d2/2d_polygons/polygon2d_bool_op.cc utility/geom/d2/2d_polygons/Quadrilateral2d.cc utility/geom/d2/2d_polygons/Rectangle2d.cc utility/geom/d2/2d_polygons/Square2d.cc utility/geom/d2/HalfPlane2d.cc utility/geom/d2/Triangle3d.cc utility/geom/d2/Circle3d.cc utility/geom/d2/D2to3d.cc utility/geom/d2/Trihedron.cc utility/geom/d2/Triang3dMesh.cc utility/geom/d2/ConvexHull2d.cc utility/geom/d2/BasicAlphaShape2d.cc utility/geom/d2/GeneralEquationOfPlane.cc utility/geom/d2/Grid2d.cc)

SET(geomD3 utility/geom/d3/BND3d.cc utility/geom/d3/GeomObj3d.cc utility/geom/d3/BspTree3d.cc utility/geom/d3/Cylinder.cc utility/geom/d3/GeomGroup3d.cc utility/geom/d3/3d_polyhedrons/VertexMap.cc utility/geom/d3/3d_polyhedrons/PolyhedronBase.cc utility/geom/d3/3d_polyhedrons/Polyhedron3d.cc utility/geom/d3/3d_polyhedrons/PolygonMap.cc utility/geom/d3/3d_polyhedrons/modifier_quad_triangle.cc utility/geom/d3/3d_polyhedrons/enriched_polyhedron.cc utility/geom/d3/3d_polyhedrons/Tetrahedron3d.cc utility/geom/d3/3d_polyhedrons/Hexahedron3d.cc utility/geom/d3/3d_polyhedrons/BlockPyramid.cc utility/geom/d3/3d_polyhedrons/polyhedron3d_bool_op.cc utility/geom/d3/HalfSpace3d.cc utility/geom/d3/ConvexHull3d.cc utility/geom/d3/Solid3d.cc utility/geom/d3/3d_prisms/TriangularPrism3d.cc)

SET(funcFromPoints utility/geom/d1/function_from_points/FunctionFromPointsR_R.cc utility/geom/d1/function_from_points/FunctionFromPointsR2_R.cc)

SET(geom utility/geom/pos_vec/SlidingVector3d.cc utility/geom/pos_vec/FixedVector3d.cc utility/geom/pos_vec/Vector2d.cc utility/geom/pos_vec/Vector3d.cc utility/geom/pos_vec/Pos2d.cc utility/geom/pos_vec/Pos2dList.cc utility/geom/pos_vec/Pos3d.cc utility/geom/pos_vec/KDTreePos.cc utility/geom/pos_vec/Pos3dList.cc utility/geom/pos_vec/Dir3d.cc utility/geom/pos_vec/Dir2d.cc utility/geom/pos_vec/SlidingVector2d.cc utility/geom/pos_vec/FixedVector2d.cc utility/geom/pos_vec/SlidingVectorsSystem3d.cc utility/geom/pos_vec/SlidingVectorsSystem2d.cc utility/geom/pos_vec/Pos2dArray.cc utility/geom/pos_vec/Pos2dArray3d.cc utility/geom/pos_vec/Pos3dArray.cc utility/geom/pos_vec/Pos3dArray3d.cc utility/geom/lists/ThreePoints3d.cc utility/geom/lists/utils_list_pos2d.cc utility/geom/lists/utils_list_pos3d.cc utility/geom/Wm3/Wm3Math.cpp utility/geom/Wm3/Wm3Distance.cc utility/geom/Wm3/Wm3DistVector3Triangle3.cpp ${geomD0} ${geomD1} ${geomD2} ${geomD3} ${geomRefCooSys} ${geomTrfs} ${funcFromPoints} utility/geom/cgal_types.cc utility/geom/GeomObj.cc utility/geom/FT_matrix.cc utility/geom/ProtoGeom.cc)

### GNU GTS
SET(GnuGts utility/gnu_gts/GTSSurface.cc utility/gnu_gts/GTSVertex.cc utility/gnu_gts/GTSEdge.cc utility/gnu_gts/GTSFace.cc utility/gnu_gts/GTSSplit.cc utility/gnu_gts/GTSSurfaceTraverse.cc utility/gnu_gts/GTSSurfaceIntersection.cc utility/gnu_gts/GTSBBoxTree.cc utility/gnu_gts/GTSGraph.cc utility/gnu_gts/TriangleMap.cc)

### Gauss quadrature.
SET(cuadGauss utility/gauss_quadrature/GaussQuadrature2D.cc utility/gauss_quadrature/GaussQuadrature3D.cc utility/gauss_quadrature/gssleg.cc utility/gauss_quadrature/GaussPoints.cc utility/gauss_quadrature/GaussWeights1d.cc utility/gauss_quadrature/GaussWeights2d.cc utility/gauss_quadrature/GaussWeights3d.cc)

### Load combinations.
SET(ca_factors utility/load_combinations/actions/factors/PartialSafetyFactors.cc utility/load_combinations/actions/factors/PartialSafetyFactorsMap.cc utility/load_combinations/actions/factors/CombinationFactors.cc utility/load_combinations/actions/factors/CombinationFactorsMap.cc utility/load_combinations/actions/factors/Factors.cc)

SET(ca_actions utility/load_combinations/actions/LeadingActionInfo.cc utility/load_combinations/actions/Action.cc utility/load_combinations/actions/ActionRelationships.cc utility/load_combinations/actions/ActionRepresentativeValues.cc utility/load_combinations/actions/ActionWrapper.cc utility/load_combinations/actions/SingleActionWrapper.cc utility/load_combinations/actions/GroupActionWrapper.cc utility/load_combinations/actions/ActionWrapperList.cc)

SET(ca_action_containers utility/load_combinations/actions/containers/ActionsFamily.cc utility/load_combinations/actions/containers/ActionFamilyContainer.cc utility/load_combinations/actions/containers/ActionsAndFactors.cc)

SET(ca_combinations utility/load_combinations/comb_analysis/Variation.cc utility/load_combinations/comb_analysis/Variations.cc utility/load_combinations/comb_analysis/LoadCombinationVector.cc utility/load_combinations/comb_analysis/LoadCombinations.cc utility/load_combinations/comb_analysis/ActionWeightingMap.cc utility/load_combinations/comb_analysis/LoadCombinationGenerator.cc)

SET(ca_load_combinations ${ca_factors} ${ca_actions} ${ca_action_containers} ${ca_combinations})

### Actor
SET(actor utility/actor/actor/Actor.cpp utility/actor/actor/DistributedBase.cc utility/actor/actor/DistributedObj.cc utility/actor/actor/MovableObject.cpp utility/actor/actor/CommMetaData.cc utility/actor/actor/PtrCommMetaData.cc utility/actor/actor/BrokedPtrCommMetaData.cc utility/actor/actor/ArrayCommMetaData.cc utility/actor/actor/MatrixCommMetaData.cc utility/actor/actor/TensorCommMetaData.cc utility/actor/actor/DbTagData.cc utility/actor/actor/Communicator.cc utility/actor/actor/MovableMap.cc utility/actor/actor/MovableDeque.cc utility/actor/actor/MovableVector.cc utility/actor/actor/MovableBJTensor.cc utility/actor/actor/MovableString.cc utility/actor/actor/MovableVectors.cc utility/actor/actor/MovableIDs.cc utility/actor/actor/MovableMatrix.cc utility/actor/actor/MovableID.cc utility/actor/actor/MovableMatrices.cc utility/actor/actor/MovableContainer.cc utility/actor/actor/MovableStrings.cc utility/actor/address/ChannelAddress.cpp utility/actor/address/SocketAddress.cpp utility/actor/channel/ChannelQueue.cc utility/actor/channel/Channel.cpp  utility/actor/channel/Socket.cpp utility/actor/channel/TCP_UDP_Socket_base.cc utility/actor/channel/TCP_Socket.cpp utility/actor/channel/UDP_Socket.cpp utility/actor/channel/mySocket.c utility/actor/machineBroker/MachineBroker.cpp utility/actor/message/Message.cpp utility/actor/objectBroker/FEM_ObjectBroker.cpp utility/actor/objectBroker/ObjectBroker.cpp utility/actor/ObjectWithObjBroker.cc utility/actor/ShadowActorBase.cc utility/actor/shadow/Shadow.cpp utility/xc_python_utils.cc)

SET(mpi utility/actor/address/MPI_ChannelAddress.cpp utility/actor/channel/MPI_Channel.cpp utility/actor/machineBroker/MPI_MachineBroker.cpp)

SET(alpha_broker utility/actor/machineBroker/AlphaBaseMachineBroker.cc utility/actor/machineBroker/AlphaMachineBroker.cpp utility/actor/machineBroker/MillMachineBroker.cpp utility/actor/machineBroker/DecMachineBroker.cpp)

SET(tcp utility/actor/channel/TCP_SocketNoDelay.cc)

SET(database utility/database/FE_Datastore.cpp utility/database/FileDatastore.cpp utility/database/DBDatastore.cc utility/database/BerkeleyDbDatastore.cpp utility/database/MySqlDatastore.cpp utility/database/SQLiteDatastore.cc utility/database/NEESData.cpp utility/database/PyDictDatastore.cc )

IF(ORACLE_FOUND)
SET(database ${database} utility/database/OracleDatastore.cc)
ENDIF(ORACLE_FOUND)

set(paving utility/paving/Paver.cc utility/paving/bpinch.c utility/paving/pltnor.c utility/paving/filsmo.c utility/paving/pltdrw.c utility/paving/ndstat.c utility/paving/match2.c utility/paving/matchk.c utility/paving/pcross.c utility/paving/mport2.c utility/paving/pltsvv.c utility/paving/close2.c utility/paving/tridel.c utility/paving/qual3.c utility/paving/mpmul4.c utility/paving/getrow.c utility/paving/cpubrk.c utility/paving/addwdg.c utility/paving/pltcv2.c utility/paving/intsct.c utility/paving/eqlang.c utility/paving/fndlnk.c utility/paving/chrtrm.c utility/paving/getime.c utility/paving/setn02.c utility/paving/pltsup.c utility/paving/pltstg.c utility/paving/periml.c utility/paving/pltbel.c utility/paving/disctp.c utility/paving/longel.c utility/paving/mpd2vc.c utility/paving/pltrim.c utility/paving/extnd3.c utility/paving/siorpt.c utility/paving/setlop.c utility/paving/cornp.c utility/paving/lupang.c utility/paving/invert.c utility/paving/undelm.c utility/paving/mpmul2.c utility/paving/node12.c utility/paving/getdum.c utility/paving/pltvwp.c utility/paving/pltstv.c utility/paving/chric.c utility/paving/pltesc.c utility/paving/pltp2d.c utility/paving/jumplp.c utility/paving/addnod.c utility/paving/add2cn.c utility/paving/wedge.c utility/paving/pltvwv.c utility/paving/trifix.c utility/paving/nxkord.c utility/paving/pltcp2.c utility/paving/sflush.c utility/paving/gkxn.c utility/paving/grsnap.c utility/paving/pltsbm.c utility/paving/addlxn.c utility/paving/nickc.c utility/paving/pltstd.c utility/paving/add1cn.c utility/paving/lcolor.c utility/paving/ch3to4.c utility/paving/plticl.c utility/paving/close4.c utility/paving/getsiz.c utility/paving/b4bad.c utility/paving/chrrvc.c utility/paving/mxzero.c utility/paving/rowsmo.c utility/paving/pltfnt.c utility/paving/setcir.c utility/paving/chrcmp.c utility/paving/grabrt.c utility/paving/putlxn.c utility/paving/cpudac.c utility/paving/connod.c utility/paving/extnd1.c utility/paving/pltrsd.c utility/paving/pltxts.c utility/paving/snapit.c utility/paving/mnorm.c utility/paving/rplotl.c utility/paving/excpus.c utility/paving/add2nd.c utility/paving/pltstt.c utility/paving/getlxn.c utility/paving/pltfrm.c utility/paving/sew2.c utility/paving/pltflu.c utility/paving/pltgtt.c utility/paving/pltmov.c utility/paving/not_found.c utility/paving/add2el.c utility/paving/getfrm.c utility/paving/cntcrn.c utility/paving/invmap.c utility/paving/colaps.c utility/paving/dellxn.c utility/paving/adjrow.c utility/paving/chrup.c utility/paving/d2node.c utility/paving/keep3.c utility/paving/gnxka.c utility/paving/ringbl.c utility/paving/dlpara.c utility/paving/extnd5.c utility/paving/pltsub.c utility/paving/nsplit.c utility/paving/symbol.c utility/paving/pltbgn.c utility/paving/cpuifc.c utility/paving/pltitm.c utility/paving/flmnmx.c utility/paving/ugrcol.c utility/paving/marksm.c utility/paving/qual2n.c utility/paving/shrunk.c utility/paving/mpview.c utility/paving/getcrn.c utility/paving/sidep.c utility/paving/addkxl.c utility/paving/tuck.c utility/paving/spaced.c utility/paving/add3nd.c utility/paving/pinch.c utility/paving/paving.c utility/paving/pltlig.c utility/paving/pltrst.c utility/paving/vdicps.c utility/paving/chrci.c utility/paving/addrow.c utility/paving/vinter.c utility/paving/close6.c utility/paving/comsrt.c utility/paving/adjtri.c utility/paving/pltrev.c utility/paving/pltvcm.c utility/paving/pltxth.c utility/paving/nicks.c utility/paving/mxmult.c utility/paving/pltdv2.c utility/paving/qual4.c utility/paving/addtuk.c utility/paving/pltrsg.c utility/paving/fixlxn.c utility/paving/usrsym.c utility/paving/delem.c utility/paving/getang.c utility/paving/mpd2sy.c utility/paving/bcross.c utility/paving/common_block_declarations.c )

SET(handler utility/handler/DataOutputDatabaseHandler.cpp utility/handler/DataOutputFileHandler.cpp utility/handler/DataOutputHandler.cpp utility/handler/DataOutputStreamHandler.cpp utility/handler/FileStream.cpp utility/handler/OPS_Stream.cpp utility/handler/StandardStream.cpp)

SET(package utility/package/packages.cpp)

SET(recorder utility/recorder/DomainRecorderBase.cc utility/recorder/response/ElementResponse.cpp utility/recorder/response/CompositeResponse.cpp utility/recorder/response/FiberResponse.cpp utility/recorder/response/MaterialResponse.cpp utility/recorder/response/Response.cpp utility/recorder/AlgorithmIncrements.cpp utility/recorder/DamageRecorder.cpp utility/recorder/DatastoreRecorder.cpp utility/recorder/HandlerRecorder.cc utility/recorder/DriftRecorder.cpp utility/recorder/MeshCompRecorder.cc utility/recorder/ElementRecorderBase.cc utility/recorder/ElementRecorder.cpp utility/recorder/EnvelopeData.cc utility/recorder/EnvelopeElementRecorder.cpp utility/recorder/NodeRecorderBase.cc utility/recorder/NodeRecorder.cpp utility/recorder/EnvelopeNodeRecorder.cpp utility/recorder/FilePlotter.cpp utility/recorder/GSA_Recorder.cpp utility/recorder/MaxNodeDispRecorder.cpp utility/recorder/PatternRecorder.cpp utility/recorder/Recorder.cpp utility/recorder/PropRecorder.cc utility/recorder/NodePropRecorder.cc utility/recorder/ElementPropRecorder.cc utility/recorder/RecorderContainer.cc utility/recorder/ObjWithRecorders.cc)

SET(remote utility/remote/remote.c)

SET(tagged utility/tagged/storage/TaggedObjectStorage.cc utility/tagged/storage/ArrayOfTaggedObjects.cpp utility/tagged/storage/ArrayOfTaggedObjectsIter.cpp utility/tagged/storage/MapOfTaggedObjects.cpp utility/tagged/storage/MapOfTaggedObjectsIter.cpp utility/tagged/TaggedObject.cpp)

SET(nDarray utility/matrix/nDarray/basics.cpp utility/matrix/nDarray/BJtensor.cpp utility/matrix/nDarray/Cosseratstresst.cpp utility/matrix/nDarray/stress_strain_tensor.cc utility/matrix/nDarray/stresst.cpp utility/matrix/nDarray/BJvector.cpp utility/matrix/nDarray/nDarray.cpp utility/matrix/nDarray/BJmatrix.cpp utility/matrix/nDarray/Cosseratstraint.cpp utility/matrix/nDarray/straint.cpp)

SET(matrix utility/matrix/ID.cpp utility/matrix/IDVarSize.cc utility/matrix/IntPtrWrapper.cc utility/matrix/AuxMatrix.cc utility/matrix/Matrix.cpp utility/matrix/DqMatrices.cc utility/matrix/Vector.cpp utility/matrix/DqVectors.cc utility/matrix/util_matrix.cc ${nDarray})

SET(utility ${actor} ${mpi} ${alpha_broker} ${database} ${handler} ${package} ${paving} ${recorder} ${remote} ${tagged} ${matrix} utility/Timer.cpp)

SET(arclength_integrators solution/analysis/integrator/static/arc-length/ProtoArcLength.cc solution/analysis/integrator/static/arc-length/ArcLength1.cpp solution/analysis/integrator/static/arc-length/ArcLengthBase.cc solution/analysis/integrator/static/arc-length/ArcLength.cpp solution/analysis/integrator/static/arc-length/HSConstraint.cpp)

SET(static_integrators solution/analysis/integrator/static/IntegratorVectors.cc  solution/analysis/integrator/static/BaseControl.cc solution/analysis/integrator/static/DispBase.cc solution/analysis/integrator/static/DisplacementControl.cpp solution/analysis/integrator/static/LoadControl.cpp solution/analysis/integrator/static/DistributedDisplacementControl.cpp solution/analysis/integrator/static/LoadPath.cpp solution/analysis/integrator/static/EQPath.cpp solution/analysis/integrator/static/MinUnbalDispNorm.cpp ${arclength_integrators}) 

SET(transient_newmark_integrators solution/analysis/integrator/transient/NewmarkBase.cc solution/analysis/integrator/transient/newmark/NewmarkBase2.cc solution/analysis/integrator/transient/newmark/Newmark.cpp solution/analysis/integrator/transient/newmark/NewmarkHybridSimulation.cpp solution/analysis/integrator/transient/newmark/Newmark1.cpp solution/analysis/integrator/transient/newmark/NewmarkExplicit.cpp) 

SET(transient_integrators solution/analysis/integrator/transient/ResponseQuantities.cc solution/analysis/integrator/transient/CentralDifferenceBase.cc solution/analysis/integrator/transient/CentralDifferenceAlternative.cpp solution/analysis/integrator/transient/HHT1.cpp solution/analysis/integrator/transient/DampingFactorsIntegrator.cc ${transient_newmark_integrators} solution/analysis/integrator/transient/CentralDifferenceNoDamping.cpp solution/analysis/integrator/transient/RayleighBase.cc solution/analysis/integrator/transient/rayleigh/AlphaOSBase.cc solution/analysis/integrator/transient/rayleigh/CentralDifference.cpp solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.cc solution/analysis/integrator/transient/rayleigh/HHTBase.cc solution/analysis/integrator/transient/rayleigh/HHT.cpp solution/analysis/integrator/transient/rayleigh/HHTGeneralizedExplicit.cpp solution/analysis/integrator/transient/rayleigh/AlphaOS.cpp solution/analysis/integrator/transient/rayleigh/Collocation.cpp solution/analysis/integrator/transient/rayleigh/HHTExplicit.cpp solution/analysis/integrator/transient/rayleigh/HHTHybridSimulation.cpp solution/analysis/integrator/transient/rayleigh/AlphaOSGeneralized.cpp solution/analysis/integrator/transient/rayleigh/CollocationHybridSimulation.cpp solution/analysis/integrator/transient/rayleigh/HHTGeneralized.cpp solution/analysis/integrator/transient/rayleigh/WilsonTheta.cpp solution/analysis/integrator/transient/TRBDFBase.cc solution/analysis/integrator/transient/TRBDF2.cpp  solution/analysis/integrator/transient/TRBDF3.cpp) 

SET(eigen_integrators solution/analysis/integrator/eigen/LinearBucklingIntegrator.cc solution/analysis/integrator/eigen/KEigenIntegrator.cc)

SET(integrators solution/analysis/integrator/EigenIntegrator.cpp solution/analysis/integrator/Integrator.cpp solution/analysis/integrator/TransientIntegrator.cpp solution/analysis/integrator/IncrementalIntegrator.cpp solution/analysis/integrator/StaticIntegrator.cpp ${eigen_integrators} ${static_integrators} ${transient_integrators})

SET(analysis_eigen_algo solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.cpp solution/analysis/algorithm/eigenAlgo/FrequencyAlgo.cpp solution/analysis/algorithm/eigenAlgo/StandardEigenAlgo.cpp solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.cc solution/analysis/algorithm/eigenAlgo/KEigenAlgo.cc) 

SET(analysis_line_search solution/analysis/algorithm/equiSolnAlgo/line_search/NewtonLineSearch.cpp solution/analysis/algorithm/equiSolnAlgo/line_search/LineSearch.cpp solution/analysis/algorithm/equiSolnAlgo/line_search/BisectionLineSearch.cpp solution/analysis/algorithm/equiSolnAlgo/line_search/InitialInterpolatedLineSearch.cpp solution/analysis/algorithm/equiSolnAlgo/line_search/RegulaFalsiLineSearch.cpp solution/analysis/algorithm/equiSolnAlgo/line_search/SecantLineSearch.cpp) 

SET(analysis_algorithm solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.cpp solution/analysis/algorithm/SolutionAlgorithm.cpp solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase.cc solution/analysis/algorithm/equiSolnAlgo/BFGS.cpp solution/analysis/algorithm/equiSolnAlgo/Broyden.cpp solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.cpp solution/analysis/algorithm/equiSolnAlgo/EquiSolnConvAlgo.cc solution/analysis/algorithm/equiSolnAlgo/KrylovNewton.cpp solution/analysis/algorithm/equiSolnAlgo/Linear.cpp solution/analysis/algorithm/equiSolnAlgo/ModifiedNewton.cpp solution/analysis/algorithm/equiSolnAlgo/NewtonBased.cc solution/analysis/algorithm/equiSolnAlgo/NewtonRaphson.cpp solution/analysis/algorithm/equiSolnAlgo/PeriodicNewton.cpp ${analysis_line_search} ${analysis_eigen_algo})

SET(analysis_handlers solution/analysis/handler/ConstraintHandler.cpp solution/analysis/handler/FactorsConstraintHandler.cc solution/analysis/handler/LagrangeConstraintHandler.cpp solution/analysis/handler/PenaltyConstraintHandler.cpp solution/analysis/handler/PlainHandler.cpp solution/analysis/handler/TransformationConstraintHandler.cpp solution/analysis/handler/AutoConstraintHandler.cpp) 

SET(analysis solution/analysis/analysis/Analysis.cpp solution/analysis/analysis/DirectIntegrationAnalysis.cpp solution/analysis/analysis/DomainDecompositionAnalysis.cpp solution/analysis/analysis/EigenAnalysis.cpp solution/analysis/analysis/ModalAnalysis.cc solution/analysis/analysis/LinearBucklingEigenAnalysis.cc solution/analysis/analysis/IllConditioningAnalysis.cc solution/analysis/analysis/LinearBucklingAnalysis.cc solution/analysis/analysis/StaticAnalysis.cpp solution/analysis/analysis/StaticDomainDecompositionAnalysis.cpp solution/analysis/analysis/SubstructuringAnalysis.cpp solution/analysis/analysis/TransientAnalysis.cpp solution/analysis/analysis/TransientDomainDecompositionAnalysis.cpp solution/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.cpp solution/analysis/model/dof_grp/DOF_Group.cpp solution/analysis/model/dof_grp/LagrangeDOF_Group.cpp solution/analysis/model/dof_grp/TransformationDOF_Group.cpp solution/analysis/model/fe_ele/MPSPBaseFE.cc solution/analysis/model/fe_ele/SFreedom_FE.cc solution/analysis/model/fe_ele/MPBase_FE.cc solution/analysis/model/fe_ele/MFreedom_FE.cc solution/analysis/model/fe_ele/MRMFreedom_FE.cc solution/analysis/model/fe_ele/lagrange/Lagrange_FE.cc solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.cpp solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.cc solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.cpp solution/analysis/model/UnbalAndTangentStorage.cc solution/analysis/model/UnbalAndTangent.cc solution/analysis/model/fe_ele/FE_Element.cpp solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.cpp solution/analysis/model/fe_ele/penalty/PenaltyMRMFreedom_FE.cc solution/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.cpp solution/analysis/model/fe_ele/transformation/TransformationFE.cpp solution/analysis/model/AnalysisModel.cpp solution/analysis/model/DOF_GrpIter.cpp solution/analysis/model/DOF_GrpConstIter.cc solution/analysis/model/FE_EleIter.cpp solution/analysis/model/FE_EleConstIter.cc solution/analysis/numberer/DOF_Numberer.cpp solution/analysis/numberer/ParallelNumberer.cpp solution/analysis/numberer/PlainNumberer.cpp ${analysis_handlers} ${analysis_algorithm} ${integrators})

SET(convergenceTest solution/analysis/convergenceTest/CTestEnergyIncr.cpp solution/analysis/convergenceTest/CTestFixedNumIter.cpp solution/analysis/convergenceTest/CTestNormDispIncr.cpp solution/analysis/convergenceTest/CTestNormUnbalance.cpp solution/analysis/convergenceTest/CTestRelativeEnergyIncr.cpp solution/analysis/convergenceTest/CTestRelativeNormDispIncr.cpp solution/analysis/convergenceTest/CTestRelativeNormUnbalance.cpp solution/analysis/convergenceTest/CTestRelativeTotalNormDispIncr.cpp solution/analysis/convergenceTest/ConvergenceTest.cpp solution/analysis/convergenceTest/ConvergenceTestTol.cc solution/analysis/convergenceTest/ConvergenceTestNorm.cc) 

SET(coordTransformation domain/mesh/element/utils/coordTransformation/CrdTransf.cpp domain/mesh/element/utils/coordTransformation/CrdTransf2d.cpp domain/mesh/element/utils/coordTransformation/CrdTransf3d.cpp domain/mesh/element/utils/coordTransformation/LinearCrdTransf2d.cpp domain/mesh/element/utils/coordTransformation/SmallDispCrdTransf3d.cc domain/mesh/element/utils/coordTransformation/LinearCrdTransf3d.cpp domain/mesh/element/utils/coordTransformation/SmallDispCrdTransf2d.cc domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf2d.cpp domain/mesh/element/utils/coordTransformation/PDeltaCrdTransf3d.cpp domain/mesh/element/utils/coordTransformation/CorotCrdTransf2d.cpp domain/mesh/element/utils/coordTransformation/CorotCrdTransf3d.cpp domain/mesh/element/utils/coordTransformation/R3vectors.cpp domain/mesh/element/utils/coordTransformation/ShellUpBasisCrdTransf3d.cc domain/mesh/element/utils/coordTransformation/ShellCrdTransf3dBase.cc domain/mesh/element/utils/coordTransformation/ShellLinearCrdTransf3d.cc domain/mesh/element/utils/coordTransformation/ShellNLCrdTransf3d.cc domain/mesh/element/utils/coordTransformation/ASDMath.cc domain/mesh/element/utils/coordTransformation/ASDEICR.cc domain/mesh/element/utils/coordTransformation/ASDShellQ4LocalCoordinateSystem.cc domain/mesh/element/utils/coordTransformation/ASDShellQ4Transformation.cc domain/mesh/element/utils/coordTransformation/ASDShellQ4CorotationalTransformation.cc domain/mesh/element/utils/coordTransformation/coordinate_transformation_class_names.cc) 

SET(domain_load domain/load/beam_loads/BeamLoad.cc domain/load/beam_loads/BeamMecLoad.cc domain/load/beam_loads/BeamUniformLoad.cc domain/load/beam_loads/BeamStrainLoad.cc domain/load/beam_loads/BeamPointLoad.cc domain/load/beam_loads/Beam2dPointLoad.cpp domain/load/beam_loads/TrussStrainLoad.cc domain/load/beam_loads/Beam2dUniformLoad.cpp domain/load/beam_loads/Beam2dPartialUniformLoad.cpp domain/load/beam_loads/Beam3dPointLoad.cpp domain/load/beam_loads/Beam3dUniformLoad.cpp domain/load/plane/BidimLoad.cc domain/load/plane/BidimStrainLoad.cc domain/load/plane/QuadStrainLoad.cc domain/load/plane/ShellStrainLoad.cc domain/load/plane/BidimMecLoad.cc domain/load/plane/QuadMecLoad.cc domain/load/plane/QuadRawLoad.cc domain/load/plane/ShellMecLoad.cc domain/load/plane/ShellRawLoad.cc domain/load/plane/ShellUniformLoad.cc domain/load/volumetric/SelfWeight.cpp domain/load/volumetric/BrickSelfWeight.cpp domain/load/volumetric/ThreedimLoad.cc domain/load/volumetric/ThreedimMecLoad.cc domain/load/volumetric/BrickMecLoad.cc domain/load/volumetric/BrickRawLoad.cc domain/load/volumetric/ThreedimStrainLoad.cc domain/load/volumetric/BrickStrainLoad.cc domain/load/elem_load.cc domain/load/ElementalLoad.cpp domain/load/ElementBodyLoad.cc domain/load/SurfaceLoad.cpp domain/load/ElementalLoadIter.cpp domain/load/ElementPtrs.cc domain/load/Load.cpp domain/load/NodalLoad.cpp domain/load/NodalLoadIter.cpp) 

SET(domain_pattern_time_series domain/load/pattern/time_series/CFactorSeries.cc domain/load/pattern/time_series/ConstantSeries.cpp domain/load/pattern/time_series/DiscretizedRandomProcessSeries.cpp domain/load/pattern/time_series/SimulatedRandomProcessSeries.cpp domain/load/pattern/time_series/PathSeriesBase.cc domain/load/pattern/time_series/PathTimeSeries.cpp domain/load/pattern/time_series/PulseBaseSeries.cc domain/load/pattern/time_series/PeriodSeries.cc domain/load/pattern/time_series/PulseSeries.cpp domain/load/pattern/time_series/RectangularSeries.cpp domain/load/pattern/time_series/LinearSeries.cpp domain/load/pattern/time_series/PathSeries.cpp domain/load/pattern/time_series/TriangleSeries.cpp domain/load/pattern/time_series/TrigSeries.cpp domain/load/pattern/time_series/time_series_class_names.cc) 

SET(domain_pattern_load_patterns domain/load/pattern/load_patterns/EQBasePattern.cc domain/load/pattern/load_patterns/EarthquakePattern.cpp domain/load/pattern/load_patterns/PBowlLoading.cpp domain/load/pattern/load_patterns/UniformExcitation.cpp domain/load/pattern/load_patterns/MultiSupportPattern.cpp domain/load/pattern/load_patterns/load_pattern_class_names.cc) 

SET(domain_pattern_time_series_integ domain/load/pattern/time_series_integrator/TrapezoidalTimeSeriesIntegrator.cpp) 

SET(domain_pattern domain/load/pattern/NodeLocker.cc domain/load/pattern/NodeLockerIter.cc domain/load/pattern/LoadContainer.cc domain/load/pattern/LoadPattern.cpp domain/load/pattern/MapLoadPatterns.cc domain/load/pattern/LoadPatternCombination.cc domain/load/pattern/LoadCombination.cc domain/load/pattern/LoadCombinationGroup.cc domain/load/pattern/LoadPatternIter.cpp domain/load/pattern/TimeSeries.cpp domain/load/pattern/TimeSeriesIntegrator.cpp ${domain_pattern_time_series_integ} ${domain_pattern_time_series} ${domain_pattern_load_patterns})

SET(domain_ground_motion domain/load/groundMotion/MotionHistory.cc domain/load/groundMotion/GroundMotion.cpp domain/load/groundMotion/DqGroundMotions.cc domain/load/groundMotion/GroundMotionRecord.cpp domain/load/groundMotion/InterpolatedGroundMotion.cpp domain/load/groundMotion/ground_motion_class_names.cc) 

SET(domain_constraints domain/constraints/ImposedMotionBase.cc domain/constraints/ImposedMotionSP.cpp domain/constraints/MFreedom_ConstraintBase.cc domain/constraints/MFreedom_Constraint.cpp domain/constraints/MRMFreedom_Constraint.cc domain/constraints/GlueNodeToElement.cc domain/constraints/MFreedom_Joint.cc domain/constraints/MFreedom_Joint2D.cpp domain/constraints/MFreedom_Joint3D.cpp domain/constraints/Constraint.cc domain/constraints/SFreedom_Constraint.cpp domain/constraints/SFreedom_ConstraintIter.cc domain/constraints/MFreedom_ConstraintIter.cc domain/constraints/MRMFreedom_ConstraintIter.cc domain/constraints/ImposedMotionSP1.cpp domain/constraints/RigidBase.cc domain/constraints/RigidRod.cpp domain/constraints/RigidDiaphragm.cpp domain/constraints/RigidBeam.cpp domain/constraints/EqualDOF.cpp domain/constraints/ConstrContainer.cc domain/constraints/constraint_class_names.cc) 

SET(domain_component domain/component/DomainComponent.cpp domain/component/ForceReprComponent.cc domain/component/ContinuaReprComponent.cc domain/component/Parameter.cpp domain/component/MatParameter.cpp) 

SET(domain_subdomain_modelbuilder domain/domain/subdomain/modelbuilder/ModelBuilder.cpp domain/domain/subdomain/modelbuilder/PartitionedModelBuilder.cpp) 

SET(domain_subdomain ${domain_subdomain_modelbuilder} domain/domain/subdomain/ActorSubdomain.cpp domain/domain/subdomain/ShadowSubdomain.cpp domain/domain/subdomain/Subdomain.cpp domain/domain/subdomain/SubdomainNodIter.cpp) 

SET(domain ${domain_component} domain/domain/PseudoTimeTracker.cc domain/domain/partitioned/PartitionedDomain.cpp domain/domain/partitioned/PartitionedDomainEleIter.cpp domain/domain/partitioned/PartitionedDomainSubIter.cpp domain/domain/Domain.cpp domain/domain/single/SingleDomAllSFreedom_Iter.cpp domain/domain/single/SingleDomEleIter.cpp domain/domain/single/SingleDomLC_Iter.cpp domain/domain/single/SingleDomMFreedom_Iter.cpp domain/domain/single/SingleDomMRMFreedom_Iter.cc domain/domain/single/SingleDomNodIter.cpp domain/domain/single/SingleDomParamIter.cpp domain/domain/single/SingleDomSFreedom_Iter.cpp ${domain_ground_motion} ${domain_load} domain/mesh/MeshComponentContainer.cc domain/mesh/Mesh.cc domain/mesh/MeshEdge.cc domain/mesh/MeshEdges.cc domain/mesh/NodeLockers.cc domain/mesh/MeshComponent.cc domain/mesh/node/DummyNode.cpp domain/mesh/node/NodeVectors.cc domain/mesh/node/NodeDispVectors.cc domain/mesh/node/NodeVelVectors.cc domain/mesh/node/NodeAccelVectors.cc domain/mesh/node/Node.cpp  domain/mesh/node/node_class_names.cc domain/mesh/node/KDTreeNodes.cc domain/mesh/node/NodeTopology.cc domain/partitioner/NodeLocations.cc domain/partitioner/DomainPartitioner.cpp domain/partitioner/loadBalancer/LoadBalancer.cpp domain/partitioner/loadBalancer/ReleaseHeavierToLighterNeighbours.cpp domain/partitioner/loadBalancer/ShedHeaviest.cpp domain/partitioner/loadBalancer/SwapHeavierToLighterNeighbours.cpp ${domain_pattern} domain/mesh/region/DqMeshRegion.cc domain/mesh/region/MeshRegion.cpp ${domain_subdomain} ${domain_constraints})

SET(trusses domain/mesh/element/truss_beam_column/truss/ProtoTruss.cc domain/mesh/element/truss_beam_column/truss/TrussBase.cc domain/mesh/element/truss_beam_column/truss/Truss.cpp domain/mesh/element/truss_beam_column/truss/CorotTrussBase.cc domain/mesh/element/truss_beam_column/truss/CorotTruss.cpp domain/mesh/element/truss_beam_column/truss/CorotTrussSection.cpp domain/mesh/element/truss_beam_column/truss/TrussSection.cpp domain/mesh/element/truss_beam_column/truss/Spring.cc)

SET(plastic_hinge_beam_integration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/PlasticLengthsBeamIntegration.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/DistHingeIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration2d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeBeamIntegration2d.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration3d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeBeamIntegration3d.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeEndpointBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration2d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration2d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration3d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration3d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration2d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegrationBase.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration3d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration.cpp) 

SET(beam_integration domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegrationBase.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/TrapezoidalBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/ChebyshevBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/ParameterIDBeamIntegration.cc domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/FixedLocationBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/MidDistanceBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LowOrderBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/CompositeSimpsonBeamIntegration.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/IntegrationPointsCoords.cc ${plastic_hinge_beam_integration})

SET(cyclic_model domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/CyclicModel.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/LinearCyclic.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/BilinearCyclic.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/QuadraticCyclic.cpp) 

SET(fvectors domain/mesh/element/utils/fvectors/FVector.cc domain/mesh/element/utils/fvectors/FVectorBeamColumn2d.cc domain/mesh/element/utils/fvectors/FVectorBeamColumn3d.cc domain/mesh/element/utils/fvectors/FVectorQuad.cc domain/mesh/element/utils/fvectors/FVectorBrick.cc domain/mesh/element/utils/fvectors/FVectorShell.cc) 

SET(gauss_models domain/mesh/element/utils/gauss_models/GaussPoint.cc domain/mesh/element/utils/gauss_models/GaussModel.cc) 

SET(beams ${fvectors} domain/mesh/element/truss_beam_column/SectionMatrices.cc domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.cc domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.cc domain/mesh/element/truss_beam_column/NLForceBeamColumn3dBase.cc domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase.cc domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d.cc domain/mesh/element/truss_beam_column/EsfBeamColumn3d.cc domain/mesh/element/truss_beam_column/ProtoBeam2d.cc domain/mesh/element/truss_beam_column/ProtoBeam3d.cc domain/mesh/element/truss_beam_column/beam2d/beam2d.cc domain/mesh/element/truss_beam_column/beam2d/beam2d02.cpp domain/mesh/element/truss_beam_column/beam2d/beam2d03.cpp domain/mesh/element/truss_beam_column/beam2d/beam2d04.cpp domain/mesh/element/truss_beam_column/beam3d/beam3dBase.cc domain/mesh/element/truss_beam_column/beam3d/beam3d01.cpp domain/mesh/element/truss_beam_column/beam3d/beam3d02.cpp domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.cpp domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.cpp domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2dBase.cc domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.cpp domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumnNL2d.cpp domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.cpp domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2dBase.cc domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.cpp domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam2d.cpp domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3dBase.cc domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.cpp domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam3d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.cpp domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussLobattoQuadRule1d01.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d01.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule1d.cpp domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule1d01.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Elastic2DGNL.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS01.cpp  domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS02.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS03.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.cpp domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.cpp) 

SET(volumetric_elements domain/mesh/element/volumetric/BrickBase.cc domain/mesh/element/volumetric/20nbrick/TwentyNodeBrick.cpp domain/mesh/element/volumetric/20nbrick/Twenty_Node_Brick.cpp domain/mesh/element/volumetric/8nbrick/EightNodeBrick.cpp domain/mesh/element/volumetric/27nbrick/TwentySevenNodeBrick.cpp domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.cpp domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD8NodeBrick.cpp domain/mesh/element/volumetric/UP-ucsd/BrickUP.cpp domain/mesh/element/volumetric/UP-ucsd/TwentyEightNodeBrickUP.cpp domain/mesh/element/volumetric/upU/EightNodeBrick_u_p_U.cpp domain/mesh/element/volumetric/upU/TwentyNodeBrick_u_p_U.cpp domain/mesh/element/volumetric/brick/BbarBrick.cpp domain/mesh/element/volumetric/brick/Brick.cpp domain/mesh/element/volumetric/brick/shp3d.cpp domain/mesh/element/volumetric/UP-ucsd/shp3dv.cpp)

SET(plane_element domain/mesh/element/plane/PlaneElement.cc domain/mesh/element/plane/TriBase3N.cc domain/mesh/element/plane/triangle/Tri31.cpp domain/mesh/element/plane/QuadBase9N.cc domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.cpp domain/mesh/element/plane/SolidMech4N.cc domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.cpp domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.cpp domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.cpp domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.cpp domain/mesh/element/plane/UP-ucsd/Nine_Four_Node_QuadUP.cpp domain/mesh/element/plane/shell/ShellBData.cc domain/mesh/element/plane/shell/ShellMITC4Base.cc domain/mesh/element/plane/shell/Shell4NBase.cc domain/mesh/element/plane/shell/ShellMITC4.cpp domain/mesh/element/plane/shell/ShellMITC9.cpp domain/mesh/element/plane/shell/ShellNLDKGQ.cpp domain/mesh/element/plane/shell/ASDShellQ4.cpp) 

SET(frictionBearing domain/mesh/element/special/frictionBearing/frictionModel/FrictionModel.cpp domain/mesh/element/special/frictionBearing/frictionModel/CoulombFriction.cpp domain/mesh/element/special/frictionBearing/frictionModel/VDependentFriction.cpp domain/mesh/element/special/frictionBearing/frictionModel/VPDependentFriction.cpp domain/mesh/element/special/frictionBearing/frictionModel/FrictionResponse.cpp domain/mesh/element/special/frictionBearing/FrictionElementBase.cc domain/mesh/element/special/frictionBearing/FlatSliderSimple2d.cpp domain/mesh/element/special/frictionBearing/FlatSliderSimple3d.cpp domain/mesh/element/special/frictionBearing/SingleFPSimple3d.cpp domain/mesh/element/special/frictionBearing/SingleFPSimple2d.cpp)

SET(uwElements domain/mesh/element/uw_elements/BeamContact2D.cpp domain/mesh/element/uw_elements/BeamContact3D.cpp)

SET(physical_properties domain/mesh/element/utils/physical_properties/PhysicalProperties.cc domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties.cc domain/mesh/element/utils/physical_properties/SolidMech2D.cc domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties.cc domain/mesh/element/utils/physical_properties/BeamColumnJointPhysicalProperties.cc domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties.cc domain/mesh/element/utils/physical_properties/ElasticSectionPhysicalProperties.cc domain/mesh/element/utils/physical_properties/ElasticSection1dPhysicalProperties.cc domain/mesh/element/utils/physical_properties/ElasticSection2dPhysicalProperties.cc domain/mesh/element/utils/physical_properties/ElasticSection3dPhysicalProperties.cc domain/mesh/element/utils/physical_properties/Joint2DPhysicalProperties.cc domain/mesh/element/utils/physical_properties/Joint3DPhysicalProperties.cc domain/mesh/element/utils/physical_properties/SpringModels.cc domain/mesh/element/utils/physical_properties/DamageModels.cc domain/mesh/element/utils/physical_properties/DamageModelVector.cc domain/mesh/element/utils/physical_properties/Contact2D.cc domain/mesh/element/utils/physical_properties/Contact3D.cc) 

SET(body_forces domain/mesh/element/utils/body_forces/BodyForces.cc domain/mesh/element/utils/body_forces/BodyForces2D.cc domain/mesh/element/utils/body_forces/BodyForces3D.cc)

SET(surface_pressures domain/mesh/element/plane/surface_pressures/QuadSurfaceLoad.cc domain/mesh/element/plane/surface_pressures/BrickSurfaceLoad.cpp)

SET(element ${physical_properties} ${body_forces} ${surface_pressures} domain/mesh/element/Element.cpp domain/mesh/element/utils/ParticlePos2d.cc domain/mesh/element/utils/ParticlePos3d.cc domain/mesh/element/utils/KDTreeElements.cc domain/mesh/element/utils/RayleighDampingFactors.cc domain/mesh/element/Element0D.cc domain/mesh/element/Element1D.cc domain/mesh/element/utils/NodePtrs.cc domain/mesh/element/utils/NodePtrsWithIDs.cc domain/mesh/element/utils/Information.cpp domain/mesh/element/NewElement.cpp ${beams} ${beam_integration} ${volumetric_elements} ${plane_element} domain/mesh/element/special/joint/BeamColumnJoint2d.cpp domain/mesh/element/special/joint/BeamColumnJoint3d.cpp domain/mesh/element/special/joint/Joint2D.cpp domain/mesh/element/special/joint/Joint3D.cpp ${trusses} domain/mesh/element/zero_length/ZeroLength.cpp domain/mesh/element/zero_length/ZeroLengthContact.cc domain/mesh/element/zero_length/ZeroLengthContact2D.cpp domain/mesh/element/zero_length/ZeroLengthContact3D.cpp domain/mesh/element/zero_length/ZeroLengthSection.cpp ${frictionBearing} ${uwElements} domain/mesh/element/element_class_names.cc)

SET(element_feap domain/mesh/element/feap/fElement.cpp domain/mesh/element/feap/fElmt02.cpp domain/mesh/element/feap/fElmt05.cpp) 

SET(graph solution/graph/graph/ModelGraph.cc solution/graph/graph/ArrayGraph.cpp solution/graph/graph/ArrayVertexIter.cpp solution/graph/graph/DOF_Graph.cpp solution/graph/graph/DOF_GroupGraph.cpp solution/graph/graph/Graph.cpp solution/graph/graph/Vertex.cpp solution/graph/graph/VertexIter.cpp solution/graph/numberer/GraphNumberer.cpp solution/graph/numberer/MyRCM.cpp solution/graph/numberer/RCM.cpp solution/graph/numberer/AMD.cpp solution/graph/numberer/BaseNumberer.cc solution/graph/numberer/SimpleNumberer.cpp solution/graph/partitioner/Metis.cpp) 

SET(graph2 solution/graph/graph/FE_VertexIter.cpp solution/graph/numberer/MetisNumberer.cpp) 

SET(damage material/damage/DamageModel.cpp material/damage/DamageResponse.cpp material/damage/HystereticEnergy.cpp material/damage/Kratzig.cpp material/damage/Mehanny.cpp material/damage/NormalizedPeak.cpp material/damage/ParkAng.cpp) 

SET(feap_material material/nD/FeapMaterial.cpp material/nD/feap/FeapMaterial01.cpp material/nD/feap/FeapMaterial02.cpp material/nD/feap/FeapMaterial03.cpp) 

SET(section_geometry_repres material/section/repres/SectionMassProperties.cc  material/section/repres/section_geometry/region/RegionContainer.cc material/section/repres/section_geometry/region/SectRegion.cc material/section/repres/section_geometry/region/QuadCellRegion.cc material/section/repres/section_geometry/region/PolygonSectRegion.cc material/section/repres/section_geometry/region/CircularSectRegion.cc material/section/repres/section_geometry/region/QuadSectRegion.cc material/section/repres/section_geometry/reinfBar/ReinfBar.cpp material/section/repres/section_geometry/reinfBar/VectorReinfBar.cc material/section/repres/section_geometry/reinfLayer/ListReinfLayer.cc material/section/repres/section_geometry/reinfLayer/ReinfLayer.cpp material/section/repres/section_geometry/reinfLayer/CircReinfLayer.cpp material/section/repres/section_geometry/reinfLayer/SingleBar.cc material/section/repres/section_geometry/reinfLayer/StraightReinfLayer.cpp material/section/repres/section_geometry/Spot.cc material/section/repres/section_geometry/Axis.cc material/section/repres/section_geometry/Segment.cc material/section/repres/section_geometry/SectionGeometryWrapper.cc material/section/repres/section_geometry/SectionReferenceFrame.cc material/section/repres/section_geometry/SectionCartesianReferenceFrame.cc material/section/repres/section_geometry/SectionGeometry.cc)

SET(section_material_repres material/section/repres/cell/VectorCells.cc material/section/repres/cell/Cell.cpp material/section/repres/cell/QuadCell.cpp material/section/repres/cell/TriangCell.cc material/section/repres/DiscretBase.cc material/section/repres/section/FiberSectionRepr.cpp material/section/repres/section/FiberData.cc material/section/repres/section/SectionRepres.cpp material/section/repres/CrossSectionProperties1d.cc material/section/repres/CrossSectionProperties2d.cc material/section/repres/CrossSectionProperties3d.cc ${section_geometry_repres})

SET(section_plate_material material/section/plate_section/PlateBase.cc material/section/plate_section/ElasticPlateBase.cc material/section/plate_section/ElasticPlateProto.cc material/section/plate_section/ElasticMembranePlateSection.cpp material/section/plate_section/ElasticPlateSection.cpp material/section/plate_section/MembranePlateFiberSectionBase.cc material/section/plate_section/MembranePlateFiberSection.cpp material/section/plate_section/LayeredShellFiberSection.cpp)

SET(fiber_section_material material/section/fiber_section/FiberSectionBase.cc material/section/fiber_section/FiberSection2d.cpp material/section/fiber_section/FiberSection3dBase.cc material/section/fiber_section/FiberSection3d.cpp material/section/fiber_section/FiberSectionGJ.cpp material/section/fiber_section/FiberSectionShear2d.cc material/section/fiber_section/FiberSectionShear3d.cc)

SET(elastic_section_material material/section/elastic_section/BaseElasticSection.cc material/section/elastic_section/BaseElasticSection1d.cc material/section/elastic_section/ElasticSection1d.cpp material/section/elastic_section/BaseElasticSection2d.cc material/section/elastic_section/BaseElasticSection3d.cc material/section/elastic_section/ElasticSection2d.cpp material/section/elastic_section/ElasticShearSection2d.cpp material/section/elastic_section/ElasticSection3d.cpp material/section/elastic_section/ElasticShearSection3d.cpp)

SET(section_material material/section/interaction_diagram/DeformationPlane.cc material/section/interaction_diagram/PivotsUltimateStrains.cc material/section/interaction_diagram/InteractionDiagramData.cc material/section/interaction_diagram/NormalStressStrengthParameters.cc material/section/interaction_diagram/NMPointCloud.cc material/section/interaction_diagram/NMPointCloudBase.cc material/section/interaction_diagram/NMyMzPointCloud.cc material/section/interaction_diagram/Pivots.cc material/section/interaction_diagram/ComputePivots.cc material/section/interaction_diagram/ClosedTriangleMesh.cc material/section/interaction_diagram/InteractionDiagram2d.cc material/section/interaction_diagram/InteractionDiagram.cc material/section/fiber_section/fiber/Fiber.cpp material/section/fiber_section/fiber/FiberSet.cc material/section/fiber_section/fiber/FiberPtrDeque.cc material/section/fiber_section/fiber/FiberSets.cc material/section/fiber_section/fiber/FiberContainer.cc material/section/fiber_section/fiber/UniaxialFiber.cc material/section/fiber_section/fiber/UniaxialFiber2d.cpp material/section/fiber_section/fiber/UniaxialFiber3d.cpp material/section/Bidirectional.cpp ${elastic_section_material} ${fiber_section_material} material/section/GenericSection1d.cpp material/section/GenericSectionNd.cpp material/section/Isolator2spring.cpp material/section/AggregatorAdditions.cc material/section/SectionAggregator.cpp material/section/ResponseId.cc material/section/CrossSectionKR.cc material/section/PrismaticBarCrossSectionsVector.cc material/section/SectionForceDeformation.cpp material/section/PrismaticBarCrossSection.cc ${section_material_repres} material/section/yieldSurface/YS_Section2D01.cpp material/section/yieldSurface/YS_Section2D02.cpp material/section/yieldSurface/YieldSurfaceSection2d.cpp ${section_plate_material} material/section/section_material_class_names.cc)

SET(nD_elastic_isotropic material/nD/elastic_isotropic/ElasticIsotropic3D.cpp material/nD/elastic_isotropic/ElasticIsotropicAxiSymm.cpp material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.cpp material/nD/elastic_isotropic/ElasticIsotropicMaterial.cpp material/nD/elastic_isotropic/ElasticIsotropic2D.cc material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.cpp material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.cpp material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.cpp material/nD/elastic_isotropic/PressureDependentElastic3D.cpp)

SET(nD_j2_plasticity material/nD/j2_plasticity/J2AxiSymm.cpp material/nD/j2_plasticity/J2PlaneStrain.cpp material/nD/j2_plasticity/J2PlaneStress.cpp material/nD/j2_plasticity/J2Plasticity.cpp material/nD/j2_plasticity/J2PlateFiber.cpp material/nD/j2_plasticity/J2PlateFibre.cpp material/nD/j2_plasticity/J2ThreeDimensional.cpp)

SET(nD_uvmaterial material/nD/uw_materials/DruckerPrager.cpp material/nD/uw_materials/DruckerPragerPlaneStrain.cpp material/nD/uw_materials/DruckerPrager3D.cpp material/nD/uw_materials/InitialStateAnalysisWrapper.cpp material/nD/uw_materials/ContactMaterialBase.cc material/nD/uw_materials/ContactMaterial2D.cpp material/nD/uw_materials/ContactMaterial3D.cpp)


SET(nd_adaptor_material material/nD/nd_adaptor/NDAdaptorMaterial.cc material/nD/nd_adaptor/PlaneStressMaterial.cpp material/nD/nd_adaptor/PlateFiberMaterial.cpp material/nD/nd_adaptor/BeamFiberMaterial.cpp)

SET(WEnergy material/nD/FiniteDeformation/WEnergy/WEnergy.cpp material/nD/FiniteDeformation/WEnergy/LogWEnergy.cpp material/nD/FiniteDeformation/WEnergy/MooneyRivlinSimoWEnergy.cpp material/nD/FiniteDeformation/WEnergy/MooneyRivlinWEnergy.cpp material/nD/FiniteDeformation/WEnergy/NeoHookeanWEnergy.cpp material/nD/FiniteDeformation/WEnergy/OgdenSimoWEnergy.cpp material/nD/FiniteDeformation/WEnergy/OgdenWEnergyBase.cc material/nD/FiniteDeformation/WEnergy/OgdenWEnergy.cpp material/nD/FiniteDeformation/WEnergy/SimoPisterWEnergy.cpp)

SET(fdEvolution material/nD/FiniteDeformation/fdEvolution/fdEvolution_S.cpp material/nD/FiniteDeformation/fdEvolution/fdEvolution_SLS.cpp material/nD/FiniteDeformation/fdEvolution/fdEvolution_T.cpp material/nD/FiniteDeformation/fdEvolution/fdEvolution_TL.cpp)

SET(fdFlow material/nD/FiniteDeformation/fdFlow/fdFlow.cpp material/nD/FiniteDeformation/fdFlow/fdFlowDP.cpp material/nD/FiniteDeformation/fdFlow/fdFlowVM.cpp)

SET(fdYield material/nD/FiniteDeformation/fdYield/fdYield.cpp material/nD/FiniteDeformation/fdYield/fdYieldDP.cpp material/nD/FiniteDeformation/fdYield/fdYieldVM.cpp)

SET(finiteDeformation material/nD/FiniteDeformation/FiniteDeformationMaterial.cc material/nD/FiniteDeformation/NeoHookeanCompressible3D.cpp material/nD/FiniteDeformation/FDEPState.cpp material/nD/FiniteDeformation/FDdecoupledElastic3D.cpp material/nD/FiniteDeformation/FiniteDeformationEP3D.cpp material/nD/FiniteDeformation/FiniteDeformationElastic3D.cpp ${fdEvolution} ${fdFlow} ${fdYield} ${WEnergy})

SET(nD_rc_multilayer material/nD/rc_multilayer/PlateAdaptorMaterial.cc material/nD/rc_multilayer/PlateRebarMaterial.cpp material/nD/rc_multilayer/PlasticDamageConcretePlaneStress.cpp material/nD/rc_multilayer/PlateFromPlaneStressMaterial.cpp)

SET(nD_soil material/nD/soil/SoilMaterialBase.cc material/nD/soil/FluidSolidPorousMaterial.cpp material/nD/soil/MultiYieldSurface.cpp material/nD/soil/PressureMultiYieldBase.cc material/nD/soil/PressureDependMultiYieldBase.cc material/nD/soil/PressureDependMultiYield.cpp material/nD/soil/PressureDependMultiYield02.cpp material/nD/soil/PressureIndependMultiYield.cpp material/nD/soil/T2Vector.cpp material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity.cpp material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticity3D.cpp material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityAxiSymm.cpp material/nD/soil/cyclicSoil/MultiaxialCyclicPlasticityPlaneStrain.cpp)

SET(nD_material material/nD/template_3d_ep/CamClayPotentialSurface.cpp material/nD/template_3d_ep/CamClayYieldSurface.cpp material/nD/template_3d_ep/DruckerPragerPotentialSurface.cpp material/nD/template_3d_ep/DruckerPragerYieldSurface.cpp material/nD/template_3d_ep/DruckerPragerYieldSurface01.cpp material/nD/template_3d_ep/EvolutionLaw_LEeq.cpp material/nD/template_3d_ep/EvolutionLaw_LEij.cpp material/nD/template_3d_ep/EvolutionLaw_NLEeq.cpp material/nD/template_3d_ep/EvolutionLaw_NLEij.cpp material/nD/template_3d_ep/EvolutionLaw_NLEijMD.cpp material/nD/template_3d_ep/EvolutionLaw_NLEp.cpp material/nD/template_3d_ep/EvolutionLaw_S.cpp material/nD/template_3d_ep/EvolutionLaw_T.cpp material/nD/template_3d_ep/EPState.cpp material/nD/template_3d_ep/ManzariDafaliasPotentialSurface.cpp material/nD/template_3d_ep/ManzariDafaliasEvolutionLaw.cpp material/nD/template_3d_ep/ManzariDafaliasPotentialSurface01.cpp material/nD/template_3d_ep/ManzariDafaliasYieldSurface.cpp material/nD/template_3d_ep/MatPoint3D.cpp material/nD/template_3d_ep/PotentialSurface.cc material/nD/template_3d_ep/RoundedMohrCoulomb01.cpp material/nD/template_3d_ep/RoundedMohrCoulomb01PotentialSurface.cpp material/nD/template_3d_ep/RoundedMohrCoulomb01YieldSurface.cpp material/nD/template_3d_ep/Template3Dep.cpp material/nD/template_3d_ep/Tri_a_fail_crit_YieldSurface.cpp material/nD/template_3d_ep/VonMisesPotentialSurface.cpp material/nD/template_3d_ep/VonMisesYieldSurface.cpp material/nD/template_3d_ep/YieldSurface.cpp material/nD/ElasticCrossAnisotropic.cpp ${nd_adaptor_material} ${nD_elastic_isotropic} ${nD_j2_plasticity} ${nD_uvmaterial} material/nD/NDMaterial.cpp material/nD/NDMaterialWrapper.cc ${finiteDeformation} ${nD_rc_multilayer} ${nD_soil} material/nD/nd_material_class_names.cc)

SET(uniaxial_steel_material material/uniaxial/steel/SteelBase.cc material/uniaxial/steel/SteelBase0103.cc material/uniaxial/steel/Steel01.cpp material/uniaxial/steel/Steel02.cpp material/uniaxial/steel/Steel03.cpp)

SET(uniaxial_concrete_material material/uniaxial/concrete/RawConcrete.cc material/uniaxial/concrete/ConcreteBase.cpp material/uniaxial/concrete/Concrete01.cpp material/uniaxial/concrete/Concrete02.cpp material/uniaxial/concrete/Concrete04.cpp material/uniaxial/concrete/TDConcreteBase.cc material/uniaxial/concrete/TDConcrete.cpp material/uniaxial/concrete/TDConcreteMC10Base.cc material/uniaxial/concrete/TDConcreteMC10.cpp material/uniaxial/concrete/TDConcreteMC10NL.cpp)

SET(uniaxial_py_material material/uniaxial/soil_structure_interaction/InternalParamsA.cc material/uniaxial/soil_structure_interaction/InternalParamsIn.cc material/uniaxial/soil_structure_interaction/InternalParamsLR.cc material/uniaxial/soil_structure_interaction/InternalParamsLRIn.cc material/uniaxial/soil_structure_interaction/PYBase.cc material/uniaxial/soil_structure_interaction/PQyzBase.cc material/uniaxial/soil_structure_interaction/PyLiq1.cpp material/uniaxial/soil_structure_interaction/PySimple1.cpp material/uniaxial/soil_structure_interaction/generators/Simple1GenBase.cc material/uniaxial/soil_structure_interaction/generators/PySimple1Gen.cpp material/uniaxial/soil_structure_interaction/QzSimple1.cpp material/uniaxial/soil_structure_interaction/TzLiq1.cpp material/uniaxial/soil_structure_interaction/TzSimple1.cpp material/uniaxial/soil_structure_interaction/generators/TzSimple1Gen.cpp material/uniaxial/soil_structure_interaction/EyBasic.cc)

SET(uniaxial_connected_material material/uniaxial/connected/ConnectedMaterial.cc material/uniaxial/connected/SeriesMaterial.cpp material/uniaxial/connected/ParallelMaterial.cpp)

SET(drainmater material/uniaxial/drain/DrainMaterial.cpp material/uniaxial/drain/DrainBilinearMaterial.cpp material/uniaxial/drain/DrainClough1Material.cpp material/uniaxial/drain/DrainClough2Material.cpp material/uniaxial/drain/DrainHardeningMaterial.cpp material/uniaxial/drain/DrainPinch1Material.cpp material/uniaxial/drain/common00.f material/uniaxial/drain/anal00.f)

SET(fedeas material/uniaxial/FedeasMaterial.cpp material/uniaxial/fedeas/FedeasBondMaterial.cc material/uniaxial/fedeas/FedeasBond1Material.cpp material/uniaxial/fedeas/FedeasBond2Material.cpp material/uniaxial/fedeas/FedeasConcrMaterial.cc material/uniaxial/fedeas/FedeasConcr1Material.cpp material/uniaxial/fedeas/FedeasConcr2Material.cpp material/uniaxial/fedeas/FedeasConcr3Material.cpp material/uniaxial/fedeas/FedeasHardeningMaterial.cpp material/uniaxial/fedeas/FedeasHyster1Material.cpp material/uniaxial/fedeas/FedeasHyster2Material.cpp material/uniaxial/fedeas/FedeasSteel1Material.cpp material/uniaxial/fedeas/FedeasSteel2Material.cpp)

SET(uniaxial_material material/uniaxial/DqUniaxialMaterial.cc material/uniaxial/ZeroLengthMaterials.cc material/uniaxial/snap/Bilinear.cpp material/uniaxial/snap/Clough.cpp material/uniaxial/snap/CloughDamage.cpp material/uniaxial/snap/Pinching.cpp material/uniaxial/snap/PinchingDamage.cpp material/uniaxial/BarSlipMaterial.cpp material/uniaxial/BoucWenMaterial.cpp material/uniaxial/CableMaterial.cpp material/uniaxial/ENTMaterial.cpp material/uniaxial/HalfDiagramMaterial.cc material/uniaxial/TensionOnlyMaterial.cpp material/uniaxial/CompressionOnlyMaterial.cpp material/uniaxial/InvertMaterial.cc material/uniaxial/EPPBaseMaterial.cc material/uniaxial/EPPGapMaterial.cpp material/uniaxial/ElasticBaseMaterial.cc material/uniaxial/ElasticMaterial.cpp material/uniaxial/ElasticPPMaterialBase.cc material/uniaxial/ElasticPPMaterial.cpp material/uniaxial/FatigueMaterial.cpp material/uniaxial/HardeningMaterial.cpp material/uniaxial/HystereticMaterial.cpp material/uniaxial/EncapsulatedUniaxialMaterial.cc material/uniaxial/MinMaxMaterial.cpp material/uniaxial/InitStrainBaseMaterial.cpp material/uniaxial/InitStrainMaterial.cpp material/uniaxial/InitStressMaterial.cpp material/uniaxial/MultiLinear.cpp material/uniaxial/NewUniaxialMaterial.cpp material/uniaxial/PathIndependentMaterial.cpp material/uniaxial/Pinching4Material.cpp material/uniaxial/ReinforcingSteel.cpp material/uniaxial/UniaxialHistoryVars.cc material/uniaxial/UniaxialStateVars.cc material/uniaxial/UniaxialMaterial.cpp  material/uniaxial/UniaxialMaterialWrapper.cc material/uniaxial/ViscousMaterial.cpp ${fedeas} ${uniaxial_py_material} ${uniaxial_concrete_material} ${uniaxial_steel_material} ${uniaxial_connected_material} ${drainmater} material/uniaxial/uniaxial_material_class_names.cc)

SET(yield_sfc_material material/yieldSurface/evolution/BkStressLimSurface2D.cpp material/yieldSurface/evolution/BoundingSurface2D.cpp material/yieldSurface/evolution/CombinedIsoKin2D01.cpp material/yieldSurface/evolution/CombinedIsoKin2D02.cpp material/yieldSurface/evolution/Isotropic2D01.cpp material/yieldSurface/evolution/Kinematic2D01.cpp material/yieldSurface/evolution/Kinematic2D02.cpp material/yieldSurface/evolution/NullEvolution.cpp material/yieldSurface/evolution/PeakOriented2D01.cpp material/yieldSurface/evolution/PeakOriented2D02.cpp material/yieldSurface/evolution/PlasticHardening2D.cpp material/yieldSurface/evolution/YS_Evolution.cpp material/yieldSurface/evolution/YS_Evolution2D.cpp material/yieldSurface/plasticHardeningMaterial/ExponReducing.cpp material/yieldSurface/plasticHardeningMaterial/MultiLinearKp.cpp material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.cpp material/yieldSurface/plasticHardeningMaterial/PlasticHardeningMaterial.cpp material/yieldSurface/yieldSurfaceBC/Attalla2D.cpp material/yieldSurface/yieldSurfaceBC/ElTawil2D.cpp material/yieldSurface/yieldSurfaceBC/ElTawil2DUnSym.cpp material/yieldSurface/yieldSurfaceBC/Hajjar2D.cpp material/yieldSurface/yieldSurfaceBC/NullYS2D.cpp material/yieldSurface/yieldSurfaceBC/Orbison2D.cpp material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.cpp material/yieldSurface/yieldSurfaceBC/YieldSurface_BC2D.cpp)

SET(material material/Material.cpp material/MaterialVector.cc material/MaterialWrapper.cc ${uniaxial_material} ${nD_material} ${section_material} ${yield_sfc_material}) 

SET(reliability reliability/FEsensitivity/NewmarkSensitivityIntegrator.cpp reliability/FEsensitivity/SensitivityAlgorithm.cpp reliability/FEsensitivity/SensitivityIntegrator.cpp reliability/FEsensitivity/StaticSensitivityIntegrator.cpp reliability/domain/components/CorrelationCoefficient.cpp reliability/domain/components/LimitStateFunction.cpp reliability/domain/components/Positioner.cc reliability/domain/components/ParameterPositioner.cpp reliability/domain/components/RandomVariable.cpp reliability/domain/components/RandomVariablePositioner.cpp reliability/domain/components/ReliabilityDomain.cpp reliability/domain/components/ReliabilityDomainComponent.cpp reliability/domain/distributions/BetaRV.cpp reliability/domain/distributions/ChiSquareRV.cpp reliability/domain/distributions/ExponentialRV.cpp reliability/domain/distributions/GammaRV.cpp reliability/domain/distributions/GumbelRV.cpp reliability/domain/distributions/LaplaceRV.cpp reliability/domain/distributions/LognormalRV.cpp reliability/domain/distributions/NormalRV.cpp reliability/domain/distributions/ParetoRV.cpp reliability/domain/distributions/RayleighRV.cpp reliability/domain/distributions/ShiftedExponentialRV.cpp reliability/domain/distributions/ShiftedRayleighRV.cpp reliability/domain/distributions/Type1LargestValueRV.cpp reliability/domain/distributions/Type1SmallestValueRV.cpp reliability/domain/distributions/Type2LargestValueRV.cpp reliability/domain/distributions/Type3SmallestValueRV.cpp reliability/domain/distributions/UniformRV.cpp reliability/domain/distributions/UserDefinedRV.cpp reliability/domain/distributions/WeibullRV.cpp reliability/domain/filter/Filter.cpp reliability/domain/filter/KooFilter.cpp reliability/domain/filter/StandardLinearOscillatorAccelerationFilter.cpp reliability/domain/filter/StandardLinearOscillatorDisplacementFilter.cpp reliability/domain/filter/StandardLinearOscillatorVelocityFilter.cpp reliability/domain/modulatingFunction/ConstantModulatingFunction.cpp reliability/domain/modulatingFunction/GammaModulatingFunction.cpp reliability/domain/modulatingFunction/KooModulatingFunction.cpp reliability/domain/modulatingFunction/ModulatingFunction.cpp reliability/domain/modulatingFunction/TrapezoidalModulatingFunction.cpp reliability/domain/spectrum/JonswapSpectrum.cpp reliability/domain/spectrum/NarrowBandSpectrum.cpp reliability/domain/spectrum/PointsSpectrum.cpp reliability/domain/spectrum/Spectrum.cpp reliability/analysis/misc/MatrixOperations.cpp reliability/analysis/analysis/ParametricReliabilityAnalysis.cpp reliability/analysis/analysis/FOSMAnalysis.cpp reliability/analysis/analysis/SamplingAnalysis.cpp reliability/analysis/analysis/GFunVisualizationAnalysis.cpp reliability/analysis/analysis/FragilityAnalysis.cpp reliability/analysis/analysis/SystemAnalysis.cpp reliability/analysis/analysis/MVFOSMAnalysis.cpp reliability/analysis/analysis/FORMAnalysis.cpp reliability/analysis/analysis/ReliabilityAnalysis.cpp reliability/analysis/analysis/SORMAnalysis.cpp reliability/analysis/analysis/OutCrossingAnalysis.cpp reliability/analysis/designPoint/FindDesignPointAlgorithm.cpp reliability/analysis/designPoint/SearchWithStepSizeAndStepDirection.cpp reliability/analysis/rootFinding/RootFinding.cpp reliability/analysis/rootFinding/SecantRootFinding.cpp reliability/analysis/rootFinding/ModNewtonRootFinding.cpp reliability/analysis/stepSize/ArmijoStepSizeRule.cpp reliability/analysis/stepSize/FixedStepSizeRule.cpp reliability/analysis/stepSize/StepSizeRule.cpp reliability/analysis/sensitivity/GradGEvaluator.cpp reliability/analysis/sensitivity/OpenSeesGradGEvaluator.cpp reliability/analysis/sensitivity/FiniteDifferenceGradGEvaluator.cpp reliability/analysis/transformation/ProbabilityTransformation.cpp reliability/analysis/transformation/NatafProbabilityTransformation.cpp reliability/analysis/direction/SearchDirection.cpp reliability/analysis/direction/PolakHeSearchDirectionAndMeritFunction.cpp reliability/analysis/direction/SQPsearchDirectionMeritFunctionAndHessian.cpp reliability/analysis/direction/HLRFSearchDirection.cpp reliability/analysis/direction/GradientProjectionSearchDirection.cpp reliability/analysis/meritFunction/MeritFunctionCheck.cpp reliability/analysis/meritFunction/AdkZhangMeritFunctionCheck.cpp reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck.cpp reliability/analysis/hessianApproximation/HessianApproximation.cpp reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.cpp reliability/analysis/convergenceCheck/OptimalityConditionReliabilityConvergenceCheck.cpp reliability/analysis/convergenceCheck/StandardReliabilityConvergenceCheck.cpp reliability/analysis/gFunction/TclGFunEvaluator.cpp reliability/analysis/gFunction/BasicGFunEvaluator.cpp reliability/analysis/gFunction/GFunEvaluator.cpp reliability/analysis/gFunction/OpenSeesGFunEvaluator.cpp reliability/analysis/randomNumber/RandomNumberGenerator.cpp reliability/analysis/randomNumber/CStdLibRandGenerator.cpp reliability/analysis/curvature/FirstPrincipalCurvature.cpp reliability/analysis/curvature/CurvaturesBySearchAlgorithm.cpp reliability/analysis/curvature/FindCurvatures.cpp)

SET(siseq_linear_distributed solution/system_of_eqn/linearSOE/DistributedLinSOE.cc solution/system_of_eqn/linearSOE/DistributedBandLinSOE.cc solution/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE.cpp solution/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.cpp solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.cpp solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.cpp solution/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE.cpp solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE.cpp solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.cpp solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.cpp solution/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.cpp) 

SET(siseq_linear solution/system_of_eqn/linearSOE/LinearSOEData.cc solution/system_of_eqn/linearSOE/BJsolvers/profmatr.cpp solution/system_of_eqn/linearSOE/BJsolvers/skymatr.cpp solution/system_of_eqn/linearSOE/DomainSolver.cpp solution/system_of_eqn/linearSOE/LinearSOE.cpp solution/system_of_eqn/linearSOE/LinearSOESolver.cpp solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.cpp solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.cpp solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.cpp solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.cpp solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.cpp solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.cpp solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.cpp solution/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.cpp solution/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.cpp solution/system_of_eqn/linearSOE/diagonal/DiagonalSOE.cpp solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.cpp solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.cpp solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.cpp solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver.cpp solution/system_of_eqn/linearSOE/itpack/ItpackLinSOE.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBase.cc solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.cpp solution/system_of_eqn/linearSOE/FactoredSOEBase.cc solution/system_of_eqn/linearSOE/SparseSOEBase.cc solution/system_of_eqn/linearSOE/sparseGEN/SparseGenSOEBase.cc solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.cpp solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.cpp solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.cpp solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.cpp solution/system_of_eqn/linearSOE/sparseGEN/SuperLU.cpp solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSOE.cpp solution/system_of_eqn/linearSOE/sparseSYM/nmat.c solution/system_of_eqn/linearSOE/sparseSYM/symbolic.cc solution/system_of_eqn/linearSOE/sparseSYM/nest.c solution/system_of_eqn/linearSOE/sparseSYM/utility.c solution/system_of_eqn/linearSOE/sparseSYM/grcm.c solution/system_of_eqn/linearSOE/sparseSYM/newordr.c solution/system_of_eqn/linearSOE/sparseSYM/nnsim.c solution/system_of_eqn/linearSOE/sparseSYM/tim.c solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver.cpp solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.cpp solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSolver.cpp solution/system_of_eqn/linearSOE/mumps/MumpsSOE.cpp solution/system_of_eqn/linearSOE/mumps/MumpsParallelSOE.cpp solution/system_of_eqn/linearSOE/mumps/MumpsSolver.cpp solution/system_of_eqn/linearSOE/mumps/MumpsSolverBase.cc solution/system_of_eqn/linearSOE/mumps/MumpsParallelSolver.cpp ${siseq_linear_distributed})

SET(siseq_eigen solution/system_of_eqn/eigenSOE/ArpackSOEBase.cc solution/system_of_eqn/eigenSOE/ArpackSOE.cc solution/system_of_eqn/eigenSOE/ArpackSolver.cpp solution/system_of_eqn/eigenSOE/BandArpackSOE.cpp solution/system_of_eqn/eigenSOE/BandArpackSolver.cpp solution/system_of_eqn/eigenSOE/EigenSOE.cpp solution/system_of_eqn/eigenSOE/EigenSolver.cpp solution/system_of_eqn/eigenSOE/SymArpackSOE.cpp solution/system_of_eqn/eigenSOE/SymArpackSolver.cpp solution/system_of_eqn/eigenSOE/SymBandEigenSOE.cpp solution/system_of_eqn/eigenSOE/SymBandEigenSolver.cpp solution/system_of_eqn/eigenSOE/BandArpackppSOE.cc solution/system_of_eqn/eigenSOE/BandArpackppSolver.cc solution/system_of_eqn/eigenSOE/FullGenEigenSOE.cpp solution/system_of_eqn/eigenSOE/FullGenEigenSolver.cpp)

if(EIGEN3_FOUND)
  if(SPECTRA_FOUND)
    SET(siseq_eigen ${siseq_eigen} solution/system_of_eqn/eigenSOE/SpectraSolver.cc solution/system_of_eqn/eigenSOE/SpectraSOE.cc)
    add_definitions("-DUSE_SPECTRA")
  endif(SPECTRA_FOUND)
endif(EIGEN3_FOUND)


SET(siseq_petsc solution/system_of_eqn/linearSOE/petsc/PetscSolver.cpp solution/system_of_eqn/linearSOE/petsc/PetscSOE.cpp solution/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.cpp solution/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.cpp solution/system_of_eqn/linearSOE/petsc/ActorPetscSOE.cpp)

SET(siseq solution/system_of_eqn/Solver.cpp solution/system_of_eqn/SystemOfEqn.cpp ${siseq_linear} ${siseq_eigen} ${siseq_petsc})

SET(siseq_no solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinThreadSolver.cpp solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.cpp solution/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.cpp solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver.cpp) 

SET(unittest unittest/unittest)

SET(preprocessor_mbt_trf preprocessor/multi_block_topology/trf/TrfGeom.cc preprocessor/multi_block_topology/trf/Rotation.cc preprocessor/multi_block_topology/trf/Scaling.cc preprocessor/multi_block_topology/trf/Reflection.cc preprocessor/multi_block_topology/trf/Translation.cc)

SET(preprocessor_mbt_0d_entities preprocessor/multi_block_topology/entities/0d/Pnt.cc)

SET(preprocessor_mbt_1d_entities preprocessor/multi_block_topology/entities/1d/Edge.cc preprocessor/multi_block_topology/entities/1d/LineBase.cc preprocessor/multi_block_topology/entities/1d/Line.cc preprocessor/multi_block_topology/entities/1d/DividedLine.cc preprocessor/multi_block_topology/entities/1d/CircularArc.cc preprocessor/multi_block_topology/entities/1d/CmbEdge.cc)

SET(preprocessor_mbt_2d_entities preprocessor/multi_block_topology/entities/2d/Face.cc preprocessor/multi_block_topology/entities/2d/QuadSurface.cc preprocessor/multi_block_topology/entities/2d/PolygonalFace.cc)

SET(preprocessor_mbt_3d_entities preprocessor/multi_block_topology/entities/3d/Body.cc preprocessor/multi_block_topology/entities/3d/Block.cc preprocessor/multi_block_topology/entities/3d/UniformGrid.cc)

SET(preprocessor_mbt_entities preprocessor/multi_block_topology/entities/EntMdlr.cc ${preprocessor_mbt_0d_entities} ${preprocessor_mbt_1d_entities} ${preprocessor_mbt_2d_entities} ${preprocessor_mbt_3d_entities})

SET(preprocessor_mbt_entities_containers preprocessor/multi_block_topology/entities/containers/PntMap.cc preprocessor/multi_block_topology/entities/containers/LineMap.cc preprocessor/multi_block_topology/entities/containers/SurfaceMap.cc preprocessor/multi_block_topology/entities/containers/BodyMap.cc preprocessor/multi_block_topology/entities/containers/UniformGridMap.cc)

SET(preprocessor_mbt_matrices preprocessor/multi_block_topology/matrices/PntPtrArray.cc preprocessor/multi_block_topology/matrices/Framework2d.cc preprocessor/multi_block_topology/matrices/PntPtrArray3d.cc preprocessor/multi_block_topology/matrices/Framework3d.cc preprocessor/multi_block_topology/matrices/NodePtrArray.cc preprocessor/multi_block_topology/matrices/ElemPtrArray.cc preprocessor/multi_block_topology/matrices/NodePtrArray3d.cc preprocessor/multi_block_topology/matrices/ElemPtrArray3d.cc)

SET(preprocessor_mbt preprocessor/multi_block_topology/ModelComponentContainerBase.cc preprocessor/multi_block_topology/ReferenceFrame.cc preprocessor/multi_block_topology/ReferenceFrameMap.cc preprocessor/multi_block_topology/CartesianReferenceFrame3d.cc preprocessor/multi_block_topology/MultiBlockTopology.cc preprocessor/multi_block_topology/aux_meshing.cc ${preprocessor_mbt_trf} ${preprocessor_mbt_entities} ${preprocessor_mbt_entities_containers} ${preprocessor_mbt_matrices} )

SET(preprocessor_prep_handlers preprocessor/PreprocessorContainer.cc preprocessor/prep_handlers/PrepHandler.cc preprocessor/prep_handlers/NodeHandler.cc preprocessor/prep_handlers/ElementHandler.cc preprocessor/prep_handlers/ProtoElementHandler.cc preprocessor/prep_handlers/MaterialHandler.cc preprocessor/prep_handlers/BeamIntegratorHandler.cc preprocessor/prep_handlers/TransfCooHandler.cc preprocessor/prep_handlers/LoadHandlerMember.cc preprocessor/prep_handlers/LoadHandler.cc preprocessor/prep_handlers/BoundaryCondHandler.cc)

SET(preprocessor_set_mgmt preprocessor/set_mgmt/DqPtrsKDTree.cc preprocessor/set_mgmt/DqPtrsNode.cc preprocessor/set_mgmt/DqPtrsElem.cc preprocessor/set_mgmt/DqPtrsConstraint.cc preprocessor/set_mgmt/SetMeshComp.cc preprocessor/set_mgmt/SetBase.cc preprocessor/set_mgmt/SetEstruct.cc preprocessor/set_mgmt/DqPtrsFaces.cc preprocessor/set_mgmt/SetEntities.cc preprocessor/set_mgmt/Set.cc preprocessor/set_mgmt/IRowSet.cc preprocessor/set_mgmt/JRowSet.cc preprocessor/set_mgmt/KRowSet.cc preprocessor/set_mgmt/MapSetBase.cc preprocessor/set_mgmt/MapSet.cc)

SET(preprocessor preprocessor/EntMdlrBase.cc preprocessor/MeshingParams.cc ${preprocessor_mbt} ${preprocessor_set_mgmt} ${preprocessor_prep_handlers} preprocessor/Preprocessor.cc)

SET(solution solution/analysis/ModelWrapper.cc solution/SolutionStrategy.cc solution/SolutionStrategyMap.cc solution/analysis/MapModelWrapper.cc solution/SolutionProcedureControl.cc solution/SolutionProcedure.cc)

# Build our libraries
add_library(xc_basic_utils ${text_utils} ${stream_utils} ${misc_utils} ${sqlitepp_utils} ${matrices} ${three_d_arrays} ${skypack} ${itpack} ${functions})
install(TARGETS xc_basic_utils DESTINATION lib)

add_library(xc_utils SHARED ${SQLITE} ${cuadGauss} ${geom} ${GnuGts} ${KERNEL} ${VtkAux} ${ca_load_combinations})


add_library(XcBib SHARED ${utility} ${material} ${siseq} ${analysis} ${convergenceTest} ${coordTransformation} ${damage} ${domain} ${gauss_models} ${cyclic_model} ${element} ${graph} ${modelbuilder} ${reliability} ${unitest} ${preprocessor} ${solution} version.cc FEProblem.cc)

#Python interface
link_libraries(xc_utils xc_basic_utils OpenMP::OpenMP_CXX ${VTK_BIB} ${Plot_LIBRARY} ${MPFR_LIBRARIES} ${GMP_LIBRARY} ${SQLITE3_LIBRARY} ${GNUGTS_LIBRARIES} ${XC_UTILS_BOOST_LIBRARIES} ${PYTHON_LIBRARIES} )
target_link_libraries(xc_utils CGAL::CGAL CGAL::CGAL_Core) # CGAL stuff
add_definitions(-fno-strict-aliasing)
add_library(xc_base SHARED utility/kernel/python_interface.cc)
#ADD_LIBRARY(vtk_aux SHARED vtk_aux/python_interface.cc)

INSTALL(TARGETS xc_basic_utils xc_utils DESTINATION lib)

target_link_libraries(XcBib xc_utils xc_basic_utils OpenMP::OpenMP_CXX ${VTK_BIB} ${VTK_LIBRARIES} CGAL::CGAL CGAL::CGAL_Core ${Plot_LIBRARY} ${MPFR_LIBRARIES} ${GMP_LIBRARY} ${MYSQL_LIBRARY} ${MySQLpp_LIBRARIES} ${SQLITE3_LIBRARY} ${GNUGTS_LIBRARIES} ${BerkeleyDB_LIBRARIES} ${ARPACK_LIB} ${ARPACKPP_LIB} ${UMFPACK_LIB} ${DMUMPS_LIB} ${LAPACK_LIBRARIES} ${SUPERLU_LIBRARIES} ${BLAS_LIBRARIES} ${PETSC_LIB_PETSC} ${METIS_LIBRARIES} ${TCL_LIBRARY} ${XC_UTILS_BOOST_LIBRARIES} ${PYTHON_LIBRARIES} ${F2C_LIBRARY} ${GMSH_LIBRARIES} ${SUITESPARSE_LIBRARIES} ${MPI_CXX_LIBRARIES})
add_definitions(-fno-strict-aliasing)

## Define the wrapper libraries
add_library(geom SHARED utility/geom/d0/export_d0_entities.cc utility/geom/d1/export_d1_entities.cc utility/geom/d2/export_d2_entities.cc utility/geom/d3/export_d3_entities.cc utility/geom/export_sys_trf.cc utility/geom/python_interface.cc)

add_library(xcGnuGts SHARED utility/gnu_gts/python_interface.cc)

add_library(loadCombinations SHARED utility/load_combinations/python_interface.cc)

add_library(xc SHARED utility/export_utility.cc material/export_material_base.cc material/uniaxial/export_material_uniaxial.cc material/nD/export_material_nD.cc material/section/export_material_section.cc material/section/export_material_fiber_section.cc material/damage/export_material_damage.cc domain/export_domain.cc domain/mesh/export_domain_mesh.cc preprocessor/export_preprocessor_handlers.cc preprocessor/export_preprocessor_build_model.cc preprocessor/export_preprocessor_sets.cc preprocessor/export_preprocessor_main.cc solution/export_solution.cc python_interface.cc)
target_link_libraries(xc ${Boost_LIBRARIES} XcBib)
# don't prepend wrapper library name with lib
set_target_properties(xc PROPERTIES PREFIX "" )


INSTALL(TARGETS geom loadCombinations XcBib DESTINATION lib)
#INSTALL(DIRECTORY ${DIR_FUENTES_XC}/macros/ DESTINATION lib/macros_xc)

