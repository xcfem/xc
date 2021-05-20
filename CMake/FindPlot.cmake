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
