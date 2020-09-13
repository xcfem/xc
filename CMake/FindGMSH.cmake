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

# ------

MESSAGE(STATUS "Check for GMSH ...")

# ------

SET(GMSH_ROOT_DIR $ENV{GMSH_ROOT_DIR} CACHE PATH "Path to the GMSH.")

IF(GMSH_ROOT_DIR)
 LIST(APPEND CMAKE_PREFIX_PATH "${GMSH_ROOT_DIR}")
ENDIF(GMSH_ROOT_DIR)

FIND_PATH(GMSH_INCLUDE_DIRS Gmsh.h gmsh.h PATH_SUFFIXES gmsh)
FIND_LIBRARY(GMSH_LIBRARIES NAMES Gmsh gmsh)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMSH REQUIRED_VARS GMSH_INCLUDE_DIRS GMSH_LIBRARIES)
