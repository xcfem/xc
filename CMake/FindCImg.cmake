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

FIND_PATH(CImg_INCLUDE_DIR CImg.h $ENV{HOME}/.local/include)

IF (CImg_INCLUDE_DIR)
   SET(CImg_FOUND TRUE)
ENDIF (CImg_INCLUDE_DIR)

IF (CImg_FOUND)
   IF (NOT CImg_FIND_QUIETLY)
      MESSAGE(STATUS "Found CImg: ${CImg_LIBRARY}")
   ENDIF (NOT CImg_FIND_QUIETLY)
ELSE (CImg_FOUND)
   IF (CImg_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find CImg")
   ENDIF (CImg_FIND_REQUIRED)
ENDIF (CImg_FOUND)
