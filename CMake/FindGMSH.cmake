# - Find GMSH
# Sets the following variables:
#   GMSH_INCLUDE_DIRS - path to the GMSH include directory
#   GMSH_LIBRARIES    - path to the GMSH libraries to be linked against
#

#########################################################################
# Copyright (C) 2012-2015  ALNEOS
# Copyright (C) 2016-2019  EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(GMSH_ROOT_DIR $ENV{GMSH_ROOT_DIR} CACHE PATH "Path to the GMSH.")

IF(GMSH_ROOT_DIR)
  LIST(APPEND CMAKE_PREFIX_PATH "${GMSH_ROOT_DIR}")
ENDIF(GMSH_ROOT_DIR)

FIND_PATH(GMSH_INCLUDE_DIRS Gmsh.h gmsh.h PATH_SUFFIXES gmsh)
FIND_LIBRARY(GMSH_LIBRARIES NAMES Gmsh gmsh)

if (GMSH_INCLUDE_DIRS)
  if (EXISTS "${GMSH_INCLUDE_DIRS}/gmsh.h")
    file(STRINGS "${GMSH_INCLUDE_DIRS}/gmsh.h" _gmsh_version
      REGEX "GMSH_API_VERSION")
    string(REGEX REPLACE ".*GMSH_API_VERSION_MAJOR *\([0-9]*\).*" "\\1" _gmsh_major "${_gmsh_version}")
    string(REGEX REPLACE ".*GMSH_API_VERSION_MINOR *\([0-9]*\).*" "\\1" _gmsh_minor "${_gmsh_version}")
    string(REGEX REPLACE ".*GMSH_API_VERSION_PATCH *\([0-9]*\).*" "\\1" _gmsh_patch "${_gmsh_version}")
    unset(_gmsh_version)
    if (NOT _gmsh_major STREQUAL "" AND
        NOT _gmsh_minor STREQUAL "" AND
        NOT _gmsh_patch STREQUAL "")
      set(GMSH_VERSION "${_gmsh_major}.${_gmsh_minor}.${_gmsh_patch}")
    endif ()
    unset(_gmsh_major)
    unset(_gmsh_minor)
    unset(_gmsh_patch)
  else ()
    set(GMSH_VERSION GMSH_VERSION-NOTFOUND)
  endif ()
endif ()

IF (GMSH_INCLUDE_DIRS AND GMSH_LIBRARIES)
   SET(GMSH_FOUND TRUE)
ENDIF (GMSH_INCLUDE_DIRS AND GMSH_LIBRARIES)

IF (GMSH_FOUND)
  MESSAGE(STATUS "Found GMSH: ${GMSH_LIBRARY} version: ${GMSH_VERSION}")
ELSE (GMSH_FOUND)
  MESSAGE(FATAL_ERROR "Could not find GMSH")
ENDIF (GMSH_FOUND)


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMSH REQUIRED_VARS GMSH_INCLUDE_DIRS GMSH_LIBRARIES)
