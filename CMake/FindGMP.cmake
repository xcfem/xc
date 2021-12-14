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
FIND_PATH(GMP_INCLUDE_DIR gmp.h $ENV{HOME}/.local/include /usr/local/include)

FIND_LIBRARY(GMP_LIBRARY NAMES gmp PATH $ENV{HOME}/.local/include /usr/local/lib)

IF (GMP_INCLUDE_DIR AND GMP_LIBRARY)
   SET(GMP_FOUND TRUE)
ENDIF (GMP_INCLUDE_DIR AND GMP_LIBRARY)

IF (GMP_FOUND)
   IF (NOT GMP_FIND_QUIETLY)
      MESSAGE(STATUS "Found GMP: ${GMP_LIBRARY}")
   ENDIF (NOT GMP_FIND_QUIETLY)
ELSE (GMP_FOUND)
   IF (GMP_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GMP")
   ENDIF (GMP_FIND_REQUIRED)
ENDIF (GMP_FOUND)
