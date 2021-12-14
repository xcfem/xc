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
MESSAGE(STATUS "Looking for execinfo")
FIND_PATH(EXECINFO_INCLUDE_PATH
	        NAMES
	                execinfo.h
	        PATHS
	                /usr/include
	                /usr/local/include
	                /sw/include
	                /opt/local/include
	                NO_DEFAULT_PATH)
	
IF(EXECINFO_INCLUDE_PATH)
  add_definitions(-DHAVE_EXECINFO_H=1)
  SET(EXECINFO_FOUND 1 CACHE STRING "Set to 1 if execinfo is found, 0 otherwise")
  MESSAGE(STATUS "Looking for execinfo headers - found")
ENDIF(EXECINFO_INCLUDE_PATH)
MARK_AS_ADVANCED(EXECINFO_FOUND)
