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

