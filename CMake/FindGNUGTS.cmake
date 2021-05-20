# Try to find gnu triangulation library GTS
# See 
# http://gts.sf.net
#
# Once run this will define: 
# 
# GTS_FOUND       = system has GTS lib
#
# GTS_LIBRARIES   = full path to the libraries
#    on Unix/Linux with additional linker flags from "gts-config --libs"
# 
# GNUGTS_INCLUDE_DIR      = where to find headers 

SET(GNUGTS_FOUND 0)
FIND_PATH( GNUGTS_INCLUDE_DIR gts.h
  PATHS /usr/include
  PATHS /usr/local/include
)

FIND_LIBRARY(GNUGTS_LIBRARIES gts 
  PATHS /usr/lib
  PATHS /usr/local/lib
  )

IF(GNUGTS_INCLUDE_DIR)
  MESSAGE(STATUS "GNUGTS include dir: " ${GNUGTS_INCLUDE_DIR})
ELSE(GNUGTS_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "No se encontró gts.h")
ENDIF(GNUGTS_INCLUDE_DIR)

IF(GNUGTS_LIBRARIES)
  MESSAGE(STATUS "GNUGTS libraries: " ${GNUGTS_LIBRARIES})
ELSE(GNUGTS_LIBRARIES)
  MESSAGE(FATAL_ERROR "No se encontró libgts")
ENDIF(GNUGTS_LIBRARIES)

IF(GNUGTS_LIBRARIES)
  IF(GNUGTS_INCLUDE_DIR)

    SET(GNUGTS_FOUND 1)
    
  ENDIF(GNUGTS_INCLUDE_DIR)
ENDIF(GNUGTS_LIBRARIES)

