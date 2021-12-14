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
find_path(Plot_INCLUDE_DIR plotter.h
  /usr/include
  /usr/local/include
  DOC "Specify the directory containing plotter.h."
)

FIND_LIBRARY(Plot_LIBRARY
  NAMES plotter
  PATHS
  /usr/lib
  /usr/local/lib
  DOC "Specify the plotter library here."
)

IF(Plot_INCLUDE_DIR)
   SET(Plot_FOUND TRUE)
ENDIF (Plot_INCLUDE_DIR)

IF(Plot_FOUND)
  MESSAGE(STATUS "Found plotter.h: ${Plot_INCLUDE_DIR}")
  MESSAGE(STATUS "Found plotter lib: ${Plot_LIBRARY}")
ELSE(Plot_FOUND)
  MESSAGE(FATAL_ERROR "Could not find libplot (plotter.h)")
ENDIF(Plot_FOUND)
