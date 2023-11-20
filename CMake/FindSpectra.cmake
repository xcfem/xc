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

# SPECTRA
# Once done, this will define
#
#  SPECTRA_FOUND - system has Spectra
#  SPECTRA_INCLUDE_DIR - the Spectra include directories

SET(SPECTRA_SEARCH_PATHS ${CMAKE_SOURCE_DIR} /usr/include/Spectra /usr/local/include/Spectra)

# Include dir
find_path(SPECTRA_INCLUDE_DIR NAMES DavidsonSymEigsSolver.h PATHS ${SPECTRA_SEARCH_PATHS})
IF(NOT SPECTRA_INCLUDE_DIR)
  MESSAGE(STATUS "Spectra not found (DavidsonSymEigsSolver.h)")
ENDIF()

SET(SPECTRA_FOUND FALSE)
IF (SPECTRA_INCLUDE_DIR)
  SET(SPECTRA_FOUND TRUE)
    MARK_AS_ADVANCED(SPECTRA_INCLUDE_DIR)
ENDIF (SPECTRA_INCLUDE_DIR)

IF (SPECTRA_FOUND)
  MESSAGE(STATUS "Found Spectra: ${SPECTRA_INCLUDE_DIR}")
ELSE(SPECTRA_FOUND)
  IF (SPECTRA_INCLUDE_DIR_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Spectra")
  ENDIF (SPECTRA_INCLUDE_DIR_REQUIRED)
ENDIF (SPECTRA_FOUND)



