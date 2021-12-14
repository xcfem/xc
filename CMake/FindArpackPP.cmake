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

# ARPACKPP
# Once done, this will define
#
#  ARPACKPP_FOUND - system has libf2c
#  ARPACKPP_INCLUDE_DIR - the libf2c include directories
#  ARPACKPP_LIBRARY - link these to use libf2c

SET(ARPACKPP_SEARCH_PATHS ${CMAKE_SOURCE_DIR} /usr/include/arpack++ /usr/local/include/arpack++)

# Include dir
find_path(ARPACKPP_INCLUDE_DIR NAMES arbnsmat.h PATHS ${ARPACKPP_SEARCH_PATHS})
IF(NOT ARPACKPP_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "arpack++ not found (arbnsmat.h)")
ENDIF()

FIND_LIBRARY(ARPACKPP_LIB NAMES arpack++ PATHS /usr/lib /usr/local/lib)

SET(ARPACKPP_FOUND FALSE)
IF (ARPACKPP_INCLUDE_DIR)
  SET(ARPACKPP_FOUND TRUE)
    MARK_AS_ADVANCED(ARPACKPP_INCLUDE_DIR)
ENDIF (ARPACKPP_INCLUDE_DIR)

IF (ARPACKPP_FOUND)
  MESSAGE(STATUS "Found Arpack++: ${ARPACKPP_INCLUDE_DIR}")
  MESSAGE(STATUS "Found Arpack++ lib: ${ARPACKPP_LIB}")
ELSE(ARPACKPP_FOUND)
  IF (ARPACKPP_INCLUDE_DIR_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Arpack++")
  ENDIF (ARPACKPP_INCLUDE_DIR_REQUIRED)
ENDIF (ARPACKPP_FOUND)



