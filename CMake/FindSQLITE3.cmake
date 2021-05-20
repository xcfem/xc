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




