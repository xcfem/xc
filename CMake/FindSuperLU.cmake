find_package(BLAS QUIET REQUIRED)
if(NOT BLAS_FOUND AND REQUIRED)
  message("BLAS not found but required for SuperLU")
  return()
endif(NOT BLAS_FOUND AND REQUIRED)

# look for header files
find_path(SUPERLU_INCLUDE_DIR
  NAMES supermatrix.h
  PATHS /usr/include/
  PATHS /usr/include/superlu/
  PATHS /usr/local/include/
  PATHS /usr/local/include/superlu/
)

# look for library
find_library(SUPERLU_LIBRARY
  NAMES "superlu_4.3" "superlu_4.2" "superlu_4.1" "superlu_4.0" "superlu_3.1" "superlu_3.0" "superlu"
  PATHS /usr/lib/
  PATHS /usr/local/lib/
)

IF(SUPERLU_INCLUDE_DIR)
  IF(SUPERLU_LIBRARY)
    SET(SUPERLU_FOUND 1)
    MESSAGE( STATUS "Found SuperLU: ${SUPERLU_INCLUDE_DIR}")
  ENDIF(SUPERLU_LIBRARY)
ELSE(SUPERLU_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "supermatrix.h not found.")
ENDIF(SUPERLU_INCLUDE_DIR)

# if both headers and library are found, store results
if(SUPERLU_FOUND)
  set(SUPERLU_INCLUDE_DIRS ${SUPERLU_INCLUDE_DIR})
  set(SUPERLU_LIBRARIES    ${SUPERLU_LIBRARY})
  # log result
  file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    "Determing location of ${SUPERLU_WITH_VERSION} succeded:\n"
    "Include directory: ${SUPERLU_INCLUDE_DIR}\n"
    "Library directory: ${SUPERLU_LIBRARY}\n\n")
  set(SUPERLU_CPPFLAGS "-I${SUPERLU_INCLUDE_DIRS} -DENABLE_SUPERLU=1")
  set(SUPERLU_LIBS "-L. ${SUPERLU_LIBRARIES} ${BLAS_LIBRARIES}")
else(SUPERLU_FOUND)
  # log errornous result
  file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
    "Determing location of SuperLU failed:\n"
    "Include directory: ${SUPERLU_INCLUDE_DIR}\n"
    "Library directory: ${SUPERLU_LIBRARY}\n\n")
endif(SUPERLU_FOUND)
