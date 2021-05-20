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