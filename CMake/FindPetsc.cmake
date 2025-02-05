# ----------------------------------------------------------------------------
#   XC program; finite element analysis code
#   for structural analysis and design.
# 
#   Copyright (C)  Luis C. Pérez Tato
# 
#   Except for the restrictions that may arise from the copyright
#   of the original program (see copyright below if any)
#   XC is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or 
#   (at your option) any later version.
# 
#   This software is distributed in the hope that it will be useful, but 
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details. 
# 
# 
#  You should have received a copy of the GNU General Public License 
#  along with this program.
#  If not, see <http:// www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------
SET( PETSC_FOUND 0 )

# Check environment variables.
if(NOT PETSC_DIR)
    if(DEFINED ENV{PETSC_DIR})
	set(DEFAULT_PETSC_DIR "$ENV{PETSC_DIR}")
    else(DEFINED ENV{PETSC_DIR})
	set(DEFAULT_PETSC_DIR "")
    endif(DEFINED ENV{PETSC_DIR})
endif(NOT PETSC_DIR)
# ----------------------------------------------------------------------------
#   XC program; finite element analysis code
#   for structural analysis and design.
# 
#   Copyright (C)  Luis C. Pérez Tato
# 
#   Except for the restrictions that may arise from the copyright
#   of the original program (see copyright below if any)
#   XC is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or 
#   (at your option) any later version.
# 
#   This software is distributed in the hope that it will be useful, but 
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details. 
# 
# 
#  You should have received a copy of the GNU General Public License 
#  along with this program.
#  If not, see <http:// www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------

## The following line doesn't work I don't know why (LCPT 07/12/2021)
#set(PETSC_DIR "${DEFAULT_PETSC_DIR}" CACHE PATH "Installation directory of PETSC library")
set(PETSC_DIR "${DEFAULT_PETSC_DIR}")

if(NOT PETSC_ARCH)
    if(DEFINED ENV{PETSC_ARCH})
	set(DEFAULT_PETSC_ARCH "$ENV{PETSC_ARCH}")
    else(DEFINED ENV{PETSC_ARCH})
	set(DEFAULT_PETSC_ARCH "")
    endif(DEFINED ENV{PETSC_ARCH})
endif(NOT PETSC_ARCH)
## The following line doesn't work I don't know why (LCPT 07/12/2021)
#set(PETSC_ARCH "${DEFAULT_PETSC_ARCH}" CACHE STRING "Build architecture")
set(PETSC_ARCH "${DEFAULT_PETSC_ARCH}")

# MESSAGE( STATUS "DEFAULT_PETSC_ARCH: ${DEFAULT_PETSC_ARCH}")
# MESSAGE( STATUS "PETSC_ARCH: ${PETSC_ARCH}")
    
FIND_PATH(
  PETSC_INCLUDE_DIR
  NAMES petsc.h
  PATHS ${PETSC_DIR}/include
  PATHS /usr/include/petsc/
  PATHS /usr/include/petscdir/
  )
FIND_PATH(
  PETSCCONF_INCLUDE_DIR
  NAMES petscconf.h
  PATHS ${PETSC_DIR}/${PETSC_ARCH}/include
  PATHS /usr/include/petsc/ 
  PATHS /usr/include/petscdir/
  )
# MESSAGE( STATUS "PETSC_INCLUDE_DIR: ${PETSC_INCLUDE_DIR}")
# MESSAGE( STATUS "PETSCCONF_INCLUDE_DIR: ${PETSCCONF_INCLUDE_DIR}")

IF( PETSCCONF_INCLUDE_DIR STREQUAL PETSC_INCLUDE_DIR )
  SET(CMAKE_REQUIRED_INCLUDES "${PETSC_INCLUDE_DIR} ${CMAKE_REQUIRED_INCLUDES}")
ELSE( PETSCCONF_INCLUDE_DIR STREQUAL PETSC_INCLUDE_DIR )
  SET(CMAKE_REQUIRED_INCLUDES "${PETSC_INCLUDE_DIR} ${PETSCCONF_INCLUDE_DIR} ${CMAKE_REQUIRED_INCLUDES}")
ENDIF( PETSCCONF_INCLUDE_DIR STREQUAL PETSC_INCLUDE_DIR )

#MESSAGE( STATUS "CMAKE_REQUIRED_INCLUDES: ${CMAKE_REQUIRED_INCLUDES}")

IF (PETSC_INCLUDE_DIR)
  IF (PETSCCONF_INCLUDE_DIR)
    SET( PETSC_FOUND 1 )
    MESSAGE( STATUS "Found PETSC include dir: ${PETSC_INCLUDE_DIR}")
    MESSAGE( STATUS "Found PETSCCONF include dir: ${PETSCCONF_INCLUDE_DIR}")
  ELSE (PETSCCONF_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "petscconf.h not found.")
  ENDIF (PETSCCONF_INCLUDE_DIR)
ELSE (PETSC_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "petsc.h not found.")
ENDIF (PETSC_INCLUDE_DIR)

IF ( PETSC_FOUND )
  ADD_DEFINITIONS( -DHAVE_PETSC_H -DHAVE_PETSC )
ENDIF( PETSC_FOUND )


#FIND_LIBRARY(PETSC_LIB_MPIUNI    mpiuni    PATHS /usr/local/petsc-2.3.3-p8/lib/linux-gnu-c-debug QUIET )
FIND_LIBRARY(
  PETSC_LIB_PETSC
  NAMES
    petsc
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
FIND_LIBRARY(
  PETSC_LIB_PETSCDM
  NAMES
    petscdm
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
FIND_LIBRARY(
  PETSC_LIB_PETSCVEC
  NAMES
    petscvec
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
FIND_LIBRARY(
  PETSC_LIB_PETSCMAT
  NAMES
    petscmat
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
FIND_LIBRARY(
  PETSC_LIB_PETSCKSP
  NAMES
    petscksp
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
FIND_LIBRARY(
  PETSC_LIB_PETSCSNES
  NAMES
    petscsnes
  PATHS
    /usr/lib
    /usr/local/lib
    ${PETSC_DIR}/${PETSC_ARCH}/lib
  )
IF (PETSC_LIB_PETSC)
  MESSAGE( STATUS "Found PETSC library: ${PETSC_LIB_PETSC}")
ELSE (PETSC_LIB_PETSC)
  MESSAGE(FATAL_ERROR "PETSC library not found.")
ENDIF(PETSC_LIB_PETSC)

SET(PETSC_LIBRARIES
  ${PETSC_LIB_PETSCSNES}
  ${PETSC_LIB_PETSCKSP}
  ${PETSC_LIB_PETSCMAT}
  ${PETSC_LIB_PETSCVEC}
  ${PETSC_LIB_PETSCDM}
  ${PETSC_LIB_PETSC}
#  ${PETSC_LIB_MPIUNI}
)
