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
SET( METIS_FOUND 0 )

FIND_PATH(
  METIS_INCLUDE_DIR
  NAMES metis.h
  PATHS /usr/include/metis/
  PATHS /usr/local/include/metis/
  )

#MESSAGE( STATUS "METIS_INCLUDE_DIR: ${METIS_INCLUDE_DIR}")

IF (METIS_INCLUDE_DIR)
  SET( METIS_FOUND 1 )
  MESSAGE( STATUS "Found METIS: ${METIS_INCLUDE_DIR}")
ELSE (METIS_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "metis.h not found.")
ENDIF (METIS_INCLUDE_DIR)

FIND_LIBRARY(
  METIS_LIB
  metis 
  PATHS /usr/lib
  PATHS /usr/local/lib
  )

SET(METIS_LIBRARIES ${METIS_LIB} )
