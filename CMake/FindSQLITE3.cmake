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

# - Try to find SQLITE3
# Once done this will define
#
#  SQLITE3_FOUND - system has SQLITE3
#  SQLITE3_INCLUDE_DIR - the SQLITE3 include directory
#  SQLITE3_LIBRARIES - Link these to use SQLITE3


find_path(SQLITE3_INCLUDE_DIR
    NAMES
      sqlite3.h
    PATHS
      ${_SQLITE3_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

find_library(SQLITE3_LIBRARY
    NAMES
      sqlite3
    PATHS
      ${_SQLITE3_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
  set(SQLITE3_FOUND TRUE)
  MESSAGE(STATUS "SQLite3 library found.")
else(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
  MESSAGE(STATUS "SQLite3 library NOT found.")  
endif(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)




