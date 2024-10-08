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
# TOra: Configure Oracle libraries
#
# ORACLE_FOUND - system has Oracle OCI
# ORACLE_INCLUDES - where to find oci.h
# ORACLE_LIBRARIES - the libraries to link against to use Oracle OCI
#
# copyright (c) 2007 Petr Vanek <petr@scribus.info>
# Redistribution and use is allowed according to the terms of the GPLv2 license.
#

SET(ORACLE_FOUND 0)


set(ORACLE_HOME $ENV{ORACLE_HOME})


IF (ORACLE_PATH_INCLUDES)
    SET (ORACLE_INCLUDES_LOCATION ${ORACLE_PATH_INCLUDES})
ELSE (ORACLE_PATH_INCLUDES)
    SET (ORACLE_INCLUDES_LOCATION
            ${ORACLE_HOME}/rdbms/public
            ${ORACLE_HOME}/include
            # sdk
            ${ORACLE_HOME}/sdk/include
            # xe on windows
            ${ORACLE_HOME}/OCI/include
       )
ENDIF (ORACLE_PATH_INCLUDES)

IF (ORACLE_PATH_LIB)
    SET (ORACLE_LIB_LOCATION ${ORACLE_PATH_LIB})
ELSE (ORACLE_PATH_LIB)
    SET (ORACLE_LIB_LOCATION
            ${ORACLE_HOME}/lib
            # xe on windows
            ${ORACLE_HOME}/OCI/lib/MSVC
        )
ENDIF (ORACLE_PATH_LIB)

FIND_PATH(
    ORACLE_INCLUDES
    oci.h
    ${ORACLE_INCLUDES_LOCATION}
)

FIND_LIBRARY(
    ORACLE_LIBRARY_OCCI
    NAMES libocci occi oraocci10
    PATHS ${ORACLE_LIB_LOCATION}
)
FIND_LIBRARY(
    ORACLE_LIBRARY_CLNTSH
    NAMES libclntsh clntsh oci
    PATHS ${ORACLE_LIB_LOCATION}
)
FIND_LIBRARY(
    ORACLE_LIBRARY_LNNZ
    NAMES libnnz10 nnz10 libnnz11 nnz11 ociw32
    PATHS ${ORACLE_LIB_LOCATION}
)

SET (ORACLE_LIBRARY ${ORACLE_LIBRARY_OCCI} ${ORACLE_LIBRARY_CLNTSH} ${ORACLE_LIBRARY_LNNZ})

IF (ORACLE_LIBRARY)
    SET(ORACLE_LIBRARIES ${ORACLE_LIBRARY})
    SET(ORACLE_FOUND 1)
ENDIF (ORACLE_LIBRARY)


# guess OCI version
IF (NOT DEFINED ORACLE_OCI_VERSION AND UNIX)
    FIND_PROGRAM(AWK awk)
    SET (sqlplus_version "${ORACLE_HOME}/bin/sqlplus -version | ${AWK} '/Release/ {print $3}'")
    EXEC_PROGRAM(${sqlplus_version} OUTPUT_VARIABLE sqlplus_out)
    MESSAGE(STATUS "found sqlplus version: ${sqlplus_out}")

    # WARNING!
    # MATCHES operator is using Cmake regular expression.
    # so the e.g. 9.* does not expand like shell file mask
    # but as "9 and then any sequence of characters"
    IF (${sqlplus_out} MATCHES "8.*")
        SET(ORACLE_OCI_VERSION "8I")
    ELSEIF (${sqlplus_out} MATCHES "9.*")
        SET(ORACLE_OCI_VERSION "9")
# do not change the order of the ora10 checking!
    ELSEIF (${sqlplus_out} MATCHES "10.2.*")
        SET(ORACLE_OCI_VERSION "10G_R2")
    ELSEIF (${sqlplus_out} MATCHES "10.*")
        SET(ORACLE_OCI_VERSION "10G")
    ELSEIF (${sqlplus_out} MATCHES "11.*")
        SET(ORACLE_OCI_VERSION "11G")
    ELSE (${sqlplus_out} MATCHES "8.*")
        SET(ORACLE_OCI_VERSION "10G_R2")
    ENDIF (${sqlplus_out} MATCHES "8.*")

    MESSAGE(STATUS "Guessed ORACLE_OCI_VERSION value: ${ORACLE_OCI_VERSION}")
ENDIF (NOT DEFINED ORACLE_OCI_VERSION AND UNIX)


IF (ORACLE_FOUND)
    IF (NOT ORACLE_FIND_QUIETLY)
         MESSAGE(STATUS "Found Oracle: ${ORACLE_LIBRARY} (ORACLE_HOME='${ORACLE_HOME}')")
    ENDIF (NOT ORACLE_FIND_QUIETLY)
    # there *must* be OCI version defined for internal libraries
    IF (ORACLE_OCI_VERSION)
        ADD_DEFINITIONS(-DOTL_ORA${ORACLE_OCI_VERSION})
    ELSE (ORACLE_OCI_VERSION)
        MESSAGE(FATAL_ERROR "Set -DORACLE_OCI_VERSION for your oci. [8, 8I, 9I, 10G, 10G_R2]")
    ENDIF (ORACLE_OCI_VERSION)

ELSE (ORACLE_FOUND)
    MESSAGE(STATUS "Oracle not found.")
    MESSAGE(STATUS "Oracle: You can specify includes: -DORACLE_PATH_INCLUDES=/usr/include/oracle/10.2.0.3/client")
    MESSAGE(STATUS "   currently found includes: ${ORACLE_INCLUDES}")
    MESSAGE(STATUS "Oracle: You can specify libs: -DORACLE_PATH_LIB=/usr/lib/oracle/10.2.0.3/client/lib")
    MESSAGE(STATUS "   currently found libs: ${ORACLE_LIBRARY}")
    IF (ORACLE_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find Oracle library")
    ELSE (ORACLE_FIND_REQUIRED)
        # setup the variables for silent continue
        SET (ORACLE_INCLUDES "")
    ENDIF (ORACLE_FIND_REQUIRED)
ENDIF (ORACLE_FOUND)

MACRO (PREPROCESS_ORACLE_FILES INFILES INCLUDE_DIRS_IN)
 
  set(SYS_INCLUDE "'sys_include=(${ORACLE_HOME}/precomp/public,/usr/include/,/usr/local/gcc3.2.3/lib/gcc-lib/i686-pc-linux-gnu/3.2.3/include,/usr/local/gcc3.2.3/lib/gcc-lib/i686-pc-linux-gnu,/usr/include/g++-3,/usr/include/c++/3.2/backward,/usr/include/c++/3.2)'")

#  set(INCLUDE_DIRS "
#      include=${ORACLE_HOME}/precomp/public
#      include=${ORACLE_HOME}/rdbms/public
#      include=${ORACLE_HOME}/rdbms/demo
#      include=${ORACLE_HOME}/plsql/public
#      include=${ORACLE_HOME}/network/public
#  ")

  set(INCLUDE_DIRS)

  foreach (_current_FILE ${INCLUDE_DIRS_IN})
    set(INCLUDE_DIRS ${INCLUDE_DIRS} include=${_current_FILE})   
  endforeach (_current_FILE ${INCLUDE_DIRS_IN})

  SET(PROCFLAGS oraca=yes code=cpp parse=partial sqlcheck=semantics ireclen=130 oreclen=130 ${INCLUDE_DIRS})
# ${SYS_INCLUDE} ${INCLUDE_DIRS})
 

#  MESSAGE("PROCFLAGS: ${PROCFLAGS}")
#  MESSAGE("INCLUDE_DIRS: ${INCLUDE_DIRS}")


  foreach (_current_FILE ${INFILES})
    GET_FILENAME_COMPONENT(OUTFILE_NAME ${_current_FILE} NAME_WE)
    set(OUTFILE "${OUTFILE_NAME}.cxx")
    ADD_CUSTOM_COMMAND(OUTPUT ${OUTFILE} 
     COMMAND $ENV{ORACLE_HOME}/bin/proc ARGS iname=${CMAKE_CURRENT_SOURCE_DIR}/${_current_FILE} oname=${CMAKE_CURRENT_BINARY_DIR}/${OUTFILE} ${PROCFLAGS} DEPENDS ${_current_FILE})
  endforeach (_current_FILE ${INFILES})

ENDMACRO (PREPROCESS_ORACLE_FILES)
