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

SET(ARPACK_SEARCH_PATHS ${CMAKE_SOURCE_DIR} /opt/local/lib/  ../ThirdParty/ARPACK)

FIND_LIBRARY(ARPACK_LIB NAMES arpack.1 arpack PATHS ${ARPACK_SEARCH_PATHS})

SET(ARPACK_FOUND FALSE)
IF (ARPACK_LIB)
  SET(ARPACK_FOUND TRUE)
    MARK_AS_ADVANCED(ARPACK_LIB)
ENDIF (ARPACK_LIB)

IF (ARPACK_FOUND)
  IF (NOT ARPACK_LIB_FIND_QUIETLY)
     MESSAGE(STATUS "Found Arpack: ${ARPACK_LIB}")
     GET_FILENAME_COMPONENT(ARPACK_PATH ${ARPACK_LIB} PATH CACHE)
     SET(ARPACK_INCLUDE_DIR ${ARPACK_PATH}/../include CACHE FILEPATH "ARPACK include directory.")
  ENDIF (NOT ARPACK_LIB_FIND_QUIETLY)
ELSE(ARPACK_FOUND)
  IF (ARPACK_LIB_FIND_REQUIRED)
     MESSAGE(FATAL_ERROR "Could not find Arpack")
  ENDIF (ARPACK_LIB_FIND_REQUIRED)
ENDIF (ARPACK_FOUND)



