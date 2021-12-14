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

# F2C
# Once done, this will define
#
#  F2C_FOUND - system has libf2c
#  F2C_INCLUDE_DIR - the libf2c include directories
#  F2C_LIBRARY - link these to use libf2c


# Include dir
find_path(F2C_INCLUDE_DIR
  NAMES f2c.h
)


# Finally the library itself
find_library(F2C_LIBRARY
  NAMES f2c
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(F2C DEFAULT_MSG F2C_LIBRARY F2C_INCLUDE_DIR)

