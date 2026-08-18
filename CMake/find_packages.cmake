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
MESSAGE(STATUS "************* find packages begins ****************")
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(VTK REQUIRED)
find_package(GTK2 REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Qt6 Core)
find_package(GNUGTS REQUIRED)
find_package(CImg REQUIRED)
find_package(MySQL REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Python version ${Python3_VERSION} found")
    message(STATUS "Python executable: ${Python3_EXECUTABLE}")
    message(STATUS "Python standard install directory: ${Python3_STDLIB}")
    message(STATUS "Python include directories: ${Python3_INCLUDE_DIRS}")
    message(STATUS "Python library directories: ${Python3_LIBRARY_DIRS}")
    message(STATUS "Python libraries: ${Python3_LIBRARIES}")
endif()
find_package(F2C REQUIRED)
find_package(ExecInfo)
find_package(Plot REQUIRED)
find_package(Gnuplot REQUIRED)
find_package(MPFR)
find_package(GMP)
find_package(SQLITE3 REQUIRED)
find_package(MPI REQUIRED)
find_package(Arpack REQUIRED)
find_package(ArpackPP REQUIRED)
find_package(Umfpack REQUIRED)
find_package(MUMPS REQUIRED)
find_package(Petsc)
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(SuperLU REQUIRED)
find_package(Spectra)
find_package(BerkeleyDB REQUIRED)
find_package(METIS REQUIRED)
find_package(TCL REQUIRED)
find_package(ORACLE)
find_package(OpenMP REQUIRED)
find_package(GMSH 4.8 REQUIRED)
find_package(SuiteSparse REQUIRED)
MESSAGE(STATUS "************* find packages ends ****************")
